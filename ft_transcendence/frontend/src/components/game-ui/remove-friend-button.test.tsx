import {
  act,
  fireEvent,
  render,
  renderHook,
  screen,
  waitFor,
} from '@testing-library/react'
import { beforeEach, describe, expect, it, vi } from 'vitest'
import { useFriends } from '@/hooks/use-friends'
import type { components } from '@/types/api'
import { RemoveFriendButton } from './remove-friend-button'

const mocks = vi.hoisted(() => ({ get: vi.fn(), delete: vi.fn() }))
vi.mock('@/lib/api-client', () => ({
  api: { GET: mocks.get, DELETE: mocks.delete },
  getAuthToken: () => 'remove-friend-test',
}))

const sora = { userId: 'user_sora', displayName: 'ソラ', avatarUrl: '' }
const minato = { userId: 'user_minato', displayName: 'ミナト', avatarUrl: '' }
const friends: components['schemas']['Friend'][] = [
  { user: sora, presence: { state: 'ONLINE', lastSeenAt: null } },
  { user: minato, presence: { state: 'OFFLINE', lastSeenAt: null } },
]

beforeEach(() => {
  vi.clearAllMocks()
  localStorage.clear()
  mocks.get.mockImplementation(async (path: string) => ({
    data: { status: 'ok', data: { items: path === '/friends' ? friends : [] } },
  }))
})

describe('RemoveFriendButton', () => {
  function openDialog(
    onRemove: () => Promise<{ success: boolean; error?: string }>,
  ) {
    render(<RemoveFriendButton friend={sora} onRemove={onRemove} />)
    fireEvent.click(
      screen.getByRole('button', { name: 'ソラさんとのフレンドを解除する' }),
    )
  }

  it('確認ダイアログで「解除する」を押したときだけ解除する', async () => {
    const onRemove = vi.fn().mockResolvedValue({ success: true })
    openDialog(onRemove)

    expect(
      await screen.findByText('ソラさんとのフレンドを解除しますか？'),
    ).toBeInTheDocument()
    expect(onRemove).not.toHaveBeenCalled()

    fireEvent.click(screen.getByRole('button', { name: '解除する' }))
    await waitFor(() => expect(onRemove).toHaveBeenCalledOnce())
    await waitFor(() =>
      expect(screen.queryByRole('alertdialog')).not.toBeInTheDocument(),
    )
  })

  it('キャンセルでは解除しない', async () => {
    const onRemove = vi.fn().mockResolvedValue({ success: true })
    openDialog(onRemove)

    fireEvent.click(await screen.findByRole('button', { name: 'キャンセル' }))
    expect(onRemove).not.toHaveBeenCalled()
  })

  it('失敗したらダイアログを開いたままエラーを出す', async () => {
    const onRemove = vi
      .fn()
      .mockResolvedValue({ success: false, error: '解除に失敗しました' })
    openDialog(onRemove)

    fireEvent.click(await screen.findByRole('button', { name: '解除する' }))
    expect(await screen.findByRole('alert')).toHaveTextContent(
      '解除に失敗しました',
    )
    expect(
      screen.getByText('ソラさんとのフレンドを解除しますか？'),
    ).toBeInTheDocument()
  })
})

describe('useFriends.removeFriend', () => {
  async function renderFriends() {
    const hook = renderHook(() => useFriends())
    await waitFor(() => expect(hook.result.current.friends).toHaveLength(2))
    return hook
  }

  it('DELETE /friends/{userId} を呼び、一覧から外す', async () => {
    mocks.delete.mockResolvedValue({ data: { status: 'ok' } })
    const { result } = await renderFriends()

    let outcome: { success: boolean } | undefined
    await act(async () => {
      outcome = await result.current.removeFriend('user_sora')
    })

    expect(mocks.delete).toHaveBeenCalledExactlyOnceWith('/friends/{userId}', {
      params: { path: { userId: 'user_sora' } },
    })
    expect(outcome).toEqual({ success: true })
    expect(result.current.friends.map((friend) => friend.user.userId)).toEqual([
      'user_minato',
    ])
  })

  it('NOT_FOUND（相手が先に解除済み）でも一覧から外す', async () => {
    mocks.delete.mockResolvedValue({
      error: {
        status: 'error',
        error: { code: 'NOT_FOUND', message: 'Not found' },
      },
    })
    const { result } = await renderFriends()

    await act(async () => {
      await result.current.removeFriend('user_sora')
    })

    expect(result.current.friends.map((friend) => friend.user.userId)).toEqual([
      'user_minato',
    ])
  })

  it('それ以外のエラーでは一覧を変えずにエラーを返す', async () => {
    mocks.delete.mockResolvedValue({
      error: {
        status: 'error',
        error: { code: 'INTERNAL', message: 'サーバーエラー' },
      },
    })
    const { result } = await renderFriends()

    let outcome: { success: boolean; error?: string } | undefined
    await act(async () => {
      outcome = await result.current.removeFriend('user_sora')
    })

    expect(outcome).toEqual({ success: false, error: 'サーバーエラー' })
    expect(result.current.friends).toHaveLength(2)
  })
})
