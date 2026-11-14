import { useCallback, useEffect, useState } from 'react'

import { useUserChannel } from '@/hooks/use-user-channel'
import { api, getAuthToken } from '@/lib/api-client'
import type { components } from '@/types/api'

export type Friend = components['schemas']['Friend']
export type FriendRequest = components['schemas']['FriendRequest']
export type UserPublic = components['schemas']['UserPublic']

export interface SentFriendRequest {
  targetUser: UserPublic
  sentAt: string
}

const SENT_REQUESTS_KEY_PREFIX = 'word_wolf_sent_friend_requests_'

function getSentRequestsKey(): string {
  const token = getAuthToken() || 'guest'
  return `${SENT_REQUESTS_KEY_PREFIX}${token}`
}

function loadSentRequestsFromStorage(): SentFriendRequest[] {
  try {
    const raw = localStorage.getItem(getSentRequestsKey())
    return raw ? (JSON.parse(raw) as SentFriendRequest[]) : []
  } catch {
    return []
  }
}

function saveSentRequestsToStorage(requests: SentFriendRequest[]) {
  try {
    localStorage.setItem(getSentRequestsKey(), JSON.stringify(requests))
  } catch {
    // ignore
  }
}

export function useFriends() {
  const [friends, setFriends] = useState<Friend[]>([])
  const [friendRequests, setFriendRequests] = useState<FriendRequest[]>([])
  const [sentRequests, setSentRequests] = useState<SentFriendRequest[]>(loadSentRequestsFromStorage)
  const [searchResults, setSearchResults] = useState<UserPublic[]>([])
  const [isSearching, setIsSearching] = useState(false)
  const [isLoading, setIsLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)

  const fetchFriendsAndRequests = useCallback(async (retryCount = 0) => {
    const token = getAuthToken()
    if (!token) {
      setFriends([])
      setFriendRequests([])
      setSentRequests([])
      setIsLoading(false)
      return
    }

    setIsLoading(true)
    setError(null)

    try {
      const [friendsRes, incomingRes, outgoingRes] = await Promise.all([
        api.GET('/friends'),
        api.GET('/friends/requests', { params: { query: { direction: 'incoming' } } }),
        api.GET('/friends/requests', { params: { query: { direction: 'outgoing' } } }),
      ])

      // サーバー起動中（502や接続エラー等）の場合は最大3回まで自動リトライ（2秒おき）
      if (
        (friendsRes.error && (!friendsRes.response || friendsRes.response.status >= 500)) &&
        retryCount < 3
      ) {
        setTimeout(() => void fetchFriendsAndRequests(retryCount + 1), 2000)
        return
      }

      let loadedFriends: Friend[] = []
      if (friendsRes.data?.status === 'ok') {
        loadedFriends = friendsRes.data.data.items
        setFriends(loadedFriends)
      } else if (friendsRes.error) {
        setError(friendsRes.error.error.message || 'フレンド一覧の取得に失敗しました')
      }

      if (incomingRes.data?.status === 'ok') {
        setFriendRequests(incomingRes.data.data.items)
      }

      if (outgoingRes.data?.status === 'ok') {
        const serverSent: SentFriendRequest[] = outgoingRes.data.data.items
          .filter((item) => item.to !== undefined)
          .map((item) => ({
            targetUser: item.to!,
            sentAt: item.sentAt,
          }))
        setSentRequests(serverSent)
        saveSentRequestsToStorage(serverSent)
      } else {
        // サーバー取得に失敗した場合はローカルキャッシュで補完
        const currentSent = loadSentRequestsFromStorage()
        const activeSent = currentSent.filter(
          (sent) => !loadedFriends.some((f) => f.user.userId === sent.targetUser.userId),
        )
        setSentRequests(activeSent)
        saveSentRequestsToStorage(activeSent)
      }
    } catch {
      if (retryCount < 3) {
        setTimeout(() => void fetchFriendsAndRequests(retryCount + 1), 2000)
        return
      }
      setError('通信エラーが発生しました')
    } finally {
      setIsLoading(false)
    }
  }, [])

  useEffect(() => {
    fetchFriendsAndRequests()
  }, [fetchFriendsAndRequests])

  // フレンドのオンライン状態のライブ更新。GET /friends は開いた時点のスナップショットなので、
  // 以降の変化は user チャンネルの friend.presence_changed で追いかける（契約 asyncapi.yaml）。
  const { on } = useUserChannel()
  useEffect(() => {
    return on('friend.presence_changed', ({ userId, state }) => {
      setFriends((prev) =>
        prev.map((friend) =>
          friend.user.userId === userId
            ? {
                ...friend,
                presence: {
                  state,
                  lastSeenAt: state === 'OFFLINE' ? new Date().toISOString() : null,
                },
              }
            : friend,
        ),
      )
    })
  }, [on])

  const searchUsers = useCallback(async (query: string): Promise<UserPublic[]> => {
    const cleanQuery = query.trim()
    if (cleanQuery.length < 2) {
      setSearchResults([])
      return []
    }

    setIsSearching(true)
    const { data, error: apiError } = await api.GET('/users', {
      params: { query: { q: cleanQuery } },
    })
    setIsSearching(false)

    if (data?.status === 'ok') {
      setSearchResults(data.data.items)
      return data.data.items
    }

    if (apiError) {
      setError(apiError.error.message || 'ユーザー検索に失敗しました')
    }

    setSearchResults([])
    return []
  }, [])

  const clearSearchResults = useCallback(() => {
    setSearchResults([])
  }, [])

  const sendFriendRequest = useCallback(
    async (targetUser: UserPublic | string): Promise<{ success: boolean; error?: string }> => {
      setError(null)
      const targetUserId = typeof targetUser === 'string' ? targetUser : targetUser.userId
      const { data, error: apiError } = await api.POST('/friends/requests', {
        body: { targetUserId },
      })

      if (apiError) {
        let msg = apiError.error.message || 'フレンド申請に失敗しました'
        if (apiError.error.code === 'ALREADY_SUBMITTED') {
          msg = '既にフレンド申請済み、または既にフレンドです。'
        }
        return { success: false, error: msg }
      }

      if (data?.status === 'ok') {
        if (typeof targetUser !== 'string') {
          const newSent: SentFriendRequest = {
            targetUser,
            sentAt: new Date().toISOString(),
          }
          setSentRequests((prev) => {
            const next = [newSent, ...prev.filter((r) => r.targetUser.userId !== targetUserId)]
            saveSentRequestsToStorage(next)
            return next
          })
        }
        return { success: true }
      }

      return { success: false, error: '予期せぬエラーが発生しました' }
    },
    [],
  )

  const acceptFriendRequest = useCallback(
    async (requestId: string): Promise<{ success: boolean; error?: string }> => {
      setError(null)
      const { data, error: apiError } = await api.POST('/friends/requests/{requestId}/accept', {
        params: { path: { requestId } },
      })

      if (apiError) {
        return { success: false, error: apiError.error.message || '承認に失敗しました' }
      }

      if (data?.status === 'ok') {
        // 受信リストから除外してフレンドリストに追加
        setFriendRequests((prev) => prev.filter((r) => r.requestId !== requestId))
        setFriends((prev) => [data.data, ...prev])
        return { success: true }
      }

      return { success: false, error: '予期せぬエラーが発生しました' }
    },
    [],
  )

  const declineFriendRequest = useCallback(
    async (requestId: string): Promise<{ success: boolean; error?: string }> => {
      setError(null)
      const { data, error: apiError } = await api.POST('/friends/requests/{requestId}/decline', {
        params: { path: { requestId } },
      })

      if (apiError) {
        return { success: false, error: apiError.error.message || '拒否に失敗しました' }
      }

      if (data?.status === 'ok') {
        setFriendRequests((prev) => prev.filter((r) => r.requestId !== requestId))
        return { success: true }
      }

      return { success: false, error: '予期せぬエラーが発生しました' }
    },
    [],
  )

  const removeFriend = useCallback(
    async (userId: string): Promise<{ success: boolean; error?: string }> => {
      setError(null)
      const { data, error: apiError } = await api.DELETE('/friends/{userId}', {
        params: { path: { userId } },
      })

      // NOT_FOUND は相手側から先に解除済み。一覧から消せば表示は正しい状態になる
      if (data?.status === 'ok' || apiError?.error.code === 'NOT_FOUND') {
        setFriends((prev) => prev.filter((friend) => friend.user.userId !== userId))
        return { success: true }
      }

      if (apiError) {
        return { success: false, error: apiError.error.message || 'フレンドの解除に失敗しました' }
      }

      return { success: false, error: '予期せぬエラーが発生しました' }
    },
    [],
  )

  return {
    friends,
    friendRequests,
    sentRequests,
    searchResults,
    isSearching,
    isLoading,
    error,
    refresh: fetchFriendsAndRequests,
    searchUsers,
    clearSearchResults,
    sendFriendRequest,
    acceptFriendRequest,
    declineFriendRequest,
    removeFriend,
  }
}
