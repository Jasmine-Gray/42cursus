import {
  Check,
  ChevronDown,
  ChevronUp,
  Copy,
  Crown,
  Eye,
  Gamepad2,
  Loader2,
  LogOut,
  Send,
  Sparkles,
  Users,
} from 'lucide-react'
import type { Channel } from 'phoenix'
import { useCallback, useEffect, useRef, useState } from 'react'
import { Link, useLocation, useSearch } from 'wouter'

import {
  AlertDialog,
  AlertDialogAction,
  AlertDialogCancel,
  AlertDialogContent,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogHeader,
  AlertDialogMedia,
  AlertDialogTitle,
} from '@/components/ui/alert-dialog'
import { RemoveRoomMemberButton } from '@/components/game-ui/remove-room-member-button'
import { RoomSettingsEditor } from '@/components/game-ui/room-settings-editor'
import { formatRoomDuration } from '@/components/game-ui/room-timing-fields'
import { ParticipantProfileButton } from '@/components/game-ui/participant-profile-button'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { SectionHeader } from '@/components/game-ui/section-header'
import { StatsStrip } from '@/components/game-ui/stats-strip'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Badge } from '@/components/ui/badge'
import { Button, buttonVariants } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { Separator } from '@/components/ui/separator'
import { useUserChannel } from '@/hooks/use-user-channel'
import { api } from '@/lib/api-client'
import { createSocket } from '@/lib/channel-client'
import { roomInviteUrl } from '@/lib/room-invite'
import { cn } from '@/lib/utils'
import type { components } from '@/types/api'
import type { RoomStateSnapshot } from '@/types/ws/events'

type RoomState = components['schemas']['RoomState']
type Friend = components['schemas']['Friend']
type Genre = components['schemas']['Genre']

const genreLabels: Record<Genre, string> = {
  ROMANCE: '恋愛',
  VALUES: '本音・価値観',
  PLACE: '場所',
  FOOD: '食べ物',
  HOBBY: '趣味',
  DAILY_ITEM: '日用品',
  WORK_SCHOOL: '学校・仕事',
  ENTERTAINMENT: 'エンタメ',
}

export function LobbyPage() {
  const [, navigate] = useLocation()
  const [roomId, setRoomId] = useState<string | null>(null)
  const [room, setRoom] = useState<RoomState | null>(null)
  const [currentUserId, setCurrentUserId] = useState<string | null>(null)
  const [friends, setFriends] = useState<Friend[]>([])
  const [invitedUserIds, setInvitedUserIds] = useState<Set<string>>(new Set())
  const [isInviting, setIsInviting] = useState<Record<string, boolean>>({})
  const [inviteParticipantType, setInviteParticipantType] =
    useState<components['schemas']['ParticipantType']>('PLAYER')
  const [copied, setCopied] = useState<
    components['schemas']['ParticipantType'] | null
  >(null)
  const [isLoading, setIsLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)
  const [actionFeedback, setActionFeedback] = useState<string | null>(null)
  const [isTogglingReady, setIsTogglingReady] = useState(false)
  const [showAllFriends, setShowAllFriends] = useState(false)
  const [leaveDialogOpen, setLeaveDialogOpen] = useState(false)
  const [isLeaving, setIsLeaving] = useState(false)
  const [leaveError, setLeaveError] = useState('')
  const [isChangingParticipation, setIsChangingParticipation] = useState(false)
  const [participationError, setParticipationError] = useState('')
  const [hostSpectateDialogOpen, setHostSpectateDialogOpen] = useState(false)
  const [removed, setRemoved] = useState(false)
  const channelRef = useRef<Channel | null>(null)

  // 1. URL または MeData から roomId を特定。
  // ロビーを開いたまま別の部屋へ移る（通知から招待を承認 → /lobby?roomId=新しい部屋）と LobbyPage は
  // 作り直されないので、ルーターの search を購読して ?roomId= の変化に追従する
  const paramRoomId = new URLSearchParams(useSearch()).get('roomId')
  useEffect(() => {
    if (paramRoomId) {
      setRoomId(paramRoomId)
    } else {
      api
        .GET('/users/me')
        .then((res) => {
          if (res.data?.data.user) {
            setCurrentUserId(res.data.data.user.userId)
          }
          if (res.data?.data.activeRoom) {
            setRoomId(res.data.data.activeRoom.roomId)
          } else {
            setError('参加中の部屋がありません。')
            setIsLoading(false)
          }
        })
        .catch(() => {
          setError('部屋情報の取得に失敗しました。')
          setIsLoading(false)
        })
    }
  }, [paramRoomId])

  // 2. 部屋状態とフレンド一覧の取得
  const fetchRoomAndFriends = useCallback(
    async (targetRoomId: string) => {
      try {
        const [roomRes, friendsRes, meRes] = await Promise.all([
          api.GET('/rooms/{roomId}', {
            params: { path: { roomId: targetRoomId } },
          }),
          api.GET('/friends'),
          api.GET('/users/me'),
        ])

        if (meRes.data?.data.user) {
          setCurrentUserId(meRes.data.data.user.userId)
        }

        if (roomRes.error || !roomRes.data) {
          if (roomRes.response?.status === 401) {
            navigate('/login')
            return
          }
          if (roomRes.response?.status === 403) {
            setError('この部屋への参加権限がありません。')
            return
          }
          setError('部屋情報の読み込みに失敗しました。')
          return
        }

        setRoom(roomRes.data.data)

        if (friendsRes.data?.data.items) {
          setFriends(friendsRes.data.data.items)
        }
      } catch {
        setError('通信エラーが発生しました。')
      } finally {
        setIsLoading(false)
      }
    },
    [navigate],
  )

  useEffect(() => {
    if (roomId) {
      // 前の部屋（招待URL・参加者）を出したままにしない
      setRemoved(false)
      setRoom(null)
      setError(null)
      setCopied(null)
      setInvitedUserIds(new Set())
      setActionFeedback(null)
      setParticipationError('')
      setLeaveDialogOpen(false)
      setHostSpectateDialogOpen(false)
      setIsLoading(true)
      void fetchRoomAndFriends(roomId)
    }
  }, [roomId, fetchRoomAndFriends])

  // フレンドのオンライン状態のライブ更新。GET /friends は開いた時点のスナップショットなので、
  // 以降の変化は user チャンネルの friend.presence_changed で追いかける（契約 asyncapi.yaml）。
  const { on } = useUserChannel()
  useEffect(() => {
    return on('friend.presence_changed', ({ userId, state }) => {
      setFriends((prev) =>
        prev.map((friend) =>
          friend.user.userId === userId
            ? {
                ...friend,
                presence: {
                  state,
                  lastSeenAt:
                    state === 'OFFLINE' ? new Date().toISOString() : null,
                },
              }
            : friend,
        ),
      )
    })
  }, [on])

  // 3. WebSocket / Channel 接続（room:{roomId}）
  useEffect(() => {
    if (!roomId) return

    let cancelled = false
    const socket = createSocket()
    socket.connect()

    const channel = socket.channel(`room:${roomId}`)
    channelRef.current = channel

    // room.state は契約 asyncapi.yaml の RoomStateSnapshot（進行中心）で、
    // REST の RoomState（部屋名・招待コード・設定を持つ）とは別物。
    // メタ情報は REST 取得分を保ち、参加者とホストだけ上書きする。
    const applySnapshot = (snapshot: RoomStateSnapshot) => {
      if (cancelled || !snapshot) return

      setRoom((prev) => {
        if (!prev) return prev
        const hostUserId = snapshot.hostUserId ?? prev.hostUserId

        return {
          ...prev,
          status: snapshot.phase === 'LOBBY' ? 'WAITING' : 'IN_GAME',
          hostUserId,
          settings: { ...prev.settings, ...snapshot.settings },
          players: snapshot.players.map((p) => ({
            userId: p.userId,
            displayName: p.displayName,
            avatarUrl: p.avatarUrl,
            isHost: p.userId === hostUserId,
            ready: p.ready,
            connected: p.connected,
          })),
          spectators: snapshot.spectators.map((s) => ({
            userId: s.userId,
            displayName: s.displayName,
            avatarUrl: s.avatarUrl,
          })),
        }
      })

      // ホストが開始したら LOBBY を抜ける。全員がこれでゲーム画面へ移る。
      // RESULT（結果の表示中。30秒で LOBBY に戻る）は、結果画面から「ロビーへ」で来た人を
      // ゲーム画面へ押し戻さないよう除く
      if (snapshot.phase !== 'LOBBY' && snapshot.phase !== 'RESULT') {
        navigate(`/game?roomId=${encodeURIComponent(roomId)}`)
      }
    }

    channel.on('room.state', applySnapshot)
    channel.on('room.removed', () => {
      if (cancelled) return
      cancelled = true
      setRemoved(true)
      channel.leave()
      socket.disconnect()
      channelRef.current = null
    })

    channel
      .join()
      .receive('ok', (reply: { publicState?: RoomStateSnapshot }) => {
        if (reply.publicState) applySnapshot(reply.publicState)
      })

    return () => {
      cancelled = true
      channel.leave()
      socket.disconnect()
      channelRef.current = null
    }
  }, [roomId, navigate])

  const handleToggleReady = useCallback(
    (nextReady: boolean) => {
      if (!channelRef.current || !room || !currentUserId) return
      setIsTogglingReady(true)

      // 楽観的更新
      setRoom((prev) => {
        if (!prev) return prev
        return {
          ...prev,
          players: prev.players.map((p) =>
            p.userId === currentUserId ? { ...p, ready: nextReady } : p,
          ),
        }
      })

      channelRef.current
        .push('room.ready', { ready: nextReady })
        .receive('ok', () => {
          setIsTogglingReady(false)
        })
        .receive('error', () => {
          setIsTogglingReady(false)
          setActionFeedback('準備状態の更新に失敗しました。')
          // ロールバック
          setRoom((prev) => {
            if (!prev) return prev
            return {
              ...prev,
              players: prev.players.map((p) =>
                p.userId === currentUserId ? { ...p, ready: !nextReady } : p,
              ),
            }
          })
        })
        .receive('timeout', () => {
          setIsTogglingReady(false)
          setActionFeedback('通信タイムアウトが発生しました。')
        })
    },
    [room, currentUserId],
  )

  // ゲーム開始（契約 openapi.yaml POST /rooms/{roomId}/start, startGame）。
  // 人数・準備・接続の検証はサーバー（GameServer）が行う。成功したら phase.changed が
  // 全員へ配信されるので、自分は /game へ移るだけでよい。
  const [startError, setStartError] = useState<string | null>(null)
  const [isStarting, setIsStarting] = useState(false)

  const handleStartGame = useCallback(async () => {
    if (!room) return
    setIsStarting(true)
    setStartError(null)

    const { error } = await api.POST('/rooms/{roomId}/start', {
      params: { path: { roomId: room.roomId } },
    })

    setIsStarting(false)
    if (error) setStartError(startMessage(error?.error?.code))
    else navigate(`/game?roomId=${encodeURIComponent(room.roomId)}`)
  }, [room, navigate])

  const handleLeaveRoom = useCallback(async () => {
    if (!room) return
    setIsLeaving(true)
    setLeaveError('')
    try {
      const response = await api.POST('/rooms/{roomId}/leave', {
        params: { path: { roomId: room.roomId } },
      })
      if (response.error && response.response.status !== 404) {
        setLeaveError('退出できませんでした。もう一度お試しください。')
        return
      }
      setLeaveDialogOpen(false)
      navigate('/home')
    } catch {
      setLeaveError('通信エラーが発生しました。もう一度お試しください。')
    } finally {
      setIsLeaving(false)
    }
  }, [room, navigate])

  async function changeParticipation(
    participantType: components['schemas']['ParticipantType'],
  ) {
    if (!room || isChangingParticipation) return
    setIsChangingParticipation(true)
    setParticipationError('')
    try {
      const response = await api.POST('/rooms/{roomId}/join', {
        params: { path: { roomId: room.roomId } },
        body: { participantType },
      })
      if (response.error || !response.data) {
        const code = response.error?.error.code
        setParticipationError(
          code === 'GAME_IN_PROGRESS'
            ? 'ゲームが始まったため、参加方法を変更できません。'
            : code === 'INVALID_PHASE'
              ? 'ホストを引き継ぐプレイヤーが必要です。'
              : '参加方法を変更できませんでした。空き状況を確認して、もう一度お試しください。',
        )
        return
      }
      setRoom(response.data.data.room)
      setHostSpectateDialogOpen(false)
    } catch {
      setParticipationError(
        '通信エラーが発生しました。もう一度お試しください。',
      )
    } finally {
      setIsChangingParticipation(false)
    }
  }

  async function copyInviteUrl(
    participantType: components['schemas']['ParticipantType'],
  ) {
    if (!room) return
    const inviteUrl = roomInviteUrl(room.inviteCode, participantType)
    try {
      await navigator.clipboard.writeText(inviteUrl)
      setCopied(participantType)
    } catch {
      // clipboard API が非対応の場合
      setActionFeedback('URLをクリップボードにコピーできませんでした。')
    }
  }

  async function handleInviteFriend(friend: Friend) {
    if (!room || isInviting[friend.user.userId]) return
    const targetUserId = friend.user.userId
    setIsInviting((prev) => ({ ...prev, [targetUserId]: true }))
    setActionFeedback(null)

    try {
      const res = await api.POST('/rooms/{roomId}/invites', {
        params: { path: { roomId: room.roomId } },
        body: { targetUserId },
      })

      if (res.error) {
        if (res.response?.status === 409) {
          setActionFeedback(
            `${friend.user.displayName}さんはすでに参加しています。`,
          )
          setInvitedUserIds((prev) => new Set(prev).add(targetUserId))
        } else {
          setActionFeedback('招待の送信に失敗しました。')
        }
        return
      }

      setInvitedUserIds((prev) => new Set(prev).add(targetUserId))
      setActionFeedback(`${friend.user.displayName}さんへ招待を送信しました！`)
      setTimeout(() => setActionFeedback(null), 4000)
    } catch {
      setActionFeedback('通信エラーが発生しました。')
    } finally {
      setIsInviting((prev) => ({ ...prev, [targetUserId]: false }))
    }
  }

  if (removed) {
    return (
      <MobileShell title="ロビー" backHref="/home">
        <main className="flex min-h-[50vh] flex-col items-center justify-center gap-4 p-5 text-center">
          <p role="status" className="text-sm">
            ホストにより、この部屋から退出しました。
          </p>
          <Link className={cn(buttonVariants())} href="/home">
            ホームへ戻る
          </Link>
        </main>
      </MobileShell>
    )
  }

  if (isLoading) {
    return (
      <MobileShell title="ロビー" backHref="/home">
        <main className="flex min-h-[50vh] items-center justify-center p-5">
          <Loader2 className="text-muted-foreground size-6 animate-spin" />
        </main>
      </MobileShell>
    )
  }

  if (error || !room) {
    return (
      <MobileShell title="ロビー" backHref="/home">
        <main className="flex min-h-[50vh] flex-col items-center justify-center gap-4 p-5 text-center">
          <p className="text-destructive text-sm">
            {error ?? 'エラーが発生しました'}
          </p>
          <div className="flex gap-2">
            <Link
              className={cn(buttonVariants({ variant: 'outline', size: 'sm' }))}
              href="/rooms"
            >
              部屋を探す
            </Link>
            <Link
              className={cn(buttonVariants({ size: 'sm' }))}
              href="/rooms/new"
            >
              部屋を作る
            </Link>
          </div>
        </main>
      </MobileShell>
    )
  }

  const isHost = currentUserId ? room.hostUserId === currentUserId : false
  const myPlayer = room.players.find((p) => p.userId === currentUserId)
  const isPlayer = !!myPlayer
  const existingMemberUserIds = new Set([
    ...room.players.map((p) => p.userId),
    ...(room.spectators?.map((s) => s.userId) ?? []),
  ])
  const invitableFriends = friends.filter(
    (f) => !existingMemberUserIds.has(f.user.userId),
  )
  const onlineFriends = invitableFriends.filter(
    (f) => f.presence.state === 'ONLINE',
  )
  const otherFriends = invitableFriends.filter(
    (f) => f.presence.state !== 'ONLINE',
  )
  const sortedFriends = [...onlineFriends, ...otherFriends]
  const visibleFriends = showAllFriends
    ? sortedFriends
    : sortedFriends.slice(0, 5)
  const hasMoreFriends = sortedFriends.length > 5

  return (
    <MobileShell
      title={room.roomName}
      onBack={() => setLeaveDialogOpen(true)}
      actions={
        <Button
          aria-label="部屋から退出"
          className="text-stone-600 hover:bg-stone-100 hover:text-stone-900"
          size="icon"
          variant="ghost"
          onClick={() => setLeaveDialogOpen(true)}
        >
          <LogOut className="size-5" aria-hidden="true" />
        </Button>
      }
    >
      <main className="space-y-6 px-5 py-5">
        {actionFeedback && (
          <div className="rounded-lg border border-emerald-200 bg-emerald-50 p-3 text-xs text-emerald-800">
            {actionFeedback}
          </div>
        )}

        {/* ロビー概要 */}
        <StatsStrip
          label="ロビーの概要"
          items={[
            {
              label: '参加者',
              value: `${room.players.length} / ${room.settings.maxPlayers}`,
            },
            { label: '観戦者', value: `${room.spectators.length}` },
            { label: 'ジャンル', value: `${room.settings.genres.length}種類` },
          ]}
        />

        {/* 参加方法ごとの招待URL */}
        <section>
          <SectionHeader title="招待URL" />
          <div className="mt-3 flex items-center gap-2 rounded-xl border bg-white p-2">
            <select
              aria-label="招待する参加方法"
              className="border-input focus-visible:ring-ring h-10 shrink-0 rounded-lg border bg-white px-2 text-xs focus-visible:ring-2"
              value={inviteParticipantType}
              onChange={(event) => {
                setInviteParticipantType(
                  event.target.value === 'SPECTATOR' ? 'SPECTATOR' : 'PLAYER',
                )
                setCopied(null)
              }}
            >
              <option value="PLAYER">プレイヤー</option>
              <option value="SPECTATOR">観戦者</option>
            </select>
            <Input
              aria-label={`${inviteParticipantType === 'PLAYER' ? 'プレイヤー' : '観戦者'}として招待するリンク`}
              className="h-10 min-w-0 flex-1 font-mono text-xs"
              readOnly
              value={roomInviteUrl(room.inviteCode, inviteParticipantType)}
            />
            <Button
              size="icon"
              className="size-10 shrink-0"
              type="button"
              variant="outline"
              aria-label={`${inviteParticipantType === 'PLAYER' ? 'プレイヤー' : '観戦者'}として招待`}
              title="招待リンクをコピー"
              onClick={() => void copyInviteUrl(inviteParticipantType)}
            >
              {copied === inviteParticipantType ? (
                <Check className="size-4" aria-hidden="true" />
              ) : (
                <Copy className="size-4" aria-hidden="true" />
              )}
            </Button>
          </div>
          {copied === inviteParticipantType && (
            <p className="mt-2 text-xs text-emerald-700" role="status">
              {copied === 'SPECTATOR' ? '観戦用' : 'プレイヤー用'}
              の招待リンクをコピーしました。
            </p>
          )}
        </section>

        {/* オンラインフレンドを招待 */}
        <section>
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-1.5">
              <Sparkles className="text-brass size-4" />
              <h2 className="text-base font-semibold text-balance">
                フレンドを招待
              </h2>
            </div>
            <div className="flex items-center gap-1.5">
              <Badge variant="outline" className="text-xs">
                オンライン {onlineFriends.length}人
              </Badge>
              <Badge variant="secondary" className="text-xs">
                候補 {sortedFriends.length}人
              </Badge>
            </div>
          </div>

          <div className="mt-3 space-y-2">
            {invitableFriends.length === 0 ? (
              <div className="text-muted-foreground rounded-xl border border-dashed p-4 text-center text-xs">
                <p>
                  {friends.length === 0
                    ? 'フレンドがいません。'
                    : '招待可能なフレンドはいません（全員参加中）'}
                </p>
                {friends.length === 0 && (
                  <Link
                    className="text-primary mt-2 inline-block font-medium underline"
                    href="/friends"
                  >
                    フレンドを探す
                  </Link>
                )}
              </div>
            ) : (
              <>
                {visibleFriends.map((friend) => {
                  const isInvited = invitedUserIds.has(friend.user.userId)
                  const initials = friend.user.displayName
                    .slice(0, 2)
                    .toUpperCase()
                  const inProgress = isInviting[friend.user.userId]
                  const isOnline = friend.presence.state === 'ONLINE'
                  const isActive = friend.presence.state !== 'OFFLINE'

                  return (
                    <div
                      key={friend.user.userId}
                      className={cn(
                        'flex items-center justify-between gap-3 rounded-lg border p-3 shadow-none transition-colors',
                        isActive ? 'bg-stone-50/90' : 'bg-white opacity-80',
                      )}
                    >
                      <div className="flex min-w-0 items-center gap-2.5">
                        <div className="relative">
                          <PlayerAvatar
                            className="size-9 border"
                            src={friend.user.avatarUrl}
                            initials={initials}
                            name={friend.user.displayName}
                          />
                          {isActive && (
                            <span className="absolute right-0 bottom-0 size-2.5 rounded-full bg-emerald-500 ring-2 ring-white" />
                          )}
                        </div>
                        <div className="min-w-0">
                          <p
                            className={cn(
                              'truncate text-sm font-semibold',
                              isActive ? 'text-stone-800' : 'text-stone-100',
                            )}
                          >
                            {friend.user.displayName}
                          </p>
                          {isOnline ? (
                            <span className="inline-flex items-center gap-1 text-[11px] font-medium text-emerald-800">
                              オンライン
                            </span>
                          ) : (
                            <span
                              className={cn(
                                'text-[11px]',
                                isActive ? 'text-amber-800' : 'text-stone-300',
                              )}
                            >
                              {friend.presence.state === 'IN_GAME'
                                ? '対戦中'
                                : 'オフライン'}
                            </span>
                          )}
                        </div>
                      </div>

                      <div>
                        <Button
                          size="sm"
                          variant={
                            isInvited || !isOnline ? 'outline' : 'default'
                          }
                          className="h-8 text-xs"
                          disabled={inProgress}
                          onClick={() => void handleInviteFriend(friend)}
                        >
                          {inProgress ? (
                            <Loader2
                              className="mr-1 size-3.5 animate-spin"
                              aria-hidden="true"
                            />
                          ) : (
                            <Send
                              className="mr-1 size-3.5"
                              aria-hidden="true"
                            />
                          )}
                          {inProgress
                            ? '送信中…'
                            : isInvited
                              ? '再招待'
                              : '招待'}
                        </Button>
                      </div>
                    </div>
                  )
                })}

                {/* 5人以上の場合はプルダウンで折りたたみ/展開 */}
                {hasMoreFriends && (
                  <Button
                    type="button"
                    variant="outline"
                    size="sm"
                    className="text-muted-foreground hover:text-foreground mt-1 h-9 w-full text-xs"
                    onClick={() => setShowAllFriends((prev) => !prev)}
                  >
                    {showAllFriends ? (
                      <>
                        <ChevronUp className="mr-1 size-3.5" />
                        フレンド一覧を折りたたむ
                      </>
                    ) : (
                      <>
                        <ChevronDown className="mr-1 size-3.5" />
                        他のフレンドを表示（あと{sortedFriends.length - 5}人）
                      </>
                    )}
                  </Button>
                )}
              </>
            )}
          </div>
        </section>

        {/* 参加プレイヤー */}
        <section>
          <SectionHeader
            title="参加プレイヤー"
            meta={
              <Badge variant="outline">
                {room.players.length} / {room.settings.maxPlayers}人
              </Badge>
            }
          />
          <div className="mt-3 space-y-2">
            {room.players.map((player) => {
              const initials = player.displayName.slice(0, 2).toUpperCase()
              return (
                <div
                  key={player.userId}
                  className="flex items-center justify-between gap-3 rounded-xl border bg-white p-3.5 shadow-none"
                >
                  <div className="flex min-w-0 items-center gap-3">
                    <span className="relative shrink-0">
                      <PlayerAvatar
                        className="size-10 border"
                        src={player.avatarUrl}
                        initials={initials}
                        name={player.displayName}
                      />
                      <span
                        role="img"
                        aria-label={
                          player.connected ? 'オンライン' : 'オフライン'
                        }
                        title={player.connected ? 'オンライン' : 'オフライン'}
                        className={cn(
                          'absolute right-0 bottom-0 size-3 rounded-full ring-2 ring-white',
                          player.connected
                            ? 'bg-emerald-500'
                            : 'border border-stone-400 bg-stone-200',
                        )}
                      />
                    </span>
                    <div className="min-w-0">
                      <div className="flex flex-wrap items-center gap-1.5">
                        <ParticipantProfileButton
                          userId={player.userId}
                          displayName={player.displayName}
                          currentUserId={currentUserId ?? undefined}
                        />
                        {player.isHost && (
                          <Badge
                            variant="outline"
                            className="border-amber-400 bg-amber-50 px-1 py-0 text-[10px] font-semibold text-amber-900"
                          >
                            <Crown className="mr-0.5 size-3 text-amber-600" />
                            ホスト
                          </Badge>
                        )}
                        {player.userId === currentUserId && (
                          <Badge
                            variant="secondary"
                            className="bg-sky-100 px-1 py-0 text-[10px] font-medium text-sky-800"
                          >
                            あなた
                          </Badge>
                        )}
                      </div>
                    </div>
                  </div>

                  <div className="flex shrink-0 flex-col items-end gap-1">
                    {player.ready ? (
                      <Badge className="bg-emerald-600 text-white hover:bg-emerald-600">
                        <Check className="mr-1 size-3" />
                        準備完了
                      </Badge>
                    ) : (
                      <Badge
                        variant="outline"
                        className="text-muted-foreground"
                      >
                        準備中
                      </Badge>
                    )}
                    {isHost && player.userId !== currentUserId && (
                      <RemoveRoomMemberButton
                        roomId={room.roomId}
                        member={player}
                        disabled={isStarting || room.status !== 'WAITING'}
                        onRemoved={(updated) =>
                          setRoom((current) =>
                            current?.roomId === updated.roomId
                              ? updated
                              : current,
                          )
                        }
                      />
                    )}
                  </div>
                </div>
              )
            })}
          </div>
        </section>

        {room.spectators.length > 0 && (
          <section>
            <SectionHeader title="観戦者" />
            <div className="mt-3 space-y-2">
              {room.spectators.map((spectator) => (
                <div
                  key={spectator.userId}
                  className="flex items-center justify-between gap-3 rounded-xl border bg-white p-3.5"
                >
                  <ParticipantProfileButton
                    userId={spectator.userId}
                    displayName={spectator.displayName}
                    currentUserId={currentUserId ?? undefined}
                  />
                  {isHost && (
                    <RemoveRoomMemberButton
                      roomId={room.roomId}
                      member={spectator}
                      disabled={isStarting || room.status !== 'WAITING'}
                      onRemoved={(updated) =>
                        setRoom((current) =>
                          current?.roomId === updated.roomId
                            ? updated
                            : current,
                        )
                      }
                    />
                  )}
                </div>
              ))}
            </div>
          </section>
        )}

        {/* ゲーム設定 */}
        <Card className="shadow-none">
          <CardContent className="p-4">
            <SectionHeader title="ゲーム設定" />
            {isHost ? (
              <div className="mt-3">
                <RoomSettingsEditor
                  key={`${room.roomId}:${room.settings.genres.join()}:${room.settings.comebackEnabled}:${room.settings.discussionMs}:${room.settings.voteMs}`}
                  room={room}
                  onSaved={(updated) =>
                    setRoom((current) =>
                      current?.roomId === updated.roomId ? updated : current,
                    )
                  }
                />
              </div>
            ) : (
              <dl className="mt-3 space-y-2 text-sm">
                <div className="flex items-center justify-between gap-3">
                  <dt className="text-muted-foreground">お題ジャンル</dt>
                  <dd className="text-right font-medium">
                    {room.settings.genres
                      .map((g) => genreLabels[g] ?? g)
                      .join('・')}
                  </dd>
                </div>
                <div className="flex items-center justify-between gap-3">
                  <dt className="text-muted-foreground">逆転モード</dt>
                  <dd>
                    <Badge variant="outline">
                      {room.settings.comebackEnabled
                        ? 'あり（30秒・入力を公開）'
                        : 'なし'}
                    </Badge>
                  </dd>
                </div>
                <div className="flex items-center justify-between gap-3">
                  <dt className="text-muted-foreground">議論時間</dt>
                  <dd className="font-mono font-medium tabular-nums">
                    {formatRoomDuration(room.settings.discussionMs)}
                  </dd>
                </div>
                <div className="flex items-center justify-between gap-3">
                  <dt className="text-muted-foreground">投票時間</dt>
                  <dd className="font-mono font-medium tabular-nums">
                    {formatRoomDuration(room.settings.voteMs)}
                  </dd>
                </div>
                <div className="flex items-center justify-between gap-3">
                  <dt className="text-muted-foreground">参加人数</dt>
                  <dd className="font-mono font-medium tabular-nums">
                    {room.settings.minPlayers}〜{room.settings.maxPlayers}人
                  </dd>
                </div>
              </dl>
            )}
          </CardContent>
        </Card>

        <section
          aria-label="参加方法"
          className="space-y-3 rounded-xl border bg-white p-4"
        >
          <p className="text-sm font-medium">
            {isPlayer ? 'プレイヤーとして参加中' : '観戦者として参加中'}
          </p>
          <Button
            className="h-11 w-full"
            variant="outline"
            disabled={
              isChangingParticipation ||
              isStarting ||
              room.status !== 'WAITING' ||
              (isHost && room.players.length < 2) ||
              (!isPlayer && room.players.length >= room.settings.maxPlayers)
            }
            onClick={() => {
              setParticipationError('')
              if (isHost) setHostSpectateDialogOpen(true)
              else void changeParticipation(isPlayer ? 'SPECTATOR' : 'PLAYER')
            }}
          >
            {isPlayer ? (
              <Eye className="mr-1 size-4" aria-hidden="true" />
            ) : (
              <Users className="mr-1 size-4" aria-hidden="true" />
            )}
            {isChangingParticipation
              ? '切り替え中…'
              : isPlayer
                ? '観戦に切り替える'
                : 'プレイヤーに切り替える'}
          </Button>
          {isHost && (
            <p className="text-muted-foreground text-xs text-pretty">
              {room.players.length < 2
                ? 'ほかのプレイヤーが参加すると、ホストを引き継いで観戦できます。'
                : '観戦に切り替えると、ほかのプレイヤーにホストを引き継ぎます。'}
            </p>
          )}
          {!isPlayer && room.players.length >= room.settings.maxPlayers && (
            <p className="text-muted-foreground text-xs">
              プレイヤー枠が空くと切り替えられます。
            </p>
          )}
          {participationError && !hostSpectateDialogOpen && (
            <p className="text-destructive text-sm" role="alert">
              {participationError}
            </p>
          )}
        </section>

        {/* ホスト / プレイヤー / 観戦者操作 */}
        {isHost ? (
          <div className="space-y-3">
            <Button
              className="h-11 w-full"
              disabled={
                room.players.length < room.settings.minPlayers || isStarting
              }
              onClick={() => void handleStartGame()}
            >
              <Gamepad2 className="mr-1.5 size-4" />
              {room.players.length < room.settings.minPlayers
                ? `プレイヤーが足りません（残り${room.settings.minPlayers - room.players.length}人）`
                : isStarting
                  ? '開始しています…'
                  : 'ゲームを開始する'}
            </Button>
            {startError && (
              <p className="text-center text-xs text-rose-600">{startError}</p>
            )}
            <p className="text-muted-foreground text-center text-xs">
              あなたはホストです（最低{room.settings.minPlayers}人必要）
            </p>
            <Button
              variant="outline"
              className="h-10 w-full text-xs text-rose-700 hover:bg-rose-50 hover:text-rose-800"
              disabled={isStarting || isLeaving}
              onClick={() => setLeaveDialogOpen(true)}
            >
              <LogOut className="mr-1.5 size-3.5" />
              部屋から退出する（ホスト移譲）
            </Button>
          </div>
        ) : isPlayer ? (
          <div className="space-y-3">
            <Button
              className={cn(
                'h-11 w-full text-sm font-semibold transition-colors',
                myPlayer?.ready
                  ? 'border border-emerald-300 bg-emerald-50 text-emerald-800 hover:bg-emerald-100 hover:text-emerald-900'
                  : 'bg-primary text-primary-foreground hover:bg-primary/90',
              )}
              aria-pressed={myPlayer?.ready ?? false}
              onClick={() =>
                void handleToggleReady(!(myPlayer?.ready ?? false))
              }
              variant={myPlayer?.ready ? 'outline' : 'default'}
              disabled={isTogglingReady}
            >
              {isTogglingReady ? (
                <Loader2 className="mr-1.5 size-4 animate-spin" />
              ) : myPlayer?.ready ? (
                <Check
                  className="mr-1.5 size-4 text-emerald-600"
                  aria-hidden="true"
                />
              ) : null}
              {myPlayer?.ready
                ? '準備完了（タップで準備中に変更）'
                : '準備完了にする'}
            </Button>

            <div
              className="bg-night rounded-xl border px-4 py-4 text-center text-white"
              role="status"
            >
              <p className="text-sm font-semibold">
                ホストの開始を待っています
              </p>
              <p className="mt-1 text-xs text-stone-400">
                {myPlayer?.ready
                  ? 'あなたの準備は完了しています'
                  : '準備が完了したら上のボタンを押してください'}
              </p>
            </div>

            <Button
              variant="outline"
              className="h-10 w-full text-xs text-rose-700 hover:bg-rose-50 hover:text-rose-800"
              disabled={isTogglingReady || isLeaving}
              onClick={() => setLeaveDialogOpen(true)}
            >
              <LogOut className="mr-1.5 size-3.5" />
              ロビーから退出する
            </Button>
          </div>
        ) : (
          <div
            className="space-y-3 rounded-xl border bg-stone-100 p-4 text-center text-stone-700"
            role="status"
          >
            <div>
              <Badge
                variant="secondary"
                className="mb-1 bg-stone-200 font-semibold text-stone-800"
              >
                観戦待機中
              </Badge>
              <p className="text-sm font-semibold">
                ゲームの開始をお待ちください
              </p>
              <p className="text-muted-foreground mt-0.5 text-xs">
                プレイヤーがゲームを開始すると自動的に観戦画面へ切り替わります
              </p>
            </div>
            <Button
              variant="outline"
              className="h-9 w-full text-xs text-rose-700 hover:bg-rose-50 hover:text-rose-800"
              disabled={isLeaving}
              onClick={() => setLeaveDialogOpen(true)}
            >
              <LogOut className="mr-1.5 size-3.5" />
              観戦をやめて部屋を出る
            </Button>
          </div>
        )}

        <Separator />
        <Button
          variant="ghost"
          className="text-muted-foreground w-full text-xs"
          disabled={isLeaving}
          onClick={() => setLeaveDialogOpen(true)}
        >
          <Users className="mr-1 size-4" />
          別の部屋を探す
        </Button>
      </main>

      {/* 退出確認ダイアログ */}
      <AlertDialog open={leaveDialogOpen} onOpenChange={setLeaveDialogOpen}>
        <AlertDialogContent>
          <AlertDialogHeader>
            <AlertDialogMedia>
              <LogOut className="size-5" aria-hidden="true" />
            </AlertDialogMedia>
            <AlertDialogTitle>
              {isHost
                ? '部屋から退出しますか？'
                : isPlayer
                  ? 'ロビーから退出しますか？'
                  : '観戦をやめて退出しますか？'}
            </AlertDialogTitle>
            <AlertDialogDescription>
              {isHost
                ? room.players.length > 1
                  ? '退出するとホスト権限は次のプレイヤーへ移譲されます。'
                  : 'あなたが最後のプレイヤーのため、退出するとこの部屋は解散されます。'
                : isPlayer
                  ? 'ロビーから退出してホーム画面へ移動します。'
                  : '観戦を終了してホーム画面へ移動します。'}
            </AlertDialogDescription>
          </AlertDialogHeader>
          {leaveError && (
            <p className="text-destructive text-sm" role="alert">
              {leaveError}
            </p>
          )}
          <AlertDialogFooter>
            <AlertDialogCancel disabled={isLeaving}>
              キャンセル
            </AlertDialogCancel>
            <AlertDialogAction
              variant="destructive"
              disabled={isLeaving}
              onClick={(e) => {
                e.preventDefault()
                void handleLeaveRoom()
              }}
            >
              {isLeaving ? (
                <Loader2 className="mr-1 size-3.5 animate-spin" />
              ) : null}
              退出する
            </AlertDialogAction>
          </AlertDialogFooter>
        </AlertDialogContent>
      </AlertDialog>

      <AlertDialog
        open={hostSpectateDialogOpen}
        onOpenChange={setHostSpectateDialogOpen}
      >
        <AlertDialogContent>
          <AlertDialogHeader>
            <AlertDialogTitle>
              ホストを引き継いで観戦しますか？
            </AlertDialogTitle>
            <AlertDialogDescription>
              先に参加した別のプレイヤーにホストを引き継ぎ、あなたは観戦者になります。
            </AlertDialogDescription>
          </AlertDialogHeader>
          {participationError && (
            <p className="text-destructive text-sm" role="alert">
              {participationError}
            </p>
          )}
          <AlertDialogFooter>
            <AlertDialogCancel disabled={isChangingParticipation}>
              キャンセル
            </AlertDialogCancel>
            <AlertDialogAction
              disabled={isChangingParticipation}
              onClick={(event) => {
                event.preventDefault()
                void changeParticipation('SPECTATOR')
              }}
            >
              ホストを引き継いで観戦する
            </AlertDialogAction>
          </AlertDialogFooter>
        </AlertDialogContent>
      </AlertDialog>
    </MobileShell>
  )
}

// 契約 startGame のエラー語彙を画面の文言へ。
function startMessage(code: string | undefined): string {
  switch (code) {
    case 'NOT_HOST':
      return 'ホストだけがゲームを開始できます。'
    case 'NOT_READY':
      return '全員が接続して準備完了になる必要があります。'
    case 'GAME_IN_PROGRESS':
      return 'すでにゲームが始まっています。'
    case 'NOT_FOUND':
      return 'お題が見つかりませんでした。部屋のジャンル設定を見直してください。'
    default:
      return '開始できませんでした。時間をおいて試してください。'
  }
}
