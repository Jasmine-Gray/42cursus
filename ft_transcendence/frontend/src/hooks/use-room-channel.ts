import { useCallback, useEffect, useRef, useState } from 'react'

import type { Channel } from 'phoenix'

import { api } from '@/lib/api-client'
import { createSocket } from '@/lib/channel-client'
import type { components } from '@/types/api'
import type {
  ComebackState,
  GameAborted,
  GamePaused,
  GameResult,
  GameResumed,
  PhaseChanged,
  PlayerConnectionChanged,
  RoomStateSnapshot,
  SkipProgress,
  VoteProgress,
} from '@/types/ws/events'

type ChatMessage = components['schemas']['ChatMessage']

/** 確定済みメッセージ、または自分が送って未確定（pending）の楽観表示。 */
export type RoomChatMessage = ChatMessage & {
  clientMessageId?: string
  pending?: boolean
}

/** join の状態。error は接続失敗（再接続待ち）。 */
export type RoomChannelStatus = 'connecting' | 'ready' | 'error'

/** 送信エラー。RoomChannel が返す code か、通信失敗の 'NETWORK'。 */
export type SendErrorCode =
  | 'RATE_LIMITED'
  | 'INVALID_PHASE'
  | 'GAME_PAUSED'
  | 'INVALID_PAYLOAD'
  | 'NOT_A_PLAYER'
  | 'STALE_CONNECTION'
  | 'FORBIDDEN'
  | 'NETWORK'

/** 失敗Reply の封筒（asyncapi.yaml §19-21）。 */
type ErrorReply = {
  status?: string
  error?: { code?: string; message?: string }
}

const errorCode = (resp: ErrorReply | undefined): SendErrorCode =>
  (resp?.error?.code as SendErrorCode) ?? 'NETWORK'

type UseRoomChannel = {
  updateComebackDraft: (content: string) => void
  submitComeback: (content: string) => Promise<boolean>
  messages: RoomChatMessage[]
  status: RoomChannelStatus
  sendError: SendErrorCode | null
  /** join 時点の公開状態。以降 room.state / phase.changed などで更新される。 */
  roomState: RoomStateSnapshot | null
  /** この Join に割り当てられた接続ID。user チャンネルの Join に渡す。 */
  connectionId: string | null
  /** 確定した結果（result.published）。未確定は null。 */
  result: GameResult | null
  /** 新しいタブに接続を奪われた。再接続せず画面を切り替える合図。 */
  replaced: boolean
  /** trim 済みの本文を送る。空なら何もしない。 */
  send: (content: string) => void
  /** 準備状態を切り替える（ロビー）。 */
  setReady: (ready: boolean) => void
  /** 議論スキップの賛成を切り替える。全員一致でサーバーが VOTING へ進める。 */
  setSkip: (skip: boolean) => void
}

export function useRoomChannel(
  roomId: string,
  gameId?: string,
): UseRoomChannel {
  const [messages, setMessages] = useState<RoomChatMessage[]>([])
  const [status, setStatus] = useState<RoomChannelStatus>('connecting')
  const [sendError, setSendError] = useState<SendErrorCode | null>(null)
  const [roomState, setRoomState] = useState<RoomStateSnapshot | null>(null)
  const [connectionId, setConnectionId] = useState<string | null>(null)
  const [result, setResult] = useState<GameResult | null>(null)
  const [replaced, setReplaced] = useState(false)
  const channelRef = useRef<Channel | null>(null)

  // chat.published の受信・履歴の復元をひとつに寄せる。
  // clientMessageId 一致 → 楽観表示を確定で差し替え。messageId 重複 → 無視。
  const upsert = useCallback((incoming: RoomChatMessage) => {
    setMessages((prev) => {
      if (incoming.clientMessageId) {
        const i = prev.findIndex(
          (m) => m.clientMessageId === incoming.clientMessageId,
        )
        if (i !== -1) {
          const next = prev.slice()
          next[i] = { ...incoming, pending: false }
          return next
        }
      }
      if (prev.some((m) => m.messageId === incoming.messageId)) return prev
      return [...prev, { ...incoming, pending: false }]
    })
  }, [])

  useEffect(() => {
    let cancelled = false
    const socket = createSocket()
    socket.connect()

    const channel = socket.channel(`room:${roomId}`)
    channelRef.current = channel

    channel.on('chat.published', (payload: RoomChatMessage) => {
      if (!cancelled) upsert(payload)
    })

    // 公開状態のスナップショット。サーバーが正本なので丸ごと差し替える。
    channel.on('room.state', (payload: RoomStateSnapshot) => {
      if (!cancelled) setRoomState(payload)
    })

    // 差分イベントはスナップショットの該当箇所だけ更新する。
    // room.state も直後に届くが、先に反映して画面のちらつきを抑える。
    channel.on('phase.changed', (payload: PhaseChanged) => {
      if (cancelled) return
      setRoomState((prev) =>
        prev
          ? {
              ...prev,
              gameId: payload.gameId ?? prev.gameId,
              phase: payload.phase,
              phaseVersion: payload.phaseVersion,
              deadline: payload.deadline,
            }
          : prev,
      )
    })

    channel.on('comeback.updated', (payload: ComebackState) => {
      if (!cancelled)
        setRoomState((previous) =>
          previous?.gameId === payload.gameId && previous.phase === 'COMEBACK'
            ? { ...previous, comeback: payload }
            : previous,
        )
    })

    channel.on('skip.progress', (payload: SkipProgress) => {
      if (!cancelled)
        setRoomState((prev) =>
          prev ? { ...prev, skipProgress: payload } : prev,
        )
    })

    channel.on('vote.progress', (payload: VoteProgress) => {
      if (!cancelled)
        setRoomState((prev) =>
          prev ? { ...prev, voteProgress: payload } : prev,
        )
    })

    channel.on(
      'player.connection_changed',
      (payload: PlayerConnectionChanged) => {
        if (cancelled) return
        setRoomState((prev) =>
          prev
            ? {
                ...prev,
                players: prev.players.map((p) =>
                  p.userId === payload.userId
                    ? { ...p, connected: payload.connected }
                    : p,
                ),
              }
            : prev,
        )
      },
    )

    channel.on('game.paused', (payload: GamePaused) => {
      if (cancelled) return
      setRoomState((prev) =>
        prev
          ? {
              ...prev,
              paused: true,
              players: prev.players.map((player) => ({
                ...player,
                connected: !payload.disconnectedUserIds.includes(player.userId),
              })),
              deadline: null,
              reconnectDeadline: payload.reconnectDeadline,
            }
          : prev,
      )
    })

    channel.on('game.resumed', (payload: GameResumed) => {
      if (cancelled) return
      setRoomState((prev) =>
        prev
          ? {
              ...prev,
              paused: false,
              reconnectDeadline: null,
              phase: payload.phase,
              phaseVersion: payload.phaseVersion,
              deadline: payload.deadline,
            }
          : prev,
      )
    })

    channel.on('result.published', (payload: GameResult) => {
      if (!cancelled) setResult(payload)
    })

    // 再接続待ちが尽きてゲームが流れた。LOBBY へ戻る（room.state が続けて届く）。
    channel.on('game.aborted', (_payload: GameAborted) => {
      if (!cancelled) setResult(null)
    })

    // 同じユーザーの新しいタブに接続を奪われた。このタブは再接続しない。
    channel.on('connection.replaced', () => {
      if (!cancelled) setReplaced(true)
    })

    channel
      .join()
      .receive(
        'ok',
        (reply: { connectionId?: string; publicState?: RoomStateSnapshot }) => {
          if (cancelled) return
          setStatus('ready')
          setConnectionId(reply?.connectionId ?? null)
          if (reply?.publicState) setRoomState(reply.publicState)
        },
      )
      .receive('error', () => !cancelled && setStatus('error'))
      .receive('timeout', () => !cancelled && setStatus('error'))

    return () => {
      cancelled = true
      channel.leave()
      socket.disconnect()
      channelRef.current = null
    }
  }, [roomId, upsert])

  // 履歴は HTTP（契約: GET /games/{gameId}/messages）。gameId を渡さない呼び出し（ゲーム画面）は
  // join で分かった公開状態の gameId を使う。リロード直後でもそのゲームの発言を復元できる。
  // join とは別の effect にして、ゲームが変わっても接続は張り直さない。
  const historyGameId = gameId ?? roomState?.gameId ?? null
  useEffect(() => {
    if (!historyGameId) return
    let cancelled = false

    void (async () => {
      const { data, error } = await api.GET('/games/{gameId}/messages', {
        params: { path: { gameId: historyGameId } },
      })
      if (cancelled || error || !data || data.status !== 'ok') return

      setMessages((prev) => {
        const seen = new Set(prev.map((m) => m.messageId))
        const restored = data.data.items.filter((m) => !seen.has(m.messageId))
        return [...restored, ...prev]
      })
    })()

    return () => {
      cancelled = true
    }
  }, [historyGameId])

  const send = useCallback((content: string) => {
    const trimmed = content.trim()
    const channel = channelRef.current
    if (!trimmed || !channel) return

    const clientMessageId =
      typeof crypto !== 'undefined' && 'randomUUID' in crypto
        ? crypto.randomUUID()
        : `${Date.now()}-${Math.random().toString(16).slice(2)}`

    setSendError(null)
    setMessages((prev) => [
      ...prev,
      {
        messageId: `local:${clientMessageId}`,
        userId: '',
        displayName: '',
        content: trimmed,
        sentAt: new Date().toISOString(),
        clientMessageId,
        pending: true,
      },
    ])

    const dropOptimistic = () =>
      setMessages((prev) =>
        prev.filter((m) => m.clientMessageId !== clientMessageId),
      )

    channel
      .push('chat.send', { clientMessageId, content: trimmed })
      .receive('error', (resp: ErrorReply | undefined) => {
        setSendError(errorCode(resp))
        dropOptimistic()
      })
      .receive('timeout', () => {
        setSendError('NETWORK')
        dropOptimistic()
      })
  }, [])

  const setReady = useCallback((ready: boolean) => {
    const channel = channelRef.current
    if (!channel) return

    setSendError(null)

    channel
      .push('room.ready', { ready })
      .receive('error', (resp: ErrorReply | undefined) =>
        setSendError(errorCode(resp)),
      )
      .receive('timeout', () => setSendError('NETWORK'))
  }, [])

  const setSkip = useCallback((skip: boolean) => {
    const channel = channelRef.current
    if (!channel) return

    setSendError(null)

    channel
      .push('discussion.skip', { skip })
      .receive('error', (resp: ErrorReply | undefined) =>
        setSendError(errorCode(resp)),
      )
      .receive('timeout', () => setSendError('NETWORK'))
  }, [])

  const updateComebackDraft = useCallback((content: string) => {
    channelRef.current
      ?.push('comeback.draft', { content })
      .receive('error', (reply: ErrorReply) => setSendError(errorCode(reply)))
      .receive('timeout', () => setSendError('NETWORK'))
  }, [])

  const submitComeback = useCallback((content: string): Promise<boolean> => {
    setSendError(null)
    return new Promise((resolve) => {
      const channel = channelRef.current
      if (!channel) {
        setSendError('NETWORK')
        resolve(false)
        return
      }
      channel
        .push('comeback.submit', { content })
        .receive('ok', () => resolve(true))
        .receive('error', (reply: ErrorReply) => {
          setSendError(errorCode(reply))
          resolve(false)
        })
        .receive('timeout', () => {
          setSendError('NETWORK')
          resolve(false)
        })
    })
  }, [])

  return {
    updateComebackDraft,
    submitComeback,
    messages,
    status,
    sendError,
    roomState,
    connectionId,
    result,
    replaced,
    send,
    setReady,
    setSkip,
  }
}
