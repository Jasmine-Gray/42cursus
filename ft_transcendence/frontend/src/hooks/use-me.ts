// ログイン/登録は hooks/use-auth.ts。認証トークンは lib/api-client.ts の middleware が付ける。
import { useCallback, useEffect, useState } from 'react'

import { DEFAULT_AVATAR_URL } from '@/data/avatar-options'
import { api, getAuthToken, setAuthToken } from '@/lib/api-client'
import type { components } from '@/types/api'

type UserPrivate = components['schemas']['UserPrivate']
type UserStats = components['schemas']['UserStats']
type RoomPreview = components['schemas']['RoomPreview']

type MutationResult = { success: true; user: UserPrivate } | { success: false; error: string }

interface GlobalMeState {
  token: string | null
  user: UserPrivate | null
  stats: UserStats | null
  friendCount: number
  activeRoom: RoomPreview | null
  isLoading: boolean
  error: string | null
}

let globalState: GlobalMeState = {
  token: null,
  user: null,
  stats: null,
  friendCount: 0,
  activeRoom: null,
  isLoading: false,
  error: null,
}

type Listener = () => void
const listeners = new Set<Listener>()

function emitChange() {
  listeners.forEach((listener) => listener())
}

/**
 * 外部（useAuth のログイン/登録/ログアウト等）からグローバルユーザー情報を安全に同期/クリアする関数
 */
export function setGlobalMeUser(user: UserPrivate | null): void {
  globalState = {
    ...globalState,
    token: getAuthToken(),
    user,
    isLoading: false,
    error: null,
  }
  emitChange()
}

export function clearGlobalMeCache(): void {
  globalState = {
    token: null,
    user: null,
    stats: null,
    friendCount: 0,
    activeRoom: null,
    isLoading: false,
    error: null,
  }
  emitChange()
}

export function getGlobalMeUser(): UserPrivate | null {
  const currentToken = getAuthToken()
  if (globalState.token !== currentToken) {
    return null
  }
  return globalState.user
}

export function useMe() {
  const currentToken = getAuthToken()

  // トークンが変更された場合は安全にステートを同期・再初期化
  if (globalState.token !== currentToken) {
    globalState = {
      token: currentToken,
      user: null,
      stats: null,
      friendCount: 0,
      activeRoom: null,
      isLoading: false,
      error: null,
    }
  }

  const [state, setState] = useState<GlobalMeState>(() => globalState)

  useEffect(() => {
    const handleUpdate = () => {
      setState({ ...globalState })
    }
    listeners.add(handleUpdate)
    return () => {
      listeners.delete(handleUpdate)
    }
  }, [])

  const refreshMe = useCallback(async (retryCount = 0) => {
    const token = getAuthToken()
    if (!token) {
      clearGlobalMeCache()
      return
    }

    if (!globalState.user) {
      globalState = { ...globalState, isLoading: true, error: null }
      emitChange()
    }

    try {
      const { data, error: apiError, response } = await api.GET('/users/me')
      if (response?.status === 401) {
        clearGlobalMeCache()
        return
      }

      if (apiError) {
        if (retryCount < 3 && (!response || response.status >= 500)) {
          setTimeout(() => void refreshMe(retryCount + 1), 2000)
          return
        }
        const msg = apiError.error?.message || 'ユーザー情報の取得に失敗しました'
        globalState = { ...globalState, error: msg, isLoading: false }
        emitChange()
      } else if (data?.status === 'ok') {
        globalState = {
          token,
          user: data.data.user,
          stats: data.data.stats,
          friendCount: data.data.friendCount,
          activeRoom: data.data.activeRoom ?? null,
          isLoading: false,
          error: null,
        }
        emitChange()
      }
    } catch (err) {
      if (retryCount < 3) {
        setTimeout(() => void refreshMe(retryCount + 1), 2000)
        return
      }
      const msg = err instanceof Error ? err.message : 'ネットワークエラーが発生しました'
      globalState = { ...globalState, error: msg, isLoading: false }
      emitChange()
    }
  }, [])

  useEffect(() => {
    if (currentToken && !globalState.user && !globalState.isLoading) {
      void refreshMe()
    }
  }, [currentToken, refreshMe])

  // アバター変更（プリセット）: 即座にグローバルステートを書き換え
  const updateAvatarPreset = useCallback(async (presetId: string): Promise<MutationResult> => {
    globalState = { ...globalState, error: null }
    emitChange()

    const { data, error: apiError } = await api.PUT('/users/me/avatar', {
      body: { presetId },
    })

    if (apiError) {
      const msg = apiError.error.message || 'アバターの更新に失敗しました'
      globalState = { ...globalState, error: msg }
      emitChange()
      return { success: false, error: msg }
    }

    if (data?.status === 'ok') {
      // 保持している global status を書き換えて全コンポーネントに反映
      globalState = {
        ...globalState,
        token: getAuthToken(),
        user: data.data,
        error: null,
      }
      emitChange()
      return { success: true, user: data.data }
    }

    return { success: false, error: '予期せぬレスポンスです' }
  }, [])

  // アバター変更（画像アップロード）: 即座にグローバルステートを書き換え
  const uploadAvatarFile = useCallback(async (file: File): Promise<MutationResult> => {
    globalState = { ...globalState, error: null }
    emitChange()

    try {
      const { data, error: apiError } = await api.PUT('/users/me/avatar', {
        body: { file: file.name },
        bodySerializer: () => {
          const formData = new FormData()
          formData.append('file', file)
          return formData
        },
      })

      if (apiError) {
        const msg = apiError.error.message || '画像のアップロードに失敗しました'
        globalState = { ...globalState, error: msg }
        emitChange()
        return { success: false, error: msg }
      }

      if (data?.status === 'ok') {
        // 保持している global status を書き換えて全コンポーネントに反映
        globalState = {
          ...globalState,
          token: getAuthToken(),
          user: data.data,
          error: null,
        }
        emitChange()
        return { success: true, user: data.data }
      }

      return { success: false, error: '予期せぬレスポンスです' }
    } catch (err) {
      const msg = err instanceof Error ? err.message : 'ネットワークエラーが発生しました'
      globalState = { ...globalState, error: msg }
      emitChange()
      return { success: false, error: msg }
    }
  }, [])

  // プロフィール更新: 即座にグローバルステートを書き換え
  const updateProfile = useCallback(async (displayName: string, bio?: string): Promise<MutationResult> => {
    globalState = { ...globalState, error: null }
    emitChange()

    const { data, error: apiError } = await api.PUT('/users/me', {
      body: { displayName, bio: bio ?? '' },
    })

    if (apiError) {
      const msg = apiError.error.message || 'プロフィールの更新に失敗しました'
      globalState = { ...globalState, error: msg }
      emitChange()
      return { success: false, error: msg }
    }

    if (data?.status === 'ok') {
      globalState = {
        ...globalState,
        token: getAuthToken(),
        user: data.data,
        error: null,
      }
      emitChange()
      return { success: true, user: data.data }
    }

    return { success: false, error: '予期せぬレスポンスです' }
  }, [])

  // アカウント削除: 成功したらトークンとグローバル状態を破棄する（以降 GET /users/me は 401）
  const deleteAccount = useCallback(async (): Promise<
    { success: true } | { success: false; error: string }
  > => {
    const { error: apiError } = await api.DELETE('/users/me', {})

    if (apiError) {
      const msg = apiError.error.message || 'アカウントの削除に失敗しました'
      globalState = { ...globalState, error: msg }
      emitChange()
      return { success: false, error: msg }
    }

    setAuthToken(null)
    clearGlobalMeCache()
    return { success: true }
  }, [])

  const avatarUrl = state.user?.avatarUrl || DEFAULT_AVATAR_URL

  return {
    user: state.user,
    avatarUrl,
    stats: state.stats,
    friendCount: state.friendCount,
    activeRoom: state.activeRoom,
    isLoading: state.isLoading,
    error: state.error,
    refreshMe,
    updateAvatarPreset,
    uploadAvatarFile,
    updateProfile,
    deleteAccount,
  }
}
