import {
  act,
  fireEvent,
  render,
  renderHook,
  screen,
  waitFor,
} from '@testing-library/react'
import { beforeEach, expect, it, vi } from 'vitest'
import { MobileShell } from './mobile-shell'
import { useRoomInvites } from '@/hooks/use-room-invites'
import type { UserEventPayloads } from '@/types/ws/events'

const mocks = vi.hoisted(() => ({
  token: '',
  listeners: new Map<string, Set<(payload: unknown) => void>>(),
  on: vi.fn(),
}))
vi.mock('@/hooks/use-user-channel', () => ({
  useUserChannel: () => ({ on: mocks.on }),
}))
vi.mock('@/hooks/use-me', () => ({ useMe: () => ({ user: null }) }))
vi.mock('@/hooks/use-friends', () => ({
  useFriends: () => ({ friendRequests: [] }),
}))
vi.mock('@/components/game-ui/victorian-scene', () => ({
  VictorianScene: () => null,
}))
vi.mock('@/lib/api-client', () => ({
  getAuthToken: () => mocks.token,
  api: {
    GET: vi.fn(async () => ({ data: { status: 'ok', data: { items: [] } } })),
  },
}))

beforeEach(() => {
  mocks.token = Math.random().toString()
  mocks.listeners.clear()
  mocks.on.mockImplementation(
    (event: string, callback: (payload: unknown) => void) => {
      const callbacks = mocks.listeners.get(event) ?? new Set()
      callbacks.add(callback)
      mocks.listeners.set(event, callbacks)
      return () => callbacks.delete(callback)
    },
  )
})

function emit<E extends keyof UserEventPayloads>(
  event: E,
  payload: UserEventPayloads[E],
) {
  act(() =>
    mocks.listeners.get(event)?.forEach((callback) => callback(payload)),
  )
}

it('ロビー以外でも退出通知を受信し、画面を変えても閉じるまで部屋名を表示する', async () => {
  const view = render(
    <MobileShell ambientScene={false}>
      <p>ホーム画面</p>
    </MobileShell>,
  )
  emit('room.removed', {
    roomId: 'room_1',
    roomName: '深夜の読書会',
    reason: 'REMOVED_BY_HOST',
  })
  expect(await screen.findByRole('alert')).toHaveTextContent(
    '「深夜の読書会」からホストにより退出させられました。',
  )
  view.unmount()
  render(
    <MobileShell ambientScene={false}>
      <p>フレンド画面</p>
    </MobileShell>,
  )
  expect(screen.getByRole('alert')).toHaveTextContent('深夜の読書会')
  fireEvent.click(screen.getByRole('button', { name: '退出通知を閉じる' }))
  expect(screen.queryByRole('alert')).not.toBeInTheDocument()
})

it('同じ招待の再送でも再び通知し、招待一覧は重複させない', async () => {
  const { result } = renderHook(() => useRoomInvites())
  await waitFor(() => expect(result.current.isLoading).toBe(false))
  // 初回のREST同期を完了させてからチャンネル配信を受ける。
  await act(async () => {})
  const invite: UserEventPayloads['room.invited'] = {
    inviteId: 'invite_1',
    roomId: 'room_1',
    roomName: '深夜の読書会',
    inviteCode: 'CASE-001',
    from: { userId: 'host', displayName: 'ホスト', avatarUrl: '' },
    sentAt: '2026-09-26T12:00:00Z',
  }
  emit('room.invited', invite)
  act(() => result.current.clearLatestNotification())
  expect(result.current.latestNotification).toBeNull()
  emit('room.invited', invite)
  expect(result.current.latestNotification?.inviteId).toBe('invite_1')
  expect(result.current.invites).toHaveLength(1)
  act(() => result.current.clearLatestNotification())
})
