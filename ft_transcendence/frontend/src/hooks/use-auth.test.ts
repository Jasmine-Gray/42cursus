import { act, renderHook } from '@testing-library/react'
import { beforeEach, describe, expect, it, vi } from 'vitest'

import { api, setAuthToken, setTabAuthToken } from '@/lib/api-client'

import { useAuth } from './use-auth'

vi.mock('@/lib/api-client', () => ({
  api: { POST: vi.fn() },
  getAuthToken: vi.fn(() => null),
  setAuthToken: vi.fn(),
  setTabAuthToken: vi.fn(),
}))

const postMock = vi.mocked(api.POST)
const setAuthTokenMock = vi.mocked(setAuthToken)
const setTabAuthTokenMock = vi.mocked(setTabAuthToken)

const sessionUser = {
  userId: 'user_1',
  displayName: '田中 ユウ',
  avatarUrl: '/avatars/yu.webp',
  email: 'yu@example.com',
}

type PostResult = Awaited<ReturnType<typeof api.POST>>

function resolveOk(token: string) {
  postMock.mockResolvedValue({
    data: { status: 'ok', data: { token, user: sessionUser } },
    response: new Response(null, { status: 200 }),
  } as PostResult)
}

function resolveError(code: string, status: number) {
  postMock.mockResolvedValue({
    error: { status: 'error', error: { code, message: 'server message' } },
    response: new Response(null, { status }),
  } as PostResult)
}

beforeEach(() => {
  postMock.mockReset()
  setAuthTokenMock.mockReset()
  setTabAuthTokenMock.mockReset()
})

describe('useAuth', () => {
  it('ログイン成功でトークンを api-client の保存先に書き、認証済みになる', async () => {
    resolveOk('tok-login')
    const { result } = renderHook(() => useAuth())

    expect(result.current.isAuthenticated).toBe(false)

    let ok = false
    await act(async () => {
      ok = await result.current.login('yu@example.com', 'password123')
    })

    expect(ok).toBe(true)
    expect(postMock).toHaveBeenCalledWith('/auth/login', {
      body: { email: 'yu@example.com', password: 'password123' },
    })
    expect(setAuthTokenMock).toHaveBeenCalledWith('tok-login')
    expect(result.current.token).toBe('tok-login')
    expect(result.current.user?.displayName).toBe('田中 ユウ')
    expect(result.current.isAuthenticated).toBe(true)
    expect(result.current.error).toBeNull()
  })

  it('登録成功も同じ保存先にトークンを書く', async () => {
    resolveOk('tok-register')
    const { result } = renderHook(() => useAuth())

    await act(async () => {
      await result.current.register({
        displayName: '田中 ユウ',
        email: 'yu@example.com',
        password: 'password123',
        termsAccepted: true,
      })
    })

    expect(postMock).toHaveBeenCalledWith('/auth/register', {
      body: { displayName: '田中 ユウ', email: 'yu@example.com', password: 'password123', termsAccepted: true },
    })
    expect(setAuthTokenMock).toHaveBeenCalledWith('tok-register')
    expect(result.current.isAuthenticated).toBe(true)
  })

  it('INVALID_CREDENTIALS は日本語メッセージに変換し、トークンは書かない', async () => {
    resolveError('INVALID_CREDENTIALS', 401)
    const { result } = renderHook(() => useAuth())

    let ok = true
    await act(async () => {
      ok = await result.current.login('yu@example.com', 'wrong-password')
    })

    expect(ok).toBe(false)
    expect(result.current.error).toBe('メールアドレスまたはパスワードが正しくありません。')
    expect(setAuthTokenMock).not.toHaveBeenCalled()
    expect(result.current.isAuthenticated).toBe(false)
  })

  it('EMAIL_TAKEN は登録済みメッセージになる', async () => {
    resolveError('EMAIL_TAKEN', 409)
    const { result } = renderHook(() => useAuth())

    await act(async () => {
      await result.current.register({
        displayName: '田中 ユウ',
        email: 'yu@example.com',
        password: 'password123',
        termsAccepted: true,
      })
    })

    expect(result.current.error).toBe('そのメールアドレスは既に登録されています。')
  })

  it("scope: 'tab' ではトークンをタブ単位の保存先に書き、ブラウザ共通の保存先には触らない", async () => {
    resolveOk('tok-tab')
    const { result } = renderHook(() => useAuth({ scope: 'tab' }))

    await act(async () => {
      await result.current.login('yu@example.com', 'password123')
    })

    expect(result.current.scope).toBe('tab')
    expect(setTabAuthTokenMock).toHaveBeenCalledWith('tok-tab')
    expect(setAuthTokenMock).not.toHaveBeenCalled()
    expect(result.current.isAuthenticated).toBe(true)

    await act(async () => {
      await result.current.logout()
    })
    expect(setTabAuthTokenMock).toHaveBeenLastCalledWith(null)
    expect(setAuthTokenMock).not.toHaveBeenCalled()
  })

  it('ログアウトで POST /auth/logout を呼び、トークンとユーザーを消す', async () => {
    resolveOk('tok-login')
    const { result } = renderHook(() => useAuth())

    await act(async () => {
      await result.current.login('yu@example.com', 'password123')
    })

    postMock.mockClear()
    await act(async () => {
      await result.current.logout()
    })

    expect(postMock).toHaveBeenCalledWith('/auth/logout', {})
    expect(setAuthTokenMock).toHaveBeenLastCalledWith(null)
    expect(result.current.isAuthenticated).toBe(false)
    expect(result.current.user).toBeNull()
  })

  it('ログアウトの API が失敗してもローカルのトークンは消す', async () => {
    resolveOk('tok-login')
    const { result } = renderHook(() => useAuth())

    await act(async () => {
      await result.current.login('yu@example.com', 'password123')
    })

    postMock.mockRejectedValueOnce(new Error('network down'))
    await act(async () => {
      await result.current.logout()
    })

    expect(setAuthTokenMock).toHaveBeenLastCalledWith(null)
    expect(result.current.isAuthenticated).toBe(false)
  })
})
