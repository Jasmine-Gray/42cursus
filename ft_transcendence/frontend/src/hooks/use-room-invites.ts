import { useCallback, useEffect, useState } from 'react'

import { useUserChannel } from '@/hooks/use-user-channel'
import { api, getAuthToken } from '@/lib/api-client'
import type { components } from '@/types/api'
import type { RoomInvite, RoomRemoved } from '@/types/ws/events'

type RoomState = components['schemas']['RoomState']
type RealtimeTopics = components['schemas']['RealtimeTopics']

type GlobalInvitesState = {
  token: string | null
  invites: RoomInvite[]
  latestNotification: RoomInvite | null
  latestRemoval: RoomRemoved | null
  isLoading: boolean
  error: string | null
}

let globalInvitesState: GlobalInvitesState = {
  token: null,
  invites: [],
  latestNotification: null,
  latestRemoval: null,
  isLoading: false,
  error: null,
}

let notificationTimeoutId: ReturnType<typeof setTimeout> | null = null

function clearNotificationTimer() {
  if (notificationTimeoutId !== null) {
    clearTimeout(notificationTimeoutId)
    notificationTimeoutId = null
  }
}

function startNotificationTimer(inviteId: string) {
  clearNotificationTimer()
  // 20秒後に自動でポップアップ通知を消去
  notificationTimeoutId = setTimeout(() => {
    if (globalInvitesState.latestNotification?.inviteId === inviteId) {
      globalInvitesState = {
        ...globalInvitesState,
        latestNotification: null,
      }
      emitChange()
    }
    notificationTimeoutId = null
  }, 20000)
}

const listeners = new Set<() => void>()

function emitChange() {
  listeners.forEach((listener) => listener())
}

export function useRoomInvites() {
  const currentToken = getAuthToken()
  const { on } = useUserChannel()

  if (globalInvitesState.token !== currentToken) {
    clearNotificationTimer()
    globalInvitesState = {
      token: currentToken,
      invites: [],
      latestNotification: null,
      latestRemoval: null,
      isLoading: false,
      error: null,
    }
  }

  const [state, setState] = useState<GlobalInvitesState>(
    () => globalInvitesState,
  )

  useEffect(() => {
    const handleUpdate = () => setState({ ...globalInvitesState })
    listeners.add(handleUpdate)
    return () => {
      listeners.delete(handleUpdate)
    }
  }, [])

  const refreshInvites = useCallback(async () => {
    const token = getAuthToken()
    if (!token) {
      clearNotificationTimer()
      globalInvitesState = {
        token: null,
        invites: [],
        latestNotification: null,
        latestRemoval: null,
        isLoading: false,
        error: null,
      }
      emitChange()
      return
    }

    try {
      const { data, error } = await api.GET('/invites')
      if (error) {
        globalInvitesState = {
          ...globalInvitesState,
          isLoading: false,
          error: error.error?.message || '招待一覧の取得に失敗しました',
        }
      } else if (data?.status === 'ok') {
        globalInvitesState = {
          ...globalInvitesState,
          token,
          invites: data.data.items,
          isLoading: false,
          error: null,
        }
      }
      emitChange()
    } catch {
      globalInvitesState = {
        ...globalInvitesState,
        isLoading: false,
        error: 'ネットワークエラーが発生しました',
      }
      emitChange()
    }
  }, [])

  // チャンネルからの room.invited イベント受信
  useEffect(() => {
    const unsubscribe = on('room.invited', (invite: RoomInvite) => {
      // 既存リストに無ければ追加
      const exists = globalInvitesState.invites.some(
        (i) => i.inviteId === invite.inviteId,
      )
      const nextInvites = exists
        ? globalInvitesState.invites
        : [invite, ...globalInvitesState.invites]

      clearNotificationTimer()
      globalInvitesState = {
        ...globalInvitesState,
        invites: nextInvites,
        latestNotification: invite,
      }
      emitChange()
      // 20秒タイマーを開始
      startNotificationTimer(invite.inviteId)
    })

    return unsubscribe
  }, [on])

  useEffect(
    () =>
      on('room.removed', (removal: RoomRemoved) => {
        if (!currentToken || currentToken !== getAuthToken()) return
        globalInvitesState = { ...globalInvitesState, latestRemoval: removal }
        emitChange()
      }),
    [on, currentToken],
  )

  const clearRemovalNotification = useCallback(() => {
    globalInvitesState = { ...globalInvitesState, latestRemoval: null }
    emitChange()
  }, [])

  // 初回マウント時および定期同期（15秒毎）
  useEffect(() => {
    if (currentToken) {
      void refreshInvites()
      const interval = setInterval(() => {
        void refreshInvites()
      }, 15000)
      return () => clearInterval(interval)
    }
  }, [currentToken, refreshInvites])

  // フレンドを部屋へ招待
  const inviteFriend = useCallback(
    async (
      roomId: string,
      targetUserId: string,
    ): Promise<{ success: boolean; data?: RoomInvite; error?: string }> => {
      try {
        const { data, error } = await api.POST('/rooms/{roomId}/invites', {
          params: { path: { roomId } },
          body: { targetUserId },
        })

        if (error) {
          const msg = error.error?.message || '招待の送信に失敗しました'
          return { success: false, error: msg }
        }

        if (data?.status === 'ok') {
          return { success: true, data: data.data }
        }

        return { success: false, error: '予期せぬエラーが発生しました' }
      } catch {
        return { success: false, error: '通信エラーが発生しました' }
      }
    },
    [],
  )

  // 招待を承認（参加）
  const acceptInvite = useCallback(
    async (
      inviteId: string,
      participantType: 'PLAYER' | 'SPECTATOR' = 'PLAYER',
    ): Promise<{
      success: boolean
      data?: { room: RoomState; realtime: RealtimeTopics }
      error?: string
    }> => {
      // 参加ボタンが押された時点で即座にポップアップを消去
      if (globalInvitesState.latestNotification?.inviteId === inviteId) {
        clearNotificationTimer()
        globalInvitesState = {
          ...globalInvitesState,
          latestNotification: null,
        }
        emitChange()
      }

      try {
        const { data, error, response } = await api.POST(
          '/invites/{inviteId}/accept',
          {
            params: { path: { inviteId } },
            body: { participantType },
          },
        )

        if (error || !data || data.status !== 'ok') {
          // 404 等で失敗した場合は古い招待を一覧から除外
          globalInvitesState = {
            ...globalInvitesState,
            invites: globalInvitesState.invites.filter(
              (i) => i.inviteId !== inviteId,
            ),
            latestNotification:
              globalInvitesState.latestNotification?.inviteId === inviteId
                ? null
                : globalInvitesState.latestNotification,
          }
          emitChange()
          void refreshInvites()

          if (response?.status === 404) {
            return {
              success: false,
              error: 'この部屋は既に終了しているか存在しません。',
            }
          }
          if (response?.status === 409) {
            return { success: false, error: 'この部屋は満員です。' }
          }
          const msg = error?.error?.message || '参加に失敗しました'
          return { success: false, error: msg }
        }

        globalInvitesState = {
          ...globalInvitesState,
          invites: globalInvitesState.invites.filter(
            (i) => i.inviteId !== inviteId,
          ),
          latestNotification:
            globalInvitesState.latestNotification?.inviteId === inviteId
              ? null
              : globalInvitesState.latestNotification,
        }
        emitChange()
        return { success: true, data: data.data }
      } catch {
        return { success: false, error: '通信エラーが発生しました' }
      }
    },
    [refreshInvites],
  )

  // 招待を辞退
  const declineInvite = useCallback(
    async (inviteId: string): Promise<{ success: boolean; error?: string }> => {
      // 不参加（辞退）ボタンが押された時点で即座にポップアップを消去
      if (globalInvitesState.latestNotification?.inviteId === inviteId) {
        clearNotificationTimer()
        globalInvitesState = {
          ...globalInvitesState,
          latestNotification: null,
        }
        emitChange()
      }

      try {
        const { data, error } = await api.POST('/invites/{inviteId}/decline', {
          params: { path: { inviteId } },
        })

        if (error) {
          const msg = error.error?.message || '辞退に失敗しました'
          return { success: false, error: msg }
        }

        if (data?.status === 'ok') {
          globalInvitesState = {
            ...globalInvitesState,
            invites: globalInvitesState.invites.filter(
              (i) => i.inviteId !== inviteId,
            ),
            latestNotification:
              globalInvitesState.latestNotification?.inviteId === inviteId
                ? null
                : globalInvitesState.latestNotification,
          }
          emitChange()
          return { success: true }
        }

        return { success: false, error: '予期せぬエラーが発生しました' }
      } catch {
        return { success: false, error: '通信エラーが発生しました' }
      }
    },
    [],
  )

  const clearLatestNotification = useCallback(() => {
    clearNotificationTimer()
    globalInvitesState = {
      ...globalInvitesState,
      latestNotification: null,
    }
    emitChange()
  }, [])

  return {
    invites: state.invites,
    unreadCount: state.invites.length,
    latestNotification: state.latestNotification,
    latestRemoval: state.latestRemoval,
    clearRemovalNotification,
    isLoading: state.isLoading,
    error: state.error,
    refreshInvites,
    inviteFriend,
    acceptInvite,
    declineInvite,
    clearLatestNotification,
  }
}
