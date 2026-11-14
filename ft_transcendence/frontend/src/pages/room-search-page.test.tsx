import { fireEvent, render, screen, waitFor } from '@testing-library/react'
import type { ReactNode } from 'react'
import { beforeEach, expect, it, vi } from 'vitest'
import { Router } from 'wouter'
import { memoryLocation } from 'wouter/memory-location'
import type { components } from '@/types/api'
import { RoomSearchPage } from './room-search-page'

const mocks = vi.hoisted(() => ({ get: vi.fn(), post: vi.fn() }))
vi.mock('@/lib/api-client', () => ({
  api: { GET: mocks.get, POST: mocks.post },
}))
vi.mock('@/components/layout/mobile-shell', () => ({
  MobileShell: ({ children }: { children: ReactNode }) => <>{children}</>,
}))

let preview: components['schemas']['RoomPreview']
beforeEach(() => {
  vi.clearAllMocks()
  preview = {
    roomId: 'room_1',
    roomName: 'テスト部屋',
    inviteCode: 'CASE-042',
    status: 'WAITING',
    genres: ['PLACE'],
    playerCount: 4,
    maxPlayers: 8,
    spectatorCount: 0,
    joinable: true,
  }
  mocks.get.mockImplementation(async () => ({ data: { data: preview } }))
  mocks.post.mockImplementation(async () => ({
    data: { data: { room: preview } },
  }))
})

it.each(['WAITING', 'IN_GAME'] as const)(
  '観戦用リンクから %s の部屋へ観戦者として入る',
  async (status) => {
    preview.status = status
    const location = memoryLocation({
      path: '/join/CASE-042?mode=spectate',
      record: true,
    })
    render(
      <Router hook={location.hook}>
        <RoomSearchPage />
      </Router>,
    )
    fireEvent.click(await screen.findByRole('button', { name: '観戦する' }))
    expect(
      screen.queryByRole('button', { name: 'プレイヤー参加' }),
    ).not.toBeInTheDocument()
    await waitFor(() =>
      expect(location.history?.at(-1)).toBe(
        `${status === 'IN_GAME' ? '/game' : '/lobby'}?roomId=room_1`,
      ),
    )
    expect(mocks.post).toHaveBeenCalledWith('/rooms/{roomId}/join', {
      params: { path: { roomId: 'room_1' } },
      body: { participantType: 'SPECTATOR' },
    })
  },
)

it('プレイヤー用リンクではプレイヤー参加を案内する', async () => {
  const location = memoryLocation({ path: '/join/CASE-042?mode=player' })
  render(
    <Router hook={location.hook}>
      <RoomSearchPage />
    </Router>,
  )
  expect(
    await screen.findByRole('button', { name: 'プレイヤー参加' }),
  ).toBeEnabled()
  expect(
    screen.queryByRole('button', { name: '観戦する' }),
  ).not.toBeInTheDocument()
})

it('貼り付けた観戦用リンクでも参加方法を保持する', async () => {
  const location = memoryLocation({ path: '/rooms' })
  render(
    <Router hook={location.hook}>
      <RoomSearchPage />
    </Router>,
  )
  fireEvent.change(screen.getByLabelText('招待URL または 招待コード'), {
    target: { value: 'https://example.com/join/CASE-042?mode=spectate' },
  })
  fireEvent.click(screen.getByRole('button', { name: '部屋を検索' }))
  expect(await screen.findByRole('button', { name: '観戦する' })).toBeEnabled()
  expect(
    screen.queryByRole('button', { name: 'プレイヤー参加' }),
  ).not.toBeInTheDocument()
})

it('入室前にゲームが始まった場合も観戦へ進める', async () => {
  mocks.post.mockResolvedValueOnce({
    error: { error: { code: 'GAME_IN_PROGRESS' } },
  })
  const location = memoryLocation({
    path: '/join/CASE-042?mode=player',
    record: true,
  })
  render(
    <Router hook={location.hook}>
      <RoomSearchPage />
    </Router>,
  )
  fireEvent.click(await screen.findByRole('button', { name: 'プレイヤー参加' }))
  fireEvent.click(
    await screen.findByRole('button', { name: '観戦者として入る' }),
  )
  await waitFor(() =>
    expect(mocks.post).toHaveBeenLastCalledWith('/rooms/{roomId}/join', {
      params: { path: { roomId: 'room_1' } },
      body: { participantType: 'SPECTATOR' },
    }),
  )
})
