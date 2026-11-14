import { act, fireEvent, render, screen, waitFor } from '@testing-library/react'
import { beforeEach, describe, expect, it, vi } from 'vitest'
import { useParams } from 'wouter'

import { useAuth } from '@/hooks/use-auth'
import { getTabAuthToken, setTabAuthToken } from '@/lib/api-client'

import { createSocket } from '@/lib/channel-client'

import { DevLoginPage, describeSocketProbe, describeTabToken, devSeedUsers, devTokenFor } from './dev-login-page'

const navigateMock = vi.fn()
const loginMock = vi.fn()

vi.mock('wouter', () => ({
  Link: ({ href, children, className }: { href: string; children: React.ReactNode; className?: string }) => (
    <a href={href} className={className}>
      {children}
    </a>
  ),
  useLocation: () => ['/dev/as', navigateMock],
  useParams: vi.fn(() => ({})),
}))

vi.mock('@/lib/api-client', () => ({
  getAuthToken: vi.fn(() => null),
  getTabAuthToken: vi.fn(() => null),
  setTabAuthToken: vi.fn(),
}))

type Handler = () => void
const fakeSocket = {
  handlers: {} as Partial<Record<'open' | 'error' | 'close', Handler>>,
  onOpen(fn: Handler) {
    this.handlers.open = fn
  },
  onError(fn: Handler) {
    this.handlers.error = fn
  },
  onClose(fn: Handler) {
    this.handlers.close = fn
  },
  connect: vi.fn(),
  disconnect: vi.fn(),
}

vi.mock('@/lib/channel-client', () => ({
  createSocket: vi.fn(() => fakeSocket),
  socketEndpoint: () => '/socket',
}))

vi.mock('@/hooks/use-auth', () => ({
  useAuth: vi.fn(),
}))

const useParamsMock = vi.mocked(useParams)
const createSocketMock = vi.mocked(createSocket)
const useAuthMock = vi.mocked(useAuth)
const getTabAuthTokenMock = vi.mocked(getTabAuthToken)
const setTabAuthTokenMock = vi.mocked(setTabAuthToken)

function stubAuth(overrides: Partial<ReturnType<typeof useAuth>> = {}) {
  useAuthMock.mockReturnValue({
    scope: 'tab',
    token: null,
    user: null,
    isAuthenticated: false,
    isSubmitting: false,
    error: null,
    login: loginMock,
    register: vi.fn(),
    logout: vi.fn(),
    ...overrides,
  } as ReturnType<typeof useAuth>)
}

beforeEach(() => {
  navigateMock.mockReset()
  loginMock.mockReset()
  setTabAuthTokenMock.mockReset()
  createSocketMock.mockClear()
  fakeSocket.connect.mockClear()
  fakeSocket.disconnect.mockClear()
  fakeSocket.handlers = {}
  getTabAuthTokenMock.mockReturnValue(null)
  useParamsMock.mockReturnValue({})
  stubAuth()
})

describe('describeTabToken', () => {
  it('シードのトークンは名前、登録ユーザーのトークンは「登録ユーザー」、無ければ null', () => {
    expect(describeTabToken(null)).toBeNull()
    expect(describeTabToken('dev-token-minato')).toBe('シード: ミナト（minato）')
    expect(describeTabToken('dev-token-unknown')).toBe('シード: unknown')
    expect(describeTabToken('miBe8qeofhGJ2z5v')).toMatch(/登録ユーザー/)
  })
})

describe('DevLoginPage /dev/as', () => {
  it('このタブの状態、シード一覧、登録ユーザーのフォームを出す', () => {
    render(<DevLoginPage />)

    expect(screen.getByRole('heading', { name: '開発用ログイン（このタブだけ）' })).toBeInTheDocument()
    expect(screen.getByText(/未設定/)).toBeInTheDocument()
    for (const u of devSeedUsers) {
      expect(screen.getByRole('link', { name: new RegExp(u.label) })).toHaveAttribute('href', `/dev/as/${u.name}`)
    }
    expect(screen.getByLabelText('メールアドレス')).toBeInTheDocument()
    expect(screen.getByRole('button', { name: 'このタブだけログイン' })).toBeDisabled()
    expect(useAuthMock).toHaveBeenCalledWith({ scope: 'tab' })
  })

  it('タブにトークンがあれば誰かを表示し、「このタブだけログアウト」で消す', () => {
    getTabAuthTokenMock.mockReturnValue('dev-token-aoi')
    render(<DevLoginPage />)

    expect(screen.getByText('シード: アオイ（aoi）')).toBeInTheDocument()
    fireEvent.click(screen.getByRole('button', { name: /このタブだけログアウト/ }))

    expect(setTabAuthTokenMock).toHaveBeenCalledWith(null)
    expect(screen.getByText(/未設定/)).toBeInTheDocument()
  })

  it('登録済みユーザーのフォームは useAuth(scope: tab).login を呼び、成功で /home へ', async () => {
    loginMock.mockResolvedValue(true)
    render(<DevLoginPage />)

    fireEvent.change(screen.getByLabelText('メールアドレス'), { target: { value: ' me@example.com ' } })
    fireEvent.change(screen.getByLabelText('パスワード'), { target: { value: 'password123' } })
    fireEvent.click(screen.getByRole('button', { name: 'このタブだけログイン' }))

    await waitFor(() => expect(loginMock).toHaveBeenCalledWith('me@example.com', 'password123'))
    await waitFor(() => expect(navigateMock).toHaveBeenCalledWith('/home'))
  })

  it('ログイン失敗時は useAuth の error を表示し、遷移しない', async () => {
    loginMock.mockResolvedValue(false)
    stubAuth({ error: 'メールアドレスまたはパスワードが正しくありません。' })
    render(<DevLoginPage />)

    fireEvent.change(screen.getByLabelText('メールアドレス'), { target: { value: 'me@example.com' } })
    fireEvent.change(screen.getByLabelText('パスワード'), { target: { value: 'wrong' } })
    fireEvent.click(screen.getByRole('button', { name: 'このタブだけログイン' }))

    await waitFor(() => expect(loginMock).toHaveBeenCalled())
    expect(screen.getByRole('alert')).toHaveTextContent('正しくありません')
    expect(navigateMock).not.toHaveBeenCalled()
  })
})

describe('WebSocket 接続テスト', () => {
  it('「この本人で接続」で createSocket().connect() を呼び、onOpen で接続済みになる', () => {
    render(<DevLoginPage />)
    expect(screen.getByTestId('socket-state')).toHaveTextContent('未接続')

    fireEvent.click(screen.getByRole('button', { name: /この本人で接続/ }))
    expect(createSocketMock).toHaveBeenCalledTimes(1)
    expect(fakeSocket.connect).toHaveBeenCalledTimes(1)
    expect(screen.getByTestId('socket-state')).toHaveTextContent('接続中')

    act(() => fakeSocket.handlers.open?.())
    expect(screen.getByTestId('socket-state')).toHaveTextContent('接続済み')

    fireEvent.click(screen.getByRole('button', { name: /切断/ }))
    expect(fakeSocket.disconnect).toHaveBeenCalled()
    expect(screen.getByTestId('socket-state')).toHaveTextContent('未接続')
  })

  it('onError（403）では再接続を止めて「拒否された」と表示する', () => {
    render(<DevLoginPage />)
    fireEvent.click(screen.getByRole('button', { name: /この本人で接続/ }))

    act(() => fakeSocket.handlers.error?.())
    expect(fakeSocket.disconnect).toHaveBeenCalled()
    expect(screen.getByTestId('socket-state')).toHaveTextContent('拒否された')
  })

  it('describeSocketProbe は全状態に文言を持つ', () => {
    for (const st of ['idle', 'connecting', 'open', 'error', 'closed'] as const) {
      expect(describeSocketProbe(st)).not.toBe('')
    }
  })
})

describe('DevLoginPage /dev/as/<name>', () => {
  it('name ありならこのタブのトークンを dev-token-<name> にして /home へ置き換え遷移する', () => {
    useParamsMock.mockReturnValue({ name: 'minato' })
    render(<DevLoginPage />)

    expect(setTabAuthTokenMock).toHaveBeenCalledWith(devTokenFor('minato'))
    expect(navigateMock).toHaveBeenCalledWith('/home', { replace: true })
  })

  it('シードにいない name はトークンを書かず、エラー表示つきの一覧に戻す', () => {
    useParamsMock.mockReturnValue({ name: 'nobody' })
    render(<DevLoginPage />)

    expect(screen.getByRole('alert')).toHaveTextContent('nobody はシードにいません')
    expect(setTabAuthTokenMock).not.toHaveBeenCalled()
    expect(navigateMock).not.toHaveBeenCalled()
  })
})
