import { fireEvent, render, screen, waitFor, within } from '@testing-library/react'
import { beforeEach, describe, expect, it, vi } from 'vitest'

import { ProfilePage } from './profile-page'

const logoutMock = vi.fn()
const refreshMeMock = vi.fn()
const historyMock = vi.fn()
const deleteAccountMock = vi.fn()
const navigateMock = vi.fn()

const meUser = {
  userId: 'user_1',
  displayName: '田中 ユウ',
  avatarUrl: '/avatars/yu.webp',
  email: 'yu@example.com',
  bio: '',
}

vi.mock('@/hooks/use-me', () => ({
  useMe: () => ({
    user: meUser,
    stats: { gamesPlayed: 0, wins: 0, losses: 0 },
    updateAvatarPreset: vi.fn(),
    uploadAvatarFile: vi.fn(),
    updateProfile: vi.fn(),
    deleteAccount: deleteAccountMock,
    refreshMe: refreshMeMock,
  }),
  getGlobalMeUser: () => meUser,
}))

vi.mock('@/hooks/use-match-history', () => ({
  useMatchHistory: () => historyMock(),
}))

vi.mock('@/hooks/use-auth', () => ({
  useAuth: () => ({ logout: logoutMock }),
}))

vi.mock('wouter', async (importOriginal) => ({
  ...(await importOriginal<typeof import('wouter')>()),
  useLocation: () => ['/profile', navigateMock],
}))

beforeEach(() => {
  logoutMock.mockReset().mockResolvedValue(undefined)
  deleteAccountMock.mockReset().mockResolvedValue({ success: true })
  navigateMock.mockReset()
  refreshMeMock.mockReset()
  historyMock.mockReset().mockReturnValue({ items: [], isLoading: false, error: null })
})

describe('ProfilePage 戦績', () => {
  it('開くたびに自分の情報（戦績）を取り直す', () => {
    render(<ProfilePage />)
    expect(refreshMeMock).toHaveBeenCalledTimes(1)
  })

  it('「最近の対戦」に対戦履歴を出す（勝敗・役職・部屋名）', () => {
    historyMock.mockReturnValue({
      items: [
        { gameId: 'game_1', roomName: '深夜の読書会', role: 'CITIZEN', outcome: 'WIN', playedAt: '2026-09-26T10:17:12Z' },
        { gameId: 'game_2', roomName: '甘いもの捜査班', role: 'WORD_WOLF', outcome: 'LOSS', playedAt: '2026-09-25T09:00:00Z' },
      ],
      isLoading: false,
      error: null,
    })
    render(<ProfilePage />)
    fireEvent.click(screen.getByRole('button', { name: '戦績' }))

    const recent = within(
      screen.getByRole('heading', { name: '最近の対戦' }).closest('section') as HTMLElement,
    )
    expect(recent.getByText('深夜の読書会')).toBeInTheDocument()
    expect(recent.getByText('甘いもの捜査班')).toBeInTheDocument()
    expect(recent.getByText('勝利')).toBeInTheDocument()
    expect(recent.getByText('敗北')).toBeInTheDocument()
    expect(recent.getByText(/人狼/)).toBeInTheDocument()
  })

  it('対戦履歴が無ければその旨を出す', () => {
    render(<ProfilePage />)
    fireEvent.click(screen.getByRole('button', { name: '戦績' }))

    expect(screen.getByText('まだ対戦履歴がありません。')).toBeInTheDocument()
  })
})

describe('ProfilePage ログアウト', () => {
  it('「ログアウト」ボタンが表示される', () => {
    render(<ProfilePage />)
    expect(screen.getByRole('button', { name: 'ログアウト' })).toBeInTheDocument()
  })

  it('クリックで logout を呼び、/login へ遷移する', async () => {
    render(<ProfilePage />)

    fireEvent.click(screen.getByRole('button', { name: 'ログアウト' }))

    await waitFor(() => expect(logoutMock).toHaveBeenCalledTimes(1))
    await waitFor(() => expect(navigateMock).toHaveBeenCalledWith('/login'))
  })
})

describe('ProfilePage アカウント削除', () => {
  it('「アカウント削除」ボタンで確認ダイアログが開く', () => {
    render(<ProfilePage />)

    fireEvent.click(screen.getByRole('button', { name: 'アカウント削除' }))

    expect(screen.getByText('アカウントを削除しますか？')).toBeInTheDocument()
  })

  it('確認ダイアログで確定すると deleteAccount を呼び、/login へ遷移する', async () => {
    render(<ProfilePage />)

    fireEvent.click(screen.getByRole('button', { name: 'アカウント削除' }))
    fireEvent.click(screen.getByRole('button', { name: 'アカウントを削除' }))

    await waitFor(() => expect(deleteAccountMock).toHaveBeenCalledTimes(1))
    await waitFor(() => expect(navigateMock).toHaveBeenCalledWith('/login'))
  })

  it('失敗時はエラーメッセージを表示し、遷移しない', async () => {
    deleteAccountMock.mockResolvedValue({ success: false, error: '通信に失敗しました。' })
    render(<ProfilePage />)

    fireEvent.click(screen.getByRole('button', { name: 'アカウント削除' }))
    fireEvent.click(screen.getByRole('button', { name: 'アカウントを削除' }))

    await waitFor(() => expect(screen.getByRole('alert')).toHaveTextContent('通信に失敗しました。'))
    expect(navigateMock).not.toHaveBeenCalledWith('/login')
  })
})
