import { act, renderHook, waitFor } from '@testing-library/react'
import { beforeEach, describe, expect, it, vi } from 'vitest'

import { api } from '@/lib/api-client'

import { useRoomChannel } from './use-room-channel'

type Cb = (resp?: unknown) => void

// vi.mock はファイル先頭へ巻き上げられるため、共有するフェイクは vi.hoisted で先に用意する。
const fake = vi.hoisted(() => {
  const chatHandlers: Record<string, (payload: unknown) => void> = {}
  const joinReceivers: Record<string, (resp?: unknown) => void> = {}
  const pushCalls: Array<{
    event: string
    payload: Record<string, unknown>
    receivers: Record<string, (resp?: unknown) => void>
  }> = []

  const channel = {
    on: vi.fn((event: string, cb: (payload: unknown) => void) => {
      chatHandlers[event] = cb
    }),
    join: vi.fn(() => {
      const r = {
        receive: vi.fn(
          (status: string, cb: (resp?: unknown) => void) => (
            (joinReceivers[status] = cb),
            r
          ),
        ),
      }
      return r
    }),
    push: vi.fn((event: string, payload: Record<string, unknown>) => {
      const receivers: Record<string, (resp?: unknown) => void> = {}
      const r = {
        receive: vi.fn(
          (status: string, cb: (resp?: unknown) => void) => (
            (receivers[status] = cb),
            r
          ),
        ),
      }
      pushCalls.push({ event, payload, receivers })
      return r
    }),
    leave: vi.fn(),
  }
  const socket = {
    connect: vi.fn(),
    disconnect: vi.fn(),
    channel: vi.fn(() => channel),
  }

  return { chatHandlers, joinReceivers, pushCalls, channel, socket }
})

vi.mock('@/lib/channel-client', () => ({ createSocket: () => fake.socket }))
vi.mock('@/lib/api-client', () => ({ api: { GET: vi.fn() } }))

const getMock = vi.mocked(api.GET)

function resolveHistory(items: Array<Record<string, unknown>>) {
  getMock.mockResolvedValue({
    data: { status: 'ok', data: { items } },
    response: new Response(null, { status: 200 }),
  } as Awaited<ReturnType<typeof api.GET>>)
}

const published = (over: Record<string, unknown>) => ({
  messageId: 'msg_srv',
  userId: 'user_aoi',
  displayName: 'アオイ',
  content: 'やあ',
  sentAt: '2026-09-06T12:00:00Z',
  ...over,
})

beforeEach(() => {
  for (const k of Object.keys(fake.chatHandlers)) delete fake.chatHandlers[k]
  for (const k of Object.keys(fake.joinReceivers)) delete fake.joinReceivers[k]
  fake.pushCalls.length = 0
  vi.clearAllMocks()
  resolveHistory([])
})

describe('useRoomChannel', () => {
  it('マウントで socket 接続と room:{roomId} への join、履歴の取得を行う', async () => {
    resolveHistory([
      {
        messageId: 'msg_1',
        userId: 'user_aoi',
        displayName: 'アオイ',
        content: '一',
        sentAt: '2026-09-06T12:00:00Z',
      },
      {
        messageId: 'msg_2',
        userId: 'user_ren',
        displayName: 'レン',
        content: '二',
        sentAt: '2026-09-06T12:00:10Z',
      },
    ])

    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))

    expect(fake.socket.connect).toHaveBeenCalled()
    expect(fake.socket.channel).toHaveBeenCalledWith('room:room_1')

    act(() => fake.joinReceivers.ok?.())
    expect(result.current.status).toBe('ready')

    await waitFor(() =>
      expect(result.current.messages.map((m) => m.messageId)).toEqual([
        'msg_1',
        'msg_2',
      ]),
    )
  })

  it('chat.published を追記し、同じ messageId は無視する', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() =>
      expect(fake.chatHandlers['chat.published']).toBeTypeOf('function'),
    )

    act(() =>
      fake.chatHandlers['chat.published'](
        published({ messageId: 'msg_a', content: 'A' }),
      ),
    )
    act(() =>
      fake.chatHandlers['chat.published'](
        published({ messageId: 'msg_a', content: 'A(再送)' }),
      ),
    )

    expect(result.current.messages).toHaveLength(1)
    expect(result.current.messages[0].content).toBe('A')
  })

  it('send は楽観表示を追加し、対応する chat.published で確定に差し替える', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() =>
      expect(fake.chatHandlers['chat.published']).toBeTypeOf('function'),
    )

    act(() => result.current.send('  こんばんは  '))

    expect(fake.pushCalls).toHaveLength(1)
    expect(fake.pushCalls[0].event).toBe('chat.send')
    const cmid = fake.pushCalls[0].payload.clientMessageId as string
    expect(fake.pushCalls[0].payload.content).toBe('こんばんは')
    expect(result.current.messages[0]).toMatchObject({
      content: 'こんばんは',
      pending: true,
    })

    act(() =>
      fake.chatHandlers['chat.published'](
        published({
          messageId: 'msg_final',
          clientMessageId: cmid,
          userId: 'me',
          displayName: '自分',
          content: 'こんばんは',
        }),
      ),
    )

    expect(result.current.messages).toHaveLength(1)
    expect(result.current.messages[0]).toMatchObject({
      messageId: 'msg_final',
      pending: false,
      clientMessageId: cmid,
    })
  })

  it('send が RATE_LIMITED を受けたら sendError を立て、楽観表示を取り消す', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() =>
      expect(fake.chatHandlers['chat.published']).toBeTypeOf('function'),
    )

    act(() => result.current.send('速すぎる'))
    expect(result.current.messages).toHaveLength(1)

    // 失敗Reply の封筒は {status:"error", error:{code, message}}（asyncapi.yaml §19-21）
    act(() =>
      (fake.pushCalls[0].receivers.error as Cb)?.({
        status: 'error',
        error: { code: 'RATE_LIMITED', message: 'Too many messages.' },
      }),
    )

    expect(result.current.sendError).toBe('RATE_LIMITED')
    expect(result.current.messages).toHaveLength(0)
  })

  it('アンマウントで channel.leave と socket.disconnect を呼ぶ', async () => {
    const { unmount } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() =>
      expect(fake.chatHandlers['chat.published']).toBeTypeOf('function'),
    )

    unmount()

    expect(fake.channel.leave).toHaveBeenCalled()
    expect(fake.socket.disconnect).toHaveBeenCalled()
  })

  // --- ゲーム進行（b/game-socket で追加）-------------------------------------

  const snapshot = (over: Record<string, unknown> = {}) => ({
    roomId: 'room_1',
    gameId: 'game_1',
    phase: 'LOBBY',
    phaseVersion: 1,
    deadline: null,
    paused: false,
    reconnectDeadline: null,
    hostUserId: 'user_yu',
    players: [
      {
        userId: 'user_yu',
        displayName: 'ユウ',
        avatarUrl: '/a.webp',
        ready: true,
        connected: true,
      },
      {
        userId: 'user_aoi',
        displayName: 'アオイ',
        avatarUrl: '/b.webp',
        ready: false,
        connected: true,
      },
    ],
    spectators: [],
    skipProgress: null,
    voteProgress: null,
    ...over,
  })

  it('join reply の connectionId と publicState を取り込む', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() => expect(fake.joinReceivers.ok).toBeTypeOf('function'))

    act(() =>
      fake.joinReceivers.ok?.({
        status: 'ok',
        connectionId: 'conn_x',
        publicState: snapshot(),
      }),
    )

    expect(result.current.status).toBe('ready')
    expect(result.current.connectionId).toBe('conn_x')
    expect(result.current.roomState?.phase).toBe('LOBBY')
  })

  it('gameId を渡さなくても（ゲーム画面）join で分かった gameId の履歴を復元し、ゲームが変わっても join し直さない', async () => {
    resolveHistory([
      published({ messageId: 'msg_1', content: 'リロード前の発言' }),
    ])
    const { result } = renderHook(() => useRoomChannel('room_1'))
    await waitFor(() => expect(fake.joinReceivers.ok).toBeTypeOf('function'))
    expect(getMock).not.toHaveBeenCalled()

    act(() =>
      fake.joinReceivers.ok?.({
        status: 'ok',
        connectionId: 'conn_x',
        publicState: snapshot({ phase: 'DISCUSSION', gameId: 'game_1' }),
      }),
    )

    await waitFor(() =>
      expect(result.current.messages.map((m) => m.content)).toEqual([
        'リロード前の発言',
      ]),
    )
    expect(getMock).toHaveBeenCalledWith('/games/{gameId}/messages', {
      params: { path: { gameId: 'game_1' } },
    })

    // 次のゲームが始まる（room.state の gameId が変わる）→ 履歴は取り直すが、接続は張り直さない
    act(() =>
      fake.chatHandlers['room.state']?.(
        snapshot({ phase: 'DEALING', gameId: 'game_2' }),
      ),
    )
    await waitFor(() => expect(getMock).toHaveBeenCalledTimes(2))
    expect(fake.channel.join).toHaveBeenCalledTimes(1)
    expect(fake.socket.disconnect).not.toHaveBeenCalled()
  })

  it('phase.changed / vote.progress が roomState に反映される', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() => expect(fake.joinReceivers.ok).toBeTypeOf('function'))
    act(() =>
      fake.joinReceivers.ok?.({
        status: 'ok',
        connectionId: 'c',
        publicState: snapshot(),
      }),
    )

    act(() =>
      fake.chatHandlers['phase.changed']?.({
        gameId: 'game_1',
        phase: 'VOTING',
        phaseVersion: 4,
        deadline: '2026-09-20T12:00:30Z',
      }),
    )
    expect(result.current.roomState?.phase).toBe('VOTING')
    expect(result.current.roomState?.phaseVersion).toBe(4)

    act(() =>
      fake.chatHandlers['vote.progress']?.({ votedCount: 2, playerCount: 4 }),
    )
    expect(result.current.roomState?.voteProgress).toEqual({
      votedCount: 2,
      playerCount: 4,
    })
  })

  it('player.connection_changed が該当プレイヤーだけ更新する', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() => expect(fake.joinReceivers.ok).toBeTypeOf('function'))
    act(() =>
      fake.joinReceivers.ok?.({
        status: 'ok',
        connectionId: 'c',
        publicState: snapshot(),
      }),
    )

    act(() =>
      fake.chatHandlers['player.connection_changed']?.({
        userId: 'user_aoi',
        connected: false,
      }),
    )

    const players = result.current.roomState?.players ?? []
    expect(players.find((p) => p.userId === 'user_aoi')?.connected).toBe(false)
    expect(players.find((p) => p.userId === 'user_yu')?.connected).toBe(true)
  })

  it('game.paused / game.resumed が締切を入れ替える', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() => expect(fake.joinReceivers.ok).toBeTypeOf('function'))
    act(() =>
      fake.joinReceivers.ok?.({
        status: 'ok',
        connectionId: 'c',
        publicState: snapshot({
          phase: 'DISCUSSION',
          deadline: '2026-09-20T12:05:00Z',
        }),
      }),
    )

    act(() =>
      fake.chatHandlers['game.paused']?.({
        reason: 'PLAYER_DISCONNECTED',
        disconnectedUserIds: ['user_aoi', 'user_yu'],
        reconnectDeadline: '2026-09-20T12:00:20Z',
      }),
    )
    expect(result.current.roomState?.paused).toBe(true)
    expect(result.current.roomState?.deadline).toBeNull()
    expect(result.current.roomState?.players.every((p) => !p.connected)).toBe(
      true,
    )
    act(() =>
      fake.chatHandlers['game.paused']?.({
        reason: 'PLAYER_DISCONNECTED',
        disconnectedUserIds: ['user_aoi'],
        reconnectDeadline: '2026-09-20T12:00:20Z',
      }),
    )
    expect(result.current.roomState?.paused).toBe(true)
    expect(
      result.current.roomState?.players.find((p) => p.userId === 'user_yu')
        ?.connected,
    ).toBe(true)
    expect(
      result.current.roomState?.players.find((p) => p.userId === 'user_aoi')
        ?.connected,
    ).toBe(false)

    act(() =>
      fake.chatHandlers['game.resumed']?.({
        phase: 'DISCUSSION',
        phaseVersion: 3,
        deadline: '2026-09-20T12:06:00Z',
      }),
    )
    expect(result.current.roomState?.paused).toBe(false)
    expect(result.current.roomState?.reconnectDeadline).toBeNull()
  })

  it('result.published を保持し、connection.replaced で replaced が立つ', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() =>
      expect(fake.chatHandlers['result.published']).toBeTypeOf('function'),
    )

    act(() =>
      fake.chatHandlers['result.published']?.({
        gameId: 'game_1',
        winner: 'CITIZEN_SIDE',
      }),
    )
    expect(result.current.result?.winner).toBe('CITIZEN_SIDE')

    expect(result.current.replaced).toBe(false)
    act(() =>
      fake.chatHandlers['connection.replaced']?.({
        reason: 'NEWER_CONNECTION',
      }),
    )
    expect(result.current.replaced).toBe(true)
  })

  it('skip.progress が roomState に反映され、setSkip が discussion.skip を push する', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() => expect(fake.joinReceivers.ok).toBeTypeOf('function'))
    act(() =>
      fake.joinReceivers.ok?.({
        status: 'ok',
        connectionId: 'c',
        publicState: snapshot({ phase: 'DISCUSSION' }),
      }),
    )

    act(() =>
      fake.chatHandlers['skip.progress']?.({ skippedCount: 2, playerCount: 4 }),
    )
    expect(result.current.roomState?.skipProgress).toEqual({
      skippedCount: 2,
      playerCount: 4,
    })

    act(() => result.current.setSkip(true))
    expect(fake.pushCalls[0].event).toBe('discussion.skip')
    expect(fake.pushCalls[0].payload).toEqual({ skip: true })
  })

  it('逆転の公開入力を復元し、別ゲームの古い更新を無視する', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() => expect(fake.joinReceivers.ok).toBeTypeOf('function'))
    const comeback = {
      gameId: 'game_1',
      wolfUserId: 'wolf',
      content: '書きかけ',
    }
    act(() =>
      fake.joinReceivers.ok?.({
        connectionId: 'c',
        publicState: snapshot({ phase: 'COMEBACK', comeback }),
      }),
    )
    expect(result.current.roomState?.comeback).toEqual(comeback)
    act(() =>
      fake.chatHandlers['comeback.updated']?.({ ...comeback, content: '最新' }),
    )
    expect(result.current.roomState?.comeback?.content).toBe('最新')
    act(() =>
      fake.chatHandlers['comeback.updated']?.({
        ...comeback,
        gameId: 'old',
        content: '古い',
      }),
    )
    expect(result.current.roomState?.comeback?.content).toBe('最新')
    act(() => result.current.updateComebackDraft('別の文字'))
    expect(fake.pushCalls.at(-1)?.event).toBe('comeback.draft')
    let submitted: Promise<boolean>
    act(() => {
      submitted = result.current.submitComeback('最終回答')
    })
    expect(fake.pushCalls.at(-1)?.payload).toEqual({ content: '最終回答' })
    act(() => fake.pushCalls.at(-1)?.receivers.ok?.({ status: 'ok' }))
    await expect(submitted!).resolves.toBe(true)
  })

  it('setReady は room.ready を push し、失敗は封筒から code を読む', async () => {
    const { result } = renderHook(() => useRoomChannel('room_1', 'game_1'))
    await waitFor(() =>
      expect(fake.chatHandlers['chat.published']).toBeTypeOf('function'),
    )

    act(() => result.current.setReady(true))
    expect(fake.pushCalls[0].event).toBe('room.ready')
    expect(fake.pushCalls[0].payload).toEqual({ ready: true })

    act(() =>
      (fake.pushCalls[0].receivers.error as Cb)?.({
        status: 'error',
        error: {
          code: 'NOT_A_PLAYER',
          message: 'Only players can perform this action.',
        },
      }),
    )
    expect(result.current.sendError).toBe('NOT_A_PLAYER')
  })
})
