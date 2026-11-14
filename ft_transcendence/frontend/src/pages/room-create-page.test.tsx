import { fireEvent, render, screen, waitFor } from '@testing-library/react'
import type { ReactNode } from 'react'
import { beforeEach, expect, it, vi } from 'vitest'
import { RoomCreatePage } from './room-create-page'
const mocks = vi.hoisted(() => ({ post: vi.fn(), navigate: vi.fn() }))
vi.mock('@/lib/api-client', () => ({ api: { POST: mocks.post } }))
vi.mock('wouter', () => ({ useLocation: () => ['/rooms/new', mocks.navigate] }))
vi.mock('@/components/layout/mobile-shell', () => ({
  MobileShell: ({ children }: { children: ReactNode }) => <>{children}</>,
}))
beforeEach(() => {
  vi.clearAllMocks()
  mocks.post.mockResolvedValue({ data: { data: { roomId: 'room_extra' } } })
})
it.each([false, true])(
  '難易度を持たず逆転モード %s を部屋に保存する',
  async (enabled) => {
    render(<RoomCreatePage />)
    expect(screen.queryByLabelText('お題の近さ')).not.toBeInTheDocument()
    expect(
      screen.queryByRole('checkbox', { name: '恋愛ジャンル' }),
    ).not.toBeInTheDocument()
    expect(
      screen.queryByRole('checkbox', { name: '本音・価値観ジャンル' }),
    ).not.toBeInTheDocument()
    for (const checkbox of screen.getAllByRole('checkbox', {
      name: /ジャンル/,
    })) {
      expect(checkbox).toBeChecked()
    }
    const toggle = screen.getByRole('checkbox', { name: '逆転モード' })
    expect(toggle).not.toBeChecked()
    if (enabled) fireEvent.click(toggle)
    fireEvent.click(
      screen.getByRole('button', { name: '部屋を作成してロビーへ' }),
    )
    await waitFor(() =>
      expect(mocks.post).toHaveBeenCalledWith('/rooms', {
        body: {
          roomName: '深夜の読書会',
          genres: [
            'PLACE',
            'FOOD',
            'HOBBY',
            'DAILY_ITEM',
            'WORK_SCHOOL',
            'ENTERTAINMENT',
          ],
          comebackEnabled: enabled,
          discussionMs: 300000,
          voteMs: 30000,
        },
      }),
    )
    expect(mocks.navigate).toHaveBeenCalledWith('/lobby?roomId=room_extra')
  },
)

it('部屋ごとの議論・投票時間をミリ秒で送る', async () => {
  render(<RoomCreatePage />)
  fireEvent.change(screen.getByLabelText('議論時間（秒）'), {
    target: { value: '90' },
  })
  fireEvent.change(screen.getByLabelText('投票時間（秒）'), {
    target: { value: '45' },
  })
  fireEvent.click(
    screen.getByRole('button', { name: '部屋を作成してロビーへ' }),
  )
  await waitFor(() =>
    expect(mocks.post).toHaveBeenCalledWith('/rooms', {
      body: expect.objectContaining({ discussionMs: 90000, voteMs: 45000 }),
    }),
  )
})

it('時間が範囲外なら作成せずに入力案内を出す', () => {
  render(<RoomCreatePage />)
  fireEvent.change(screen.getByLabelText('議論時間（秒）'), {
    target: { value: '0' },
  })
  fireEvent.click(
    screen.getByRole('button', { name: '部屋を作成してロビーへ' }),
  )
  expect(mocks.post).not.toHaveBeenCalled()
  expect(
    screen.getByText(
      '議論時間は30〜900秒、投票時間は10〜180秒で入力してください。',
    ),
  ).toBeInTheDocument()
})
