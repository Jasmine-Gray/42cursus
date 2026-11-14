import { act, fireEvent, render, screen, waitFor } from '@testing-library/react'
import type { ReactNode } from 'react'
import { beforeAll, beforeEach, expect, it, vi } from 'vitest'
import type {
  GameResult,
  PrivateState,
  RoomStateSnapshot,
} from '@/types/ws/events'
import { GamePage } from './game-page'

const mocks = vi.hoisted(() => ({
  get: vi.fn(),
  post: vi.fn(),
  send: vi.fn(),
  setSkip: vi.fn(),
  navigate: vi.fn(),
  on: vi.fn(),
  room: vi.fn(),
}))
vi.mock('@/lib/api-client', () => ({
  api: { GET: mocks.get, POST: mocks.post },
}))
vi.mock('wouter', () => ({ useLocation: () => ['/game', mocks.navigate] }))
vi.mock('@/hooks/use-room-channel', () => ({ useRoomChannel: mocks.room }))
vi.mock('@/hooks/use-wake-lock', () => ({ useWakeLock: vi.fn() }))
vi.mock('@/hooks/use-user-channel', () => ({
  useUserChannel: () => ({ on: mocks.on }),
}))
vi.mock('@/hooks/use-me', () => ({
  useMe: () => ({
    user: {
      userId: 'self',
      displayName: '自分',
      avatarUrl: '/avatars/yu.webp',
    },
  }),
}))
vi.mock('@/components/layout/mobile-shell', () => ({
  MobileShell: ({ children }: { children: ReactNode }) => (
    <div className="case-scroll">{children}</div>
  ),
}))
vi.mock('@/components/game-ui/word-card', () => ({
  WordCard: ({
    word,
    onVisibilityChange,
  }: {
    word: string
    onVisibilityChange?: (value: boolean) => void
  }) => <button onClick={() => onVisibilityChange?.(true)}>{word}</button>,
}))
vi.mock('@/pages/game-countdown-page', () => ({
  GameCountdownPage: ({ onComplete }: { onComplete: () => void }) => (
    <button onClick={onComplete}>入場演出を完了</button>
  ),
}))

// 結果画面の motion（whileInView）が使う。jsdom には無いので何もしない実装で足す
beforeAll(() => {
  vi.stubGlobal(
    'IntersectionObserver',
    class {
      observe() {}
      unobserve() {}
      disconnect() {}
      takeRecords() {
        return []
      }
    },
  )
})

let roomState: RoomStateSnapshot
let result: GameResult | null
let privateListener: (value: PrivateState) => void
beforeEach(() => {
  vi.clearAllMocks()
  sessionStorage.clear()
  result = null
  roomState = {
    comeback: null,
    roomId: 'room_1',
    gameId: 'game_1',
    phase: 'DISCUSSION',
    phaseVersion: 2,
    deadline: new Date(Date.now() + 300000).toISOString(),
    paused: false,
    reconnectDeadline: null,
    hostUserId: 'self',
    spectators: [],
    players: [
      {
        userId: 'self',
        displayName: '自分',
        avatarUrl: '/avatars/yu.webp',
        ready: true,
        connected: true,
      },
      {
        userId: 'other',
        displayName: 'アオイ',
        avatarUrl: '/avatars/aoi.webp',
        ready: true,
        connected: true,
      },
    ],
    skipProgress: { skippedCount: 0, playerCount: 2 },
    voteProgress: null,
  }
  mocks.get.mockResolvedValue({
    data: {
      status: 'ok',
      data: { user: { userId: 'self' }, activeRoom: { roomId: 'room_1' } },
    },
  })
  mocks.post.mockResolvedValue({ data: { status: 'ok' } })
  mocks.on.mockImplementation((_event, callback) => {
    privateListener = callback
    return () => {}
  })
  mocks.room.mockImplementation(() => ({
    roomState,
    status: 'ready',
    result,
    replaced: false,
    messages: [],
    sendError: null,
    send: mocks.send,
    setSkip: mocks.setSkip,
  }))
})

async function openGame() {
  const view = render(<GamePage />)
  await waitFor(() => expect(mocks.room).toHaveBeenCalledWith('room_1'))
  return view
}

it('旧お題画面から議論に進み、共有チャンネルでチャットとスキップを送る', async () => {
  await openGame()
  act(() =>
    privateListener({ gameId: 'game_1', word: '図書館', category: 'PLACE' }),
  )
  expect(screen.getByRole('button', { name: '議論へ' })).toBeDisabled()
  fireEvent.click(screen.getByRole('button', { name: '図書館' }))
  fireEvent.click(screen.getByRole('button', { name: '議論へ' }))
  expect(
    document.querySelector(
      '.case-scroll > main.discussion-board > .discussion-chat',
    ),
  ).not.toBeNull()
  fireEvent.change(screen.getByLabelText('メッセージ'), {
    target: { value: '静かな場所です' },
  })
  fireEvent.click(screen.getByRole('button', { name: /送信/ }))
  expect(mocks.send).toHaveBeenCalledWith('静かな場所です')
  fireEvent.click(screen.getByRole('button', { name: '投票へ' }))
  expect(mocks.setSkip).toHaveBeenCalledWith(true)
  expect(mocks.navigate).not.toHaveBeenCalled()
  expect(mocks.room.mock.calls.every((args) => args.length === 1)).toBe(true)
})

it('投票確認を確定して初めてAPIへ送り、結果への移行はサーバーを待つ', async () => {
  roomState.phase = 'VOTING'
  roomState.voteProgress = { votedCount: 0, playerCount: 2 }
  await openGame()
  fireEvent.click(screen.getByRole('button', { name: /アオイ/ }))
  fireEvent.click(screen.getByRole('button', { name: 'アオイに投票する' }))
  expect(mocks.post).not.toHaveBeenCalled()
  fireEvent.click(screen.getByRole('button', { name: '投票を確定' }))
  await waitFor(() =>
    expect(mocks.post).toHaveBeenCalledWith('/games/{gameId}/votes', {
      params: { path: { gameId: 'game_1' } },
      body: { targetUserId: 'other' },
    }),
  )
  await waitFor(() =>
    expect(screen.getByRole('button', { name: '投票しました' })).toBeDisabled(),
  )
  expect(mocks.navigate).not.toHaveBeenCalled()
})

it('観戦者には全員のお題を表示し、チャット入力と投票操作は表示しない', async () => {
  roomState.players = roomState.players.filter(
    (player) => player.userId !== 'self',
  )
  roomState.spectators = [
    { userId: 'self', displayName: '観戦者', avatarUrl: '' },
  ]
  mocks.get.mockImplementation(async (path: string) => ({
    data: {
      status: 'ok',
      data:
        path === '/games/{gameId}/spectator-assignments'
          ? {
              gameId: 'game_1',
              category: 'PLACE',
              players: [
                {
                  userId: 'other',
                  displayName: 'アオイ',
                  avatarUrl: '',
                  word: '図書館',
                  role: 'WORD_WOLF',
                },
              ],
            }
          : { user: { userId: 'self' }, activeRoom: { roomId: 'room_1' } },
    },
  }))
  const view = await openGame()
  expect(await screen.findByText('図書館')).toBeInTheDocument()
  expect(screen.getByText('ウルフ')).toBeInTheDocument()
  expect(
    screen.queryByRole('button', { name: '議論へ' }),
  ).not.toBeInTheDocument()
  expect(screen.queryByLabelText('メッセージ')).not.toBeInTheDocument()
  expect(
    screen.queryByRole('button', { name: '投票へ' }),
  ).not.toBeInTheDocument()
  roomState = { ...roomState, phase: 'VOTING' }
  view.rerender(<GamePage />)
  expect(screen.getByText('投票の完了を待っています')).toBeInTheDocument()
  expect(
    screen.queryByRole('button', { name: /投票する/ }),
  ).not.toBeInTheDocument()
})

it('入場演出が終わってもサーバーのフェーズを進めない', async () => {
  roomState.phase = 'DEALING'
  await openGame()
  fireEvent.click(screen.getByRole('button', { name: '入場演出を完了' }))
  expect(screen.getByRole('button', { name: '議論へ' })).toBeDisabled()
  expect(mocks.post).not.toHaveBeenCalled()
  expect(mocks.navigate).not.toHaveBeenCalled()
})

function finishedResult(): GameResult {
  return {
    comeback: null,
    gameId: 'game_1',
    winner: 'CITIZEN_SIDE',
    reason: 'WOLF_IDENTIFIED',
    wolfUserId: 'other',
    words: {
      majority: { word: '図書館', count: 1 },
      minority: { word: '書店', count: 1 },
    },
    players: [
      {
        userId: 'self',
        displayName: '自分',
        avatarUrl: '/avatars/yu.webp',
        role: 'CITIZEN',
        word: '図書館',
        votesReceived: 0,
      },
      {
        userId: 'other',
        displayName: 'アオイ',
        avatarUrl: '/avatars/aoi.webp',
        role: 'WORD_WOLF',
        word: '書店',
        votesReceived: 2,
      },
    ],
  }
}

it('RESULT の30秒が過ぎて LOBBY に戻っても結果画面に留まり、「ロビーへ」で初めて移る', async () => {
  roomState.phase = 'RESULT'
  result = finishedResult()
  const view = await openGame()
  fireEvent.click(screen.getByRole('button', { name: '演出をスキップ' }))
  fireEvent.click(screen.getByRole('button', { name: '結果を見る' }))
  expect(screen.getByRole('button', { name: 'ロビーへ' })).toBeInTheDocument()

  // サーバーが LOBBY へ戻す（reset_lobby: phase.changed → room.state で gameId が null になる）
  roomState = { ...roomState, phase: 'LOBBY', gameId: null, deadline: null }
  view.rerender(<GamePage />)

  expect(screen.getByRole('button', { name: 'ロビーへ' })).toBeInTheDocument()
  // 開票の演出を最初から流し直さない
  expect(
    screen.queryByRole('button', { name: '演出をスキップ' }),
  ).not.toBeInTheDocument()
  expect(mocks.navigate).not.toHaveBeenCalled()

  fireEvent.click(screen.getByRole('button', { name: 'ロビーへ' }))
  expect(mocks.navigate).toHaveBeenCalledWith('/lobby?roomId=room_1')
})

it('結果を持たずに LOBBY の部屋を開いたらロビーへ移る', async () => {
  roomState.phase = 'LOBBY'
  roomState.gameId = null
  await openGame()
  expect(mocks.navigate).toHaveBeenCalledWith('/lobby?roomId=room_1')
})

it('結果を見ている間に次のゲームが始まったら、次のゲームを表示する', async () => {
  roomState.phase = 'RESULT'
  result = finishedResult()
  const view = await openGame()
  fireEvent.click(screen.getByRole('button', { name: '演出をスキップ' }))
  fireEvent.click(screen.getByRole('button', { name: '結果を見る' }))
  roomState = { ...roomState, phase: 'LOBBY', gameId: null, deadline: null }
  view.rerender(<GamePage />)

  roomState = {
    ...roomState,
    phase: 'DEALING',
    gameId: 'game_2',
    deadline: new Date(Date.now() + 5000).toISOString(),
  }
  view.rerender(<GamePage />)

  expect(
    screen.queryByRole('button', { name: 'ロビーへ' }),
  ).not.toBeInTheDocument()
  expect(
    screen.getByRole('button', { name: '入場演出を完了' }),
  ).toBeInTheDocument()
})

it('議論へ進んだあとリロードしても、お題をめくる画面を飛ばして議論に戻る', async () => {
  const first = await openGame()
  act(() =>
    privateListener({ gameId: 'game_1', word: '図書館', category: 'PLACE' }),
  )
  fireEvent.click(screen.getByRole('button', { name: '図書館' }))
  fireEvent.click(screen.getByRole('button', { name: '議論へ' }))
  first.unmount()

  // リロード（同じタブで開き直す）
  await openGame()

  expect(
    screen.queryByRole('button', { name: '議論へ' }),
  ).not.toBeInTheDocument()
  expect(screen.getByLabelText('メッセージ')).toBeInTheDocument()
})

it('別のゲームで議論へ進んだ記録しか無ければ、お題をめくる画面から始める', async () => {
  sessionStorage.setItem('word_wolf_confirmed_game_id', 'game_0')
  await openGame()

  expect(screen.getByRole('button', { name: '議論へ' })).toBeInTheDocument()
})

it('結果の表示中にスリープ復帰などでタブが読み込み直されても、結果画面を復元し演出は流し直さない', async () => {
  roomState.phase = 'RESULT'
  result = finishedResult()
  const first = await openGame()
  fireEvent.click(screen.getByRole('button', { name: '演出をスキップ' }))
  fireEvent.click(screen.getByRole('button', { name: '結果を見る' }))
  first.unmount()

  // 読み込み直し: フックは結果を持っていない。サーバーはすでに LOBBY へ戻っている
  result = null
  roomState = { ...roomState, phase: 'LOBBY', gameId: null, deadline: null }
  await openGame()

  expect(
    await screen.findByRole('button', { name: 'ロビーへ' }),
  ).toBeInTheDocument()
  expect(
    screen.queryByRole('button', { name: '演出をスキップ' }),
  ).not.toBeInTheDocument()
  expect(mocks.navigate).not.toHaveBeenCalledWith('/lobby?roomId=room_1')

  fireEvent.click(screen.getByRole('button', { name: 'ロビーへ' }))
  expect(mocks.navigate).toHaveBeenCalledWith('/lobby?roomId=room_1')
  expect(sessionStorage.getItem('word_wolf_held_result')).toBeNull()
})

it('次のゲームが始まったら、保存していた前の結果は消す', async () => {
  sessionStorage.setItem(
    'word_wolf_held_result',
    JSON.stringify({
      roomId: 'room_1',
      result: { ...finishedResult(), gameId: 'game_0' },
    }),
  )
  roomState.phase = 'DEALING'
  roomState.gameId = 'game_1'
  await openGame()

  await waitFor(() =>
    expect(sessionStorage.getItem('word_wolf_held_result')).toBeNull(),
  )
})

it('プレイヤーには観戦者用のお題APIを呼ばない', async () => {
  await openGame()
  expect(
    screen.queryByRole('region', { name: '観戦者用のお題一覧' }),
  ).not.toBeInTheDocument()
  expect(
    mocks.get.mock.calls.some(
      ([path]) => path === '/games/{gameId}/spectator-assignments',
    ),
  ).toBe(false)
})

it('切断中の全員を表示し、一人戻っても残りの再接続を待つ', async () => {
  roomState.paused = true
  roomState.deadline = null
  roomState.reconnectDeadline = new Date(Date.now() + 20000).toISOString()
  roomState.players = roomState.players.map((player) => ({
    ...player,
    connected: false,
  }))
  const view = render(<GamePage />)
  expect(
    await screen.findByText('2人の再接続を待っています'),
  ).toBeInTheDocument()
  const waiting = screen.getByRole('list', { name: '再接続待ちのプレイヤー' })
  expect(waiting).toHaveTextContent('自分')
  expect(waiting).toHaveTextContent('アオイ')
  roomState = {
    ...roomState,
    players: roomState.players.map((player) => ({
      ...player,
      connected: player.userId === 'self',
    })),
  }
  view.rerender(<GamePage />)
  expect(screen.getByText('1人の再接続を待っています')).toBeInTheDocument()
  expect(
    screen.getByRole('list', { name: '再接続待ちのプレイヤー' }),
  ).not.toHaveTextContent('自分')
  expect(
    screen.getByRole('list', { name: '再接続待ちのプレイヤー' }),
  ).toHaveTextContent('アオイ')
})

function spectateResult() {
  roomState.phase = 'RESULT'
  roomState.spectators = [
    { userId: 'viewer', displayName: '観戦者', avatarUrl: '' },
  ]
  result = finishedResult()
  mocks.get.mockResolvedValue({
    data: { status: 'ok', data: { user: { userId: 'viewer' }, activeRoom: { roomId: 'room_1' } } },
  })
}

it('観戦者も開票から始まり、選んだ参加者の勝敗を演出後に表示する', async () => {
  spectateResult()
  const view = await openGame()
  expect(screen.getByRole('heading', { name: '開票' })).toBeInTheDocument()
  expect(screen.queryByLabelText('陣営とお題')).not.toBeInTheDocument()
  expect(screen.queryByText('書店')).not.toBeInTheDocument()
  expect(screen.getByRole('combobox', { name: '観戦する視点' })).toHaveValue(
    'self',
  )
  fireEvent.change(screen.getByRole('combobox', { name: '観戦する視点' }), {
    target: { value: 'other' },
  })
  expect(screen.getByRole('heading', { name: '開票' })).toBeInTheDocument()
  // ロビー復帰やホスト交代でも、見ている結果と選択した視点を保つ。
  roomState = {
    ...roomState,
    phase: 'LOBBY',
    gameId: null,
    hostUserId: 'other',
  }
  view.rerender(<GamePage />)
  fireEvent.click(screen.getByRole('button', { name: '演出をスキップ' }))
  expect(screen.getByRole('heading', { name: 'DEFEAT' })).toBeInTheDocument()
  expect(screen.queryByLabelText('陣営とお題')).not.toBeInTheDocument()
  fireEvent.click(screen.getByRole('button', { name: '結果を見る' }))
  expect(screen.getByLabelText('アオイさんの視点の勝敗')).toHaveTextContent(
    '敗北',
  )
  expect(screen.getByLabelText('陣営とお題')).toBeInTheDocument()
})

it('ホストが結果の参加者にいない場合も先頭の参加者視点で演出を表示する', async () => {
  spectateResult()
  roomState.hostUserId = 'viewer'
  await openGame()
  expect(screen.getByRole('combobox', { name: '観戦する視点' })).toHaveValue(
    'self',
  )
  expect(screen.queryByLabelText('陣営とお題')).not.toBeInTheDocument()
})

it('観戦者が演出途中でリロードしても結果一覧を先に表示しない', async () => {
  spectateResult()
  const first = await openGame()
  first.unmount()
  result = null
  roomState = { ...roomState, phase: 'LOBBY', gameId: null }
  await openGame()
  expect(screen.getByRole('heading', { name: '開票' })).toBeInTheDocument()
  expect(screen.queryByLabelText('陣営とお題')).not.toBeInTheDocument()
})
