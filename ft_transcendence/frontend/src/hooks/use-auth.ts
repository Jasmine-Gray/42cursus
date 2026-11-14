// ここは POST /auth/login, /auth/register の呼び出しと、その結果の保持だけを担当する。
// Authorization ヘッダーの付与は lib/api-client.ts の middleware が全リクエストに対して行う。
import { useCallback, useState } from 'react'

import { clearGlobalMeCache, setGlobalMeUser } from '@/hooks/use-me'
import { api, getAuthToken, setAuthToken, setTabAuthToken } from '@/lib/api-client'
import type { components } from '@/types/api'

type AuthUser = components['schemas']['UserPrivate']
type RegisterAttrs = components['schemas']['RegisterRequest']
type ApiErrorBody = components['schemas']['ErrorResponse']

function errorMessage(body: ApiErrorBody | undefined): string {
  switch (body?.error?.code) {
    case 'INVALID_CREDENTIALS':
      return 'メールアドレスまたはパスワードが正しくありません。'
    case 'EMAIL_TAKEN':
      return 'そのメールアドレスは既に登録されています。'
    case 'INVALID_PAYLOAD':
      return '入力内容を確認してください。'
    default:
      return body?.error?.message ?? '通信に失敗しました。時間をおいて再度お試しください。'
  }
}

// トークンの保存先。'browser' = localStorage（通常。全タブ共通）、'tab' = sessionStorage（このタブだけ。開発用 /dev/as）
export type AuthScope = 'browser' | 'tab'

type UseAuthOptions = {
  scope?: AuthScope
}

export function useAuth({ scope = 'browser' }: UseAuthOptions = {}) {
  const writeToken = scope === 'tab' ? setTabAuthToken : setAuthToken
  const [token, setToken] = useState<string | null>(() => getAuthToken())
  const [user, setUser] = useState<AuthUser | null>(null)
  const [isSubmitting, setIsSubmitting] = useState(false)
  const [error, setError] = useState<string | null>(null)

  const login = useCallback(async (email: string, password: string): Promise<boolean> => {
    setIsSubmitting(true)
    setError(null)

    const { data, error: apiError } = await api.POST('/auth/login', {
      body: { email, password },
    })

    setIsSubmitting(false)

    if (apiError) {
      setError(errorMessage(apiError))
      return false
    }

    writeToken(data.data.token)
    setToken(data.data.token)
    setUser(data.data.user)
    setGlobalMeUser(data.data.user)
    return true
  }, [writeToken])

  const register = useCallback(async (attrs: RegisterAttrs): Promise<boolean> => {
    setIsSubmitting(true)
    setError(null)

    const { data, error: apiError } = await api.POST('/auth/register', {
      body: attrs,
    })

    setIsSubmitting(false)

    if (apiError) {
      setError(errorMessage(apiError))
      return false
    }

    writeToken(data.data.token)
    setToken(data.data.token)
    setUser(data.data.user)
    setGlobalMeUser(data.data.user)
    return true
  }, [writeToken])

  const logout = useCallback(async (): Promise<void> => {
    // サーバー側セッションを失効させる。401（既に失効）や通信断でもローカルの後始末は必ず行う。
    try {
      await api.POST('/auth/logout', {})
    } catch {
      // ネットワークエラーは無視してローカルログアウトを続行する
    }
    writeToken(null)
    setToken(null)
    setUser(null)
    clearGlobalMeCache()
  }, [writeToken])

  return { scope, token, user, isAuthenticated: token !== null, isSubmitting, error, login, register, logout }
}
