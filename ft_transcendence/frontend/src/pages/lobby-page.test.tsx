import { act, fireEvent, render, screen, waitFor } from '@testing-library/react'
import type { ReactNode } from 'react'
import { beforeEach, expect, it, vi } from 'vitest'
import type { components } from '@/types/api'
import { LobbyPage } from './lobby-page'

const mocks = vi.hoisted(() => ({
  search: 'roomId=room_1',
  channelTopics: [] as string[],
  get: vi.fn(),
  post: vi.fn(),
  put: vi.fn(),
  delete: vi.fn(),
  channelOn: vi.fn(),
  navigate: vi.fn(),
  copy: vi.fn(),
}))
vi.mock('@/lib/api-client', () => ({
  api: {
    GET: mocks.get,
    POST: mocks.post,
    PUT: mocks.put,
    DELETE: mocks.delete,
  },
}))
vi.mock('wouter', async (original) => ({
  ...(await original<typeof import('wouter')>()),
  useLocation: () => ['/lobby', mocks.navigate],
  useSearch: () => mocks.search,
}))
vi.mock('@/hooks/use-user-channel', () => ({
  useUserChannel: () => ({ on: () => () => {} }),
}))
vi.mock('@/components/layout/mobile-shell', () => ({
  MobileShell: ({ children }: { children: ReactNode }) => <>{children}</>,
}))
vi.mock('@/lib/channel-client', () => ({
  createSocket: () => ({
    connect: vi.fn(),
    disconnect: vi.fn(),
    channel: (topic: string) => {
      mocks.channelTopics.push(topic)
      return {
        on: mocks.channelOn,
        leave: vi.fn(),
        join: () => ({ receive: vi.fn() }),
      }
    },
  }),
}))

let room: components['schemas']['RoomState']
let currentUserId: string
beforeEach(() => {
  vi.clearAllMocks()
  mocks.search = 'roomId=room_1'
  mocks.channelTopics.length = 0
  window.history.replaceState({}, '', '/lobby?roomId=room_1')
  Object.defineProperty(navigator, 'clipboard', {
    configurable: true,
    value: { writeText: mocks.copy },
  })
  mocks.copy.mockResolvedValue(undefined)
  currentUserId = 'player'
  room = {
    roomId: 'room_1',
    roomName: 'テスト部屋',
    status: 'WAITING',
    inviteCode: 'CASE-042',
    inviteUrl: '/join/CASE-042',
    hostUserId: 'host',
    currentGameId: null,
    settings: {
      roomName: 'テスト部屋',
      genres: ['PLACE'],
      comebackEnabled: false,
      discussionMs: 300000,
      voteMs: 30000,
      reconnectGraceMs: 20000,
      minPlayers: 3,
      maxPlayers: 8,
    },
    players: ['host', 'player'].map((userId) => ({
      userId,
      displayName: userId,
      avatarUrl: '',
      isHost: userId === 'host',
      ready: true,
      connected: true,
    })),
    spectators: [],
  }
  mocks.get.mockImplementation(async (path: string) => ({
    data: {
      data:
        path === '/users/me'
          ? { user: { userId: currentUserId } }
          : path === '/friends'
            ? { items: [] }
            : room,
    },
  }))
  mocks.post.mockImplementation(async () => ({
    data: {
      data: {
        room: {
          ...room,
          hostUserId: currentUserId === 'host' ? 'player' : 'host',
          players: room.players.filter((p) => p.userId !== currentUserId),
          spectators: [
            {
              userId: currentUserId,
              displayName: currentUserId,
              avatarUrl: '',
            },
          ],
        },
      },
    },
  }))
})

it('ロビーで観戦に切り替えると参加操作も観戦者向けになる', async () => {
  render(<LobbyPage />)
  fireEvent.click(
    await screen.findByRole('button', { name: '観戦に切り替える' }),
  )
  expect(
    await screen.findByRole('button', { name: 'プレイヤーに切り替える' }),
  ).toBeEnabled()
  expect(mocks.post).toHaveBeenCalledWith('/rooms/{roomId}/join', {
    params: { path: { roomId: 'room_1' } },
    body: { participantType: 'SPECTATOR' },
  })
  expect(
    screen.queryByRole('button', { name: /タップで準備中/ }),
  ).not.toBeInTheDocument()
})

it('ホストは引き継ぎを確認してから観戦へ切り替える', async () => {
  currentUserId = 'host'
  render(<LobbyPage />)
  fireEvent.click(
    await screen.findByRole('button', { name: '観戦に切り替える' }),
  )
  expect(mocks.post).not.toHaveBeenCalled()
  fireEvent.click(
    screen.getByRole('button', { name: 'ホストを引き継いで観戦する' }),
  )
  expect(
    await screen.findByRole('button', { name: 'プレイヤーに切り替える' }),
  ).toBeEnabled()
})

it('参加用と観戦用の招待リンクを別々にコピーできる', async () => {
  render(<LobbyPage />)
  fireEvent.change(
    await screen.findByRole('combobox', { name: '招待する参加方法' }),
    { target: { value: 'SPECTATOR' } },
  )
  fireEvent.click(screen.getByRole('button', { name: '観戦者として招待' }))
  await waitFor(() =>
    expect(mocks.copy).toHaveBeenCalledWith(
      `${window.location.origin}/join/CASE-042?mode=spectate`,
    ),
  )
  fireEvent.change(screen.getByRole('combobox', { name: '招待する参加方法' }), {
    target: { value: 'PLAYER' },
  })
  fireEvent.click(screen.getByRole('button', { name: 'プレイヤーとして招待' }))
  await waitFor(() =>
    expect(mocks.copy).toHaveBeenLastCalledWith(
      `${window.location.origin}/join/CASE-042?mode=player`,
    ),
  )
})

it('切り替え失敗時はプレイヤーのまま理由を表示する', async () => {
  mocks.post.mockResolvedValue({
    error: { error: { code: 'GAME_IN_PROGRESS' } },
  })
  render(<LobbyPage />)
  fireEvent.click(
    await screen.findByRole('button', { name: '観戦に切り替える' }),
  )
  expect(await screen.findByRole('alert')).toHaveTextContent(
    'ゲームが始まったため',
  )
  expect(screen.getByRole('button', { name: '観戦に切り替える' })).toBeEnabled()
})

it('別の部屋へ移ると参加用・観戦用のリンクとチャンネルが切り替わる', async () => {
  const view = render(<LobbyPage />)
  fireEvent.change(
    await screen.findByRole('combobox', { name: '招待する参加方法' }),
    { target: { value: 'SPECTATOR' } },
  )
  expect(screen.getByRole('button', { name: '観戦者として招待' })).toBeEnabled()
  fireEvent.click(screen.getByRole('button', { name: '観戦者として招待' }))
  await waitFor(() =>
    expect(mocks.copy).toHaveBeenLastCalledWith(
      `${window.location.origin}/join/CASE-042?mode=spectate`,
    ),
  )

  room = {
    ...room,
    roomId: 'room_2',
    inviteCode: 'CASE-222',
    roomName: '新しい部屋',
  }
  mocks.search = 'roomId=room_2'
  view.rerender(<LobbyPage />)
  await waitFor(() =>
    expect(screen.getByLabelText('観戦者として招待するリンク')).toHaveValue(
      `${window.location.origin}/join/CASE-222?mode=spectate`,
    ),
  )
  fireEvent.click(screen.getByRole('button', { name: '観戦者として招待' }))
  await waitFor(() =>
    expect(mocks.copy).toHaveBeenLastCalledWith(
      `${window.location.origin}/join/CASE-222?mode=spectate`,
    ),
  )
  fireEvent.change(screen.getByRole('combobox', { name: '招待する参加方法' }), {
    target: { value: 'PLAYER' },
  })
  fireEvent.click(screen.getByRole('button', { name: 'プレイヤーとして招待' }))
  await waitFor(() =>
    expect(mocks.copy).toHaveBeenLastCalledWith(
      `${window.location.origin}/join/CASE-222?mode=player`,
    ),
  )
  expect(mocks.channelTopics).toEqual(['room:room_1', 'room:room_2'])
})

it('ホストが議論・投票時間を保存し、部屋の時間表示が更新される', async () => {
  currentUserId = 'host'
  mocks.put.mockResolvedValue({
    data: {
      data: {
        ...room,
        settings: { ...room.settings, discussionMs: 90000, voteMs: 45000 },
      },
    },
  })
  render(<LobbyPage />)
  fireEvent.change(await screen.findByRole('combobox', { name: '議論時間' }), {
    target: { value: '90000' },
  })
  fireEvent.change(screen.getByRole('combobox', { name: '投票時間' }), {
    target: { value: '45000' },
  })
  fireEvent.click(screen.getByRole('button', { name: '設定を保存' }))
  await waitFor(() =>
    expect(mocks.put).toHaveBeenCalledWith('/rooms/{roomId}/settings', {
      params: { path: { roomId: 'room_1' } },
      body: {
        roomName: room.settings.roomName,
        genres: ['PLACE'],
        comebackEnabled: false,
        discussionMs: 90000,
        voteMs: 45000,
      },
    }),
  )
  await waitFor(() =>
    expect(screen.getByRole('combobox', { name: '議論時間' })).toHaveValue(
      '90000',
    ),
  )
  expect(screen.getByRole('combobox', { name: '投票時間' })).toHaveValue(
    '45000',
  )
})

it('プレイヤーには時間の編集操作を表示しない', async () => {
  render(<LobbyPage />)
  await screen.findByText('ゲーム設定')
  expect(
    screen.queryByRole('button', { name: '設定を保存' }),
  ).not.toBeInTheDocument()
  expect(screen.getByText('5:00')).toBeInTheDocument()
  expect(screen.getByText('0:30')).toBeInTheDocument()
})

it('ホストが変更した時間をroom.stateから受け取ると参加者の表示も変わる', async () => {
  render(<LobbyPage />)
  await screen.findByText('ゲーム設定')
  const handler = mocks.channelOn.mock.calls.find(
    ([event]) => event === 'room.state',
  )?.[1]
  act(() =>
    handler({
      ...room,
      phase: 'LOBBY',
      settings: {
        discussionMs: 90000,
        voteMs: 45000,
        genres: ['FOOD'],
        comebackEnabled: true,
      },
    }),
  )
  expect(screen.getByText('食べ物')).toBeInTheDocument()
  expect(screen.getByText('あり（30秒・入力を公開）')).toBeInTheDocument()
  expect(screen.getByText('1:30')).toBeInTheDocument()
  expect(screen.getByText('0:45')).toBeInTheDocument()
})

it('ホストがジャンルと逆転モードを編集して保存できる', async () => {
  currentUserId = 'host'
  mocks.put.mockResolvedValue({
    data: {
      data: {
        ...room,
        settings: { ...room.settings, genres: ['FOOD'], comebackEnabled: true },
      },
    },
  })
  render(<LobbyPage />)
  await screen.findByText('お題ジャンル（複数選択可）')
  fireEvent.click(screen.getByRole('checkbox', { name: /食べ物ジャンル/ }))
  fireEvent.click(screen.getByRole('checkbox', { name: /場所ジャンル/ }))
  fireEvent.change(screen.getByRole('combobox', { name: '逆転モード' }), {
    target: { value: 'enabled' },
  })
  fireEvent.click(screen.getByRole('button', { name: '設定を保存' }))
  await waitFor(() =>
    expect(mocks.put).toHaveBeenCalledWith('/rooms/{roomId}/settings', {
      params: { path: { roomId: 'room_1' } },
      body: {
        roomName: room.settings.roomName,
        genres: ['FOOD'],
        comebackEnabled: true,
        discussionMs: 300000,
        voteMs: 30000,
      },
    }),
  )
  await waitFor(() =>
    expect(screen.getByRole('combobox', { name: '逆転モード' })).toHaveValue(
      'enabled',
    ),
  )
})

it('ジャンルを全解除したときは保存せず、保存失敗時は入力を残す', async () => {
  currentUserId = 'host'
  mocks.put.mockResolvedValue({ error: { error: { code: 'INVALID_PHASE' } } })
  render(<LobbyPage />)
  fireEvent.click(await screen.findByRole('checkbox', { name: /場所ジャンル/ }))
  fireEvent.click(screen.getByRole('button', { name: '設定を保存' }))
  expect(await screen.findByRole('alert')).toHaveTextContent('1つ以上')
  expect(mocks.put).not.toHaveBeenCalled()
  fireEvent.click(screen.getByRole('checkbox', { name: /食べ物ジャンル/ }))
  fireEvent.click(screen.getByRole('button', { name: '設定を保存' }))
  expect(await screen.findByRole('alert')).toHaveTextContent(
    'ゲームが始まったため',
  )
  expect(screen.getByRole('checkbox', { name: /食べ物ジャンル/ })).toBeChecked()
})

it('ホストだけが自分以外の参加者を確認後に退出させられる', async () => {
  currentUserId = 'host'
  mocks.delete.mockResolvedValue({
    data: {
      data: {
        ...room,
        players: room.players.filter((p) => p.userId !== 'player'),
      },
    },
  })
  render(<LobbyPage />)
  fireEvent.click(
    await screen.findByRole('button', { name: 'playerさんを退出させる' }),
  )
  expect(
    screen.queryByRole('button', { name: 'hostさんを退出させる' }),
  ).not.toBeInTheDocument()
  expect(mocks.delete).not.toHaveBeenCalled()
  fireEvent.click(screen.getByRole('button', { name: 'キャンセル' }))
  expect(mocks.delete).not.toHaveBeenCalled()
  fireEvent.click(
    screen.getByRole('button', { name: 'playerさんを退出させる' }),
  )
  fireEvent.click(screen.getByRole('button', { name: '退出させる' }))
  await waitFor(() =>
    expect(mocks.delete).toHaveBeenCalledWith(
      '/rooms/{roomId}/members/{userId}',
      {
        params: { path: { roomId: 'room_1', userId: 'player' } },
      },
    ),
  )
  await waitFor(() =>
    expect(
      screen.queryByRole('button', { name: 'playerさんを退出させる' }),
    ).not.toBeInTheDocument(),
  )
})

it('退出に失敗したときは理由を表示し参加者を残す', async () => {
  currentUserId = 'host'
  mocks.delete.mockResolvedValue({
    error: { error: { code: 'INVALID_PHASE' } },
  })
  render(<LobbyPage />)
  fireEvent.click(
    await screen.findByRole('button', { name: 'playerさんを退出させる' }),
  )
  fireEvent.click(screen.getByRole('button', { name: '退出させる' }))
  expect(await screen.findByRole('alert')).toHaveTextContent(
    'ゲームが始まったため',
  )
  fireEvent.click(screen.getByRole('button', { name: 'キャンセル' }))
  expect(
    screen.getByRole('button', { name: 'playerさんを退出させる' }),
  ).toBeInTheDocument()
})

it('退出対象者は理由とホームへの導線を受け取り操作画面を閉じる', async () => {
  render(<LobbyPage />)
  await screen.findByText('ゲーム設定')
  expect(
    screen.queryByRole('button', { name: /さんを退出させる/ }),
  ).not.toBeInTheDocument()
  const handler = mocks.channelOn.mock.calls.find(
    ([event]) => event === 'room.removed',
  )?.[1]
  act(() => handler({ roomId: 'room_1', reason: 'REMOVED_BY_HOST' }))
  expect(
    screen.getByText('ホストにより、この部屋から退出しました。'),
  ).toBeInTheDocument()
  expect(screen.getByRole('link', { name: 'ホームへ戻る' })).toHaveAttribute(
    'href',
    '/home',
  )
  expect(screen.queryByText('ゲーム設定')).not.toBeInTheDocument()
})

it('招待後も再招待でき、失敗しても再試行できる', async () => {
  const friend = {
    user: { userId: 'friend', displayName: '友達', avatarUrl: '' },
    presence: { state: 'ONLINE', lastSeenAt: null },
  }
  mocks.get.mockImplementation(async (path: string) => ({
    data: {
      data:
        path === '/users/me'
          ? { user: { userId: currentUserId } }
          : path === '/friends'
            ? { items: [friend] }
            : room,
    },
  }))
  mocks.post.mockResolvedValue({ data: { data: {} } })
  render(<LobbyPage />)
  fireEvent.click(await screen.findByRole('button', { name: '招待' }))
  fireEvent.click(await screen.findByRole('button', { name: '再招待' }))
  await waitFor(() => expect(mocks.post).toHaveBeenCalledTimes(2))
  expect(mocks.post).toHaveBeenLastCalledWith('/rooms/{roomId}/invites', {
    params: { path: { roomId: 'room_1' } },
    body: { targetUserId: 'friend' },
  })
  mocks.post.mockRejectedValueOnce(new Error('offline'))
  await waitFor(() =>
    expect(screen.getByRole('button', { name: '再招待' })).toBeEnabled(),
  )
  fireEvent.click(screen.getByRole('button', { name: '再招待' }))
  expect(
    await screen.findByText('通信エラーが発生しました。'),
  ).toBeInTheDocument()
  expect(screen.getByRole('button', { name: '再招待' })).toBeEnabled()
})

it('参加者の接続状態をオンライン・オフラインのアイコンで表示し、room.stateで更新する', async () => {
  render(<LobbyPage />)
  expect(
    await screen.findAllByRole('img', { name: 'オンライン' }),
  ).toHaveLength(2)
  expect(screen.queryByText('接続中')).not.toBeInTheDocument()
  const handler = mocks.channelOn.mock.calls.find(
    ([event]) => event === 'room.state',
  )?.[1]
  act(() =>
    handler({
      ...room,
      phase: 'LOBBY',
      players: room.players.map((player) => ({
        userId: player.userId,
        displayName: player.displayName,
        avatarUrl: player.avatarUrl,
        ready: player.ready,
        connected: player.userId !== 'player',
      })),
    }),
  )
  expect(screen.getByRole('img', { name: 'オフライン' })).toBeInTheDocument()
  expect(screen.getAllByRole('img', { name: 'オンライン' })).toHaveLength(1)
  expect(screen.queryByText('切断')).not.toBeInTheDocument()
})
