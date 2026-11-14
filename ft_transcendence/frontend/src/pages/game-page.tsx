import { useCallback, useEffect, useMemo, useState } from 'react'

import {
  AlertTriangle,
  Gavel,
  Loader2,
  LogOut,
  ShieldCheck,
  Users,
} from 'lucide-react'
import { motion, useReducedMotion } from 'motion/react'
import { useLocation } from 'wouter'

import { MobileShell } from '@/components/layout/mobile-shell'
import { Button } from '@/components/ui/button'
import { Badge } from '@/components/ui/badge'
import { Progress } from '@/components/ui/progress'
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
  AlertDialogTrigger,
} from '@/components/ui/alert-dialog'
import { ComebackPanel } from '@/components/game-ui/comeback-panel'
import { ChatThreadView } from '@/components/game-ui/chat-thread'
import { ResultCinematic } from '@/components/game-ui/result-cinematic'
import { GameCountdownPage } from '@/pages/game-countdown-page'
import { useMe } from '@/hooks/use-me'
import { useRoomChannel } from '@/hooks/use-room-channel'
import { useUserChannel } from '@/hooks/use-user-channel'
import { useWakeLock } from '@/hooks/use-wake-lock'
import { api } from '@/lib/api-client'
import { SpectatorAssignments } from '@/components/game-ui/spectator-assignments'
import { GameTimer } from '@/components/game-ui/game-timer'
import { ParticipantProfileButton } from '@/components/game-ui/participant-profile-button'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import {
  PlayerRoster,
  type RosterPlayer,
} from '@/components/game-ui/player-roster'
import { WordCard } from '@/components/game-ui/word-card'
import type {
  GameResult,
  PrivateState,
  RoomStateSnapshot,
} from '@/types/ws/events'

const genreLabels: Record<string, string> = {
  ROMANCE: '恋愛',
  VALUES: '本音・価値観',
  PLACE: '場所',
  FOOD: '食べ物',
  HOBBY: '趣味',
  DAILY_ITEM: '日用品',
  WORK_SCHOOL: '学校・仕事',
  ENTERTAINMENT: 'エンタメ',
}

/** 契約の player を表示部品の Player 型へ寄せる。 */
function toRosterPlayer(
  player: RoomStateSnapshot['players'][number],
  hostUserId: string,
  meId?: string,
): RosterPlayer {
  return {
    id: player.userId,
    name: player.displayName,
    initials: player.displayName.slice(0, 2),
    connected: player.connected,
    ready: player.ready,
    isHost: player.userId === hostUserId,
    isCurrentUser: player.userId === meId,
    avatarSrc: player.avatarUrl,
  }
}

/** 締切までの残りを mm:ss に。締切なし（一時停止中など）は「--:--」。 */
// 「議論へ」を押したゲームの id。リロードしてもお題めくりをやり直させないためにタブ単位で覚える
// （sessionStorage。開発用の /dev/as でタブごとに別ユーザーでも混ざらない）。
const CONFIRMED_GAME_STORAGE_KEY = 'word_wolf_confirmed_game_id'

function readConfirmedGameId(): string | null {
  try {
    return sessionStorage.getItem(CONFIRMED_GAME_STORAGE_KEY)
  } catch {
    return null
  }
}

function writeConfirmedGameId(gameId: string): void {
  try {
    sessionStorage.setItem(CONFIRMED_GAME_STORAGE_KEY, gameId)
  } catch {
    // 保存できなくても、このタブを開いている間は state で覚えている
  }
}

// 見ている結果。スリープ復帰でブラウザがタブを読み込み直すとメモリ上の結果（result.published）は
// 消えるので、タブ単位でも持つ。「ロビーへ」「ホーム」を押したとき・次のゲームが始まったときに消す。
const HELD_RESULT_STORAGE_KEY = 'word_wolf_held_result'
const COMPLETED_RESULT_STORAGE_KEY = 'word_wolf_completed_result'

function readCompletedResult(): string | null {
  try {
    return sessionStorage.getItem(COMPLETED_RESULT_STORAGE_KEY)
  } catch {
    return null
  }
}

function readHeldResult(roomId: string): GameResult | null {
  try {
    const raw = sessionStorage.getItem(HELD_RESULT_STORAGE_KEY)
    if (!raw) return null
    const held: unknown = JSON.parse(raw)
    if (
      typeof held !== 'object' ||
      held === null ||
      !('roomId' in held) ||
      held.roomId !== roomId ||
      !('result' in held)
    )
      return null
    const result = held.result as GameResult
    return typeof result?.gameId === 'string' && Array.isArray(result.players)
      ? result
      : null
  } catch {
    return null
  }
}

function writeHeldResult(roomId: string, result: GameResult): void {
  try {
    sessionStorage.setItem(
      HELD_RESULT_STORAGE_KEY,
      JSON.stringify({ roomId, result }),
    )
  } catch {
    // 保存できなくても、タブを開いている間はメモリの結果で表示できる
  }
}

function clearHeldResult(): void {
  try {
    sessionStorage.removeItem(HELD_RESULT_STORAGE_KEY)
    sessionStorage.removeItem(COMPLETED_RESULT_STORAGE_KEY)
  } catch {
    // sessionStorage が使えない環境では何もしない
  }
}

function remaining(deadline: string | null): string {
  if (!deadline) return '--:--'
  const ms = new Date(deadline).getTime() - Date.now()
  if (!Number.isFinite(ms) || ms <= 0) return '00:00'
  const total = Math.floor(ms / 1000)
  return `${String(Math.floor(total / 60)).padStart(2, '0')}:${String(total % 60).padStart(2, '0')}`
}

export function GamePage() {
  const [, navigate] = useLocation()
  const { user } = useMe()
  const reducedMotion = useReducedMotion()
  const [entryDeadline, setEntryDeadline] = useState<number | null>(null)
  const [revealed, setRevealed] = useState(false)
  const [confirmedGameId, setConfirmedGameId] = useState<string | null>(
    readConfirmedGameId,
  )
  const [showResult, setShowResult] = useState(false)
  const [completedResult] = useState(readCompletedResult)
  const [perspectiveUserId, setPerspectiveUserId] = useState<string | null>(
    null,
  )
  const [submittingVote, setSubmittingVote] = useState(false)
  const [voteDialogOpen, setVoteDialogOpen] = useState(false)
  const [roomId, setRoomId] = useState<string | null>(null)
  const [meId, setMeId] = useState<string | undefined>()
  const [assignment, setAssignment] = useState<PrivateState | null>(null)
  const [selected, setSelected] = useState<string | null>(null)
  const [voteError, setVoteError] = useState<string | null>(null)
  const [voted, setVoted] = useState(false)

  // 1. roomId は URL の ?roomId= か、GET /users/me の activeRoom（lobby-page と同じ流儀）
  useEffect(() => {
    const fromQuery = new URLSearchParams(window.location.search).get('roomId')

    void (async () => {
      const { data } = await api.GET('/users/me')
      if (data?.status === 'ok') setMeId(data.data.user.userId)

      if (fromQuery) {
        setRoomId(fromQuery)
        return
      }

      const active = data?.status === 'ok' ? data.data.activeRoom : null
      if (active) setRoomId(active.roomId)
      else navigate('/home')
    })()
  }, [navigate])

  // ChatThread は内部で独自に room チャンネルへ Join するため、ここでは使わない。
  // 同じユーザーが二重に Join すると GameServer が古い接続を置換してしまう
  // （契約: 有効な最新接続だけが維持される）。チャットもこのフックの send で送る。
  const {
    roomState,
    result: liveResult,
    replaced,
    messages,
    status,
    sendError,
    send,
    setSkip,
    updateComebackDraft,
    submitComeback,
  } = useRoomChannel(roomId ?? '')
  const [skipAgreed, setSkipAgreed] = useState(false)

  // ゲーム中は画面が自動でスリープしないようにする（切断 → 一時停止の入口を減らす）
  useWakeLock(true)

  // スリープ復帰・リロードで読み込み直したときは、保存しておいた結果を使う。
  // 部屋が分かった描画の時点で読む（effect で後から入れると、先に LOBBY → /lobby へ飛んでしまう）
  const storedResult = useMemo(
    () => (roomId ? readHeldResult(roomId) : null),
    [roomId],
  )
  // 次のゲームが始まったら（gameId が変わったら）前のゲームの結果は捨てる。
  // その後 LOBBY で gameId が null に戻っても復活させないよう、捨てたことを覚える
  const [storedDiscarded, setStoredDiscarded] = useState(false)
  if (
    !storedDiscarded &&
    storedResult &&
    roomState?.gameId &&
    roomState.gameId !== storedResult.gameId
  ) {
    setStoredDiscarded(true)
  }
  const restoredResult = storedDiscarded ? null : storedResult
  useEffect(() => {
    if (storedDiscarded) clearHeldResult()
  }, [storedDiscarded])
  useEffect(() => {
    if (roomId && liveResult) writeHeldResult(roomId, liveResult)
  }, [roomId, liveResult])
  const result = liveResult ?? restoredResult

  // 結果画面は「ロビーへ」「ホーム」を押すまで表示し続ける。サーバーは RESULT を30秒で
  // LOBBY に戻す（契約 asyncapi.yaml）が、結果を見ている途中で画面を切り替えない。
  // 次のゲームが始まれば（DEALING 以降）そちらを表示する。
  const holdingResult =
    result !== null &&
    (roomState?.phase === 'RESULT' || roomState?.phase === 'LOBBY')

  // 2. 自分のお題は本人チャンネルから（公開状態には載らない）
  const { on } = useUserChannel()
  useEffect(() => on('private.state', setAssignment), [on])

  // 3. LOBBY に戻ったらロビー画面へ（ゲーム中止・次の回の待機）。結果を見ている間は留まる
  useEffect(() => {
    if (roomId && roomState?.phase === 'LOBBY' && !holdingResult)
      navigate(`/lobby?roomId=${encodeURIComponent(roomId ?? '')}`)
  }, [roomId, roomState?.phase, holdingResult, navigate])

  // フェーズが変わったら投票の選択状態を捨てる
  useEffect(() => {
    setSelected(null)
    setVoted(false)
    setVoteError(null)
    setVoteDialogOpen(false)
    setSkipAgreed(false)
  }, [roomState?.phase])

  const gameId = roomState?.gameId ?? null
  useEffect(() => {
    // LOBBY に戻ると gameId は null になる。そこでは戻さない（開票の演出を流し直さない）
    if (!gameId) return
    setShowResult(false)
    setRevealed(false)
    // 保存済みの id が今のゲームなら確認済み（リロード）、違えば新しいゲームなのでめくり直す
    setConfirmedGameId(readConfirmedGameId())
  }, [gameId])
  // 結果の受信と演出の完了を区別し、途中のリロードでは結果一覧を先に出さない。
  const cinematicDone =
    showResult ||
    (restoredResult !== null &&
      completedResult === restoredResult.gameId &&
      (gameId === null || gameId === restoredResult.gameId))
  useEffect(() => {
    if (roomState?.phase === 'DEALING' && roomState.deadline) {
      setEntryDeadline(new Date(roomState.deadline).getTime())
    }
  }, [roomState?.phase, roomState?.deadline])
  const completeEntry = useCallback(() => setEntryDeadline(null), [])

  const castVote = useCallback(async () => {
    if (!gameId || !selected || voted || submittingVote) return
    setSubmittingVote(true)

    try {
      const { error } = await api.POST('/games/{gameId}/votes', {
        params: { path: { gameId } },
        body: { targetUserId: selected },
      })

      if (error) setVoteError(error?.error?.code ?? 'NETWORK')
      else setVoted(true)
    } catch {
      setVoteError('NETWORK')
    } finally {
      setSubmittingVote(false)
    }
  }, [gameId, selected, voted, submittingVote])

  // 参加者は本人、観戦者は選択した参加者（初期値はホスト）の視点で結果を見る。
  const me = result?.players.find((p) => p.userId === meId)
  const resultPerspective =
    me ??
    result?.players.find((player) => player.userId === perspectiveUserId) ??
    result?.players.find((player) => player.userId === roomState?.hostUserId) ??
    result?.players[0]
  const resultPerspectiveId = resultPerspective?.userId
  const won =
    resultPerspective != null &&
    ((resultPerspective.role === 'WORD_WOLF' &&
      result?.winner === 'WOLF_SIDE') ||
      (resultPerspective.role === 'CITIZEN' &&
        result?.winner === 'CITIZEN_SIDE'))

  const rosterPlayers = useMemo(
    () =>
      (roomState?.players ?? []).map((p) =>
        toRosterPlayer(p, roomState?.hostUserId ?? '', meId),
      ),
    [roomState, meId],
  )

  const resultPlayers = useMemo(
    () =>
      (result?.players ?? []).map((player) => ({
        id: player.userId,
        name: player.displayName,
        avatarSrc: player.avatarUrl,
        initials: player.displayName.slice(0, 2),
        connected: true,
        ready: false,
        isCurrentUser: player.userId === resultPerspectiveId,
      })),
    [result, resultPerspectiveId],
  )
  const ballots = useMemo(
    () =>
      (result?.players ?? []).flatMap((player) =>
        Array.from({ length: player.votesReceived }, () => ({
          targetId: player.userId,
        })),
      ),
    [result],
  )
  const highestVoteCount = Math.max(
    0,
    ...(result?.players.map((player) => player.votesReceived) ?? []),
  )
  const leaders =
    result?.players.filter(
      (player) => player.votesReceived === highestVoteCount,
    ) ?? []
  const selectedResultPlayer =
    highestVoteCount > 0 && leaders.length === 1
      ? (resultPlayers.find((player) => player.id === leaders[0].userId) ??
        null)
      : null

  if (replaced) {
    return (
      <MobileShell title="接続が切り替わりました" darkHeader>
        <main className="px-5 py-10 text-center">
          <p className="text-sm">
            別のタブでこの部屋を開いたため、こちらの接続は切断されました。
          </p>
          <Button className="mt-6" size="sm" onClick={() => navigate('/home')}>
            ホームへ
          </Button>
        </main>
      </MobileShell>
    )
  }

  if (!roomId || status === 'connecting' || !roomState) {
    return (
      <MobileShell title="接続中" darkHeader>
        <main className="px-5 py-10 text-center text-sm">接続しています…</main>
      </MobileShell>
    )
  }

  const spectator = !roomState.players.some((player) => player.userId === meId)
  const spectatorAssignments =
    gameId && roomState.spectators.some((user) => user.userId === meId) ? (
      <SpectatorAssignments key={gameId} gameId={gameId} />
    ) : null
  const selectedPlayer = rosterPlayers.find((player) => player.id === selected)
  const showWord =
    !spectator && roomState.phase === 'DISCUSSION' && confirmedGameId !== gameId
  const chat = (
    <ChatThreadView
      messages={messages}
      status={status}
      sendError={sendError}
      send={send}
      user={user}
      players={roomState.players}
      readOnly={spectator || roomState.phase !== 'DISCUSSION'}
    />
  )
  const timer = (
    <GameTimer
      value={remaining(roomState.deadline)}
      deadline={deadlineMs(roomState.deadline)}
      label={
        roomState.phase === 'COMEBACK'
          ? '逆転回答の締切'
          : roomState.phase === 'VOTING'
            ? '投票締切'
            : '残り時間'
      }
    />
  )

  if (roomState.paused) {
    const disconnectedPlayers = rosterPlayers.filter(
      (player) => !player.connected,
    )
    return (
      <MobileShell
        title="ゲームを一時停止中"
        darkHeader
        actions={<GameExitButton roomId={roomId} spectator={spectator} />}
      >
        <main className="flex min-h-[calc(100dvh-4rem)] flex-col justify-center px-6 py-10 text-center">
          <h2 className="text-2xl font-semibold text-balance">
            {disconnectedPlayers.length > 0
              ? `${disconnectedPlayers.length}人の`
              : '参加者の'}
            再接続を待っています
          </h2>
          <ul
            className="mt-5 flex flex-wrap justify-center gap-4"
            aria-label="再接続待ちのプレイヤー"
          >
            {disconnectedPlayers.map((player) => (
              <li
                key={player.id}
                className="flex min-w-0 flex-col items-center gap-2"
              >
                <PlayerAvatar
                  className="size-12 border"
                  src={player.avatarSrc}
                  initials={player.initials}
                  name={player.name}
                />
                <span className="max-w-40 truncate text-sm font-medium">
                  {player.name}
                </span>
              </li>
            ))}
          </ul>
          <p className="text-muted-foreground mt-3 text-sm leading-6 text-pretty">
            ゲームとタイマーを停止しました。切断中の全員が期限内に戻ると再開します。
          </p>
          <div className="mx-auto mt-8 w-full max-w-xs rounded-xl border bg-white p-5">
            <GameTimer
              value={remaining(roomState.reconnectDeadline)}
              deadline={deadlineMs(roomState.reconnectDeadline)}
              label="再接続の残り時間"
            />
          </div>
          <div className="bg-muted text-muted-foreground mt-8 flex items-start gap-2 rounded-lg border p-3 text-left text-xs leading-5">
            <AlertTriangle
              className="mt-0.5 size-4 shrink-0"
              aria-hidden="true"
            />
            期限までに全員が戻らない場合、このゲームは勝敗なしで中止され、全員がロビーへ戻ります。
          </div>
        </main>
      </MobileShell>
    )
  }
  if (
    entryDeadline &&
    (roomState.phase === 'DEALING' || roomState.phase === 'DISCUSSION')
  ) {
    return (
      <GameCountdownPage endsAt={entryDeadline} onComplete={completeEntry} />
    )
  }
  // 表示上のフェーズ。結果を見ている間は、サーバーが LOBBY に戻っていても RESULT として描く
  const viewPhase = holdingResult ? 'RESULT' : roomState.phase
  const currentResultPlayer = resultPlayers.find(
    (player) => player.id === resultPerspectiveId,
  )
  if (
    viewPhase === 'RESULT' &&
    result &&
    currentResultPlayer &&
    !cinematicDone
  ) {
    return (
      <MobileShell title="開票" darkHeader>
        <ResultCinematic
          key={result.gameId}
          players={resultPlayers}
          votes={ballots}
          selectedPlayer={selectedResultPlayer}
          voteCount={highestVoteCount}
          currentPlayer={currentResultPlayer}
          won={won}
          onPerspectiveChange={me ? undefined : setPerspectiveUserId}
          onComplete={() => {
            setShowResult(true)
            try {
              sessionStorage.setItem(
                COMPLETED_RESULT_STORAGE_KEY,
                result.gameId,
              )
            } catch {
              // 保存できない場合も現在の画面では結果へ進める。
            }
          }}
        />
      </MobileShell>
    )
  }
  const mainClass =
    viewPhase === 'RESULT'
      ? 'result-screen'
      : showWord || roomState.phase === 'DEALING'
        ? 'word-reveal-screen px-5 py-6'
        : roomState.phase === 'DISCUSSION'
          ? 'discussion-board space-y-6 px-5 py-5'
          : 'space-y-5 px-5 py-5'
  return (
    <MobileShell
      title={showWord ? 'お題確認' : titleFor(viewPhase)}
      darkHeader
      backHref={viewPhase === 'RESULT' ? '/home' : undefined}
      actions={
        viewPhase === 'RESULT' ? undefined : (
          <div className="flex items-center gap-1">
            {timer}
            <GameExitButton roomId={roomId} spectator={spectator} />
          </div>
        )
      }
    >
      <main className={mainClass}>
        {(roomState.phase === 'DEALING' || showWord) && (
          <>
            <h2 className="mt-3 text-2xl font-semibold text-balance">
              お題をめくる
            </h2>
            {assignment ? (
              <div className="mt-7">
                <WordCard
                  word={assignment.word}
                  category={
                    genreLabels[assignment.category] ?? assignment.category
                  }
                  initiallyVisible={false}
                  onVisibilityChange={setRevealed}
                />
              </div>
            ) : (
              <p className="text-muted-foreground mt-4 text-sm">
                お題を配っています…
              </p>
            )}
            <div className="word-reveal-action">
              <Button
                size="sm"
                disabled={!revealed || roomState.phase !== 'DISCUSSION'}
                onClick={() => {
                  setConfirmedGameId(gameId)
                  if (gameId) writeConfirmedGameId(gameId)
                }}
              >
                議論へ
              </Button>
            </div>
          </>
        )}

        {roomState.phase === 'DISCUSSION' && !showWord && (
          <>
            <aside className="discussion-sidebar">
              {spectator
                ? spectatorAssignments
                : assignment && (
                    <WordCard
                      word={assignment.word}
                      category={
                        genreLabels[assignment.category] ?? assignment.category
                      }
                    />
                  )}

              <section className="discussion-players">
                <div className="flex items-center justify-between">
                  <h2 className="text-base font-semibold text-balance">
                    参加者
                  </h2>
                  <span className="text-muted-foreground inline-flex items-center gap-1.5 text-xs">
                    <Users className="size-4" aria-hidden="true" />
                    {roomState.players.length}人
                    {roomState.spectators.length > 0 &&
                      ` · 観戦 ${roomState.spectators.length}人`}
                  </span>
                </div>
                <div className="mt-3 grid grid-cols-3 gap-2">
                  {rosterPlayers.map((player) => (
                    <div
                      key={player.id}
                      className="min-w-0 rounded-lg border bg-white px-2 py-3 text-center"
                      data-disconnected={!player.connected || undefined}
                    >
                      <PlayerAvatar
                        className="mx-auto size-12"
                        src={player.avatarSrc}
                        initials={player.initials}
                        name={player.name}
                      />
                      <ParticipantProfileButton
                        userId={player.id}
                        displayName={player.name}
                        currentUserId={meId}
                      />
                    </div>
                  ))}
                </div>
              </section>
            </aside>

            {chat}

            {!spectator && (
              <div className="discussion-next space-y-2">
                <Button
                  className="h-11 w-full"
                  variant={skipAgreed ? 'outline' : 'default'}
                  aria-pressed={skipAgreed}
                  onClick={() => {
                    const next = !skipAgreed
                    setSkipAgreed(next)
                    setSkip(next)
                  }}
                >
                  {skipAgreed ? '投票へ進むのに賛成済み（取り消す）' : '投票へ'}
                </Button>
                {roomState.skipProgress && (
                  <p className="text-muted-foreground text-center text-xs">
                    {roomState.skipProgress.skippedCount} /{' '}
                    {roomState.skipProgress.playerCount}{' '}
                    人が賛成（全員でスキップ）
                  </p>
                )}
              </div>
            )}
          </>
        )}

        {roomState.phase === 'VOTING' && (
          <>
            {spectatorAssignments}
            {spectator ? (
              <section className="rounded-xl border bg-white p-4">
                <div className="flex items-center justify-between gap-3">
                  <div>
                    <Badge variant="outline">観戦者</Badge>
                    <h2 className="mt-2 font-semibold text-balance">
                      投票の完了を待っています
                    </h2>
                    <p className="text-muted-foreground mt-1 text-xs leading-5">
                      投票先と得票数は結果発表まで公開されません。
                    </p>
                  </div>
                  <p className="shrink-0 font-mono text-lg font-semibold tabular-nums">
                    {roomState.voteProgress?.votedCount ?? 0} /{' '}
                    {roomState.players.length}
                  </p>
                </div>
                <Progress
                  aria-label="投票完了率"
                  className="mt-4"
                  value={
                    (100 * (roomState.voteProgress?.votedCount ?? 0)) /
                    Math.max(1, roomState.players.length)
                  }
                />
              </section>
            ) : (
              <div className="border-primary/20 bg-primary/5 rounded-xl border p-4">
                <div className="flex items-start gap-3">
                  <Gavel
                    className="text-primary mt-0.5 size-5 shrink-0"
                    aria-hidden="true"
                  />
                  <div>
                    <h2 className="font-semibold text-balance">
                      ワードウルフだと思う人物へ投票
                    </h2>
                    <p className="mt-1 text-sm leading-6 text-pretty text-stone-600">
                      投票は1人1回で、確定後は変更できません。自分自身には投票できません。
                    </p>
                  </div>
                </div>
              </div>
            )}

            <PlayerRoster
              players={rosterPlayers}
              selectedId={selected}
              onSelect={
                spectator || voted || submittingVote ? undefined : setSelected
              }
            />

            {!spectator && (
              <div className="bg-background sticky bottom-0 -mx-5 border-t px-5 pt-3 pb-4">
                <p className="text-muted-foreground mb-2 truncate text-center text-xs">
                  {roomState.voteProgress
                    ? `現在 ${roomState.voteProgress.votedCount} / ${roomState.voteProgress.playerCount}人が投票済み`
                    : ''}
                </p>
                <AlertDialog
                  open={voteDialogOpen}
                  onOpenChange={setVoteDialogOpen}
                >
                  <AlertDialogTrigger
                    disabled={!selected || voted || submittingVote}
                    render={<Button className="h-11 w-full" />}
                  >
                    {voted
                      ? '投票しました'
                      : submittingVote
                        ? '送信中…'
                        : selectedPlayer
                          ? `${selectedPlayer.name}に投票する`
                          : '投票する人物を選択'}
                  </AlertDialogTrigger>
                  <AlertDialogContent>
                    <AlertDialogHeader>
                      <AlertDialogMedia>
                        <Gavel className="size-5" aria-hidden="true" />
                      </AlertDialogMedia>
                      <AlertDialogTitle>
                        {selectedPlayer?.name}で確定しますか？
                      </AlertDialogTitle>
                      <AlertDialogDescription>
                        この投票は送信後に変更できません。議論内容を確認してから確定してください。
                      </AlertDialogDescription>
                    </AlertDialogHeader>
                    <AlertDialogFooter>
                      <AlertDialogCancel>戻る</AlertDialogCancel>
                      <AlertDialogAction
                        onClick={() => {
                          setVoteDialogOpen(false)
                          void castVote()
                        }}
                      >
                        投票を確定
                      </AlertDialogAction>
                    </AlertDialogFooter>
                  </AlertDialogContent>
                </AlertDialog>
                {voteError && (
                  <p className="mt-2 text-center text-xs">
                    投票できませんでした（{voteError}）
                  </p>
                )}
              </div>
            )}

            {spectator && chat}
          </>
        )}

        {roomState.phase === 'COMEBACK' && spectatorAssignments}
        {roomState.phase === 'COMEBACK' && roomState.comeback && (
          <ComebackPanel
            key={roomState.gameId}
            comeback={roomState.comeback}
            players={roomState.players}
            meId={meId}
            updateComebackDraft={updateComebackDraft}
            submitComeback={submitComeback}
            sendError={sendError}
            status={status}
          />
        )}

        {viewPhase === 'RESULT' && result && (
          <>
            {resultPerspective && (
              <section
                className="result-personal"
                data-won={won}
                aria-label={
                  me
                    ? '自分の勝敗'
                    : `${resultPerspective.displayName}さんの視点の勝敗`
                }
              >
                {!me && <p>{resultPerspective.displayName}さんの視点</p>}
                <FactionMark
                  wolf={resultPerspective.role === 'WORD_WOLF'}
                  className="result-emblem"
                />
                <p className="result-faction">
                  {resultPerspective.role === 'WORD_WOLF'
                    ? 'ワードウルフ'
                    : '市民'}
                </p>
                <h1>{won ? 'VICTORY' : 'DEFEAT'}</h1>
                <p className="result-verdict">{won ? '勝利' : '敗北'}</p>
                <div className="result-self">
                  <PlayerAvatar
                    className="size-12"
                    src={resultPerspective.avatarUrl}
                    initials={resultPerspective.displayName.slice(0, 2)}
                    name={resultPerspective.displayName}
                  />
                  <strong>{resultPerspective.displayName}</strong>
                </div>
              </section>
            )}

            {result.comeback && (
              <section
                className="rounded-xl border p-5 text-center"
                aria-label="逆転回答の結果"
              >
                <h2 className="text-lg font-semibold">
                  {result.comeback.correct
                    ? '逆転成功！'
                    : result.comeback.timedOut
                      ? '回答時間終了'
                      : '逆転ならず'}
                </h2>
                <p className="mt-2 break-words">
                  {result.comeback.timedOut ? '未確定の入力' : 'ウルフの回答'}：
                  {result.comeback.answer || '（未入力）'}
                </p>
                <p className="text-muted-foreground mt-2 text-sm">
                  市民のお題：{result.words.majority.word}
                </p>
              </section>
            )}
            <section className="result-words" aria-label="陣営とお題">
              {([false, true] as const).map((wolf) => {
                const side = wolf
                  ? result.words.minority
                  : result.words.majority
                const winner = wolf === (result.winner === 'WOLF_SIDE')

                return (
                  <div key={String(wolf)} data-winner={winner}>
                    <FactionMark wolf={wolf} />
                    <div>
                      <p>
                        {wolf ? 'ワードウルフ' : '市民'} · {side.count}人
                      </p>
                      <strong>{side.word}</strong>
                    </div>
                    {winner && <span className="result-winner-tag">勝利</span>}
                  </div>
                )
              })}
            </section>

            <section
              className="result-ballots"
              aria-labelledby="revealed-votes-title"
            >
              <h2 id="revealed-votes-title">得票</h2>
              {/* 契約上「誰が誰に入れたか」は配信されないので、得票数だけを多い順に出す。
                  自分の投票先（myVote）は HTTP の getGameResult にしか無い */}
              <ul className="result-ballot-cards">
                {[...result.players]
                  .sort((a, b) => b.votesReceived - a.votesReceived)
                  .map((player, index) => (
                    <motion.li
                      initial={reducedMotion ? false : { opacity: 0, y: 10 }}
                      whileInView={{ opacity: 1, y: 0 }}
                      viewport={{ once: true, amount: 0.2 }}
                      transition={{
                        duration: 0.35,
                        delay: reducedMotion ? 0 : index * 0.09,
                        ease: 'easeOut',
                      }}
                      whileHover={{
                        backgroundColor:
                          player.userId === meId ? '#705332' : '#3a2b1d',
                        transition: { duration: 0.16, delay: 0 },
                      }}
                      className="result-ballot-card"
                      key={player.userId}
                      data-current-player={player.userId === meId || undefined}
                    >
                      <div className="ballot-card-voter">
                        <PlayerAvatar
                          className="size-10"
                          src={player.avatarUrl}
                          initials={player.displayName.slice(0, 2)}
                          name={player.displayName}
                        />
                        <div>
                          <ParticipantProfileButton
                            userId={player.userId}
                            displayName={player.displayName}
                            currentUserId={meId}
                          />
                          <span>
                            <FactionMark wolf={player.role === 'WORD_WOLF'} />
                            {player.role === 'WORD_WOLF'
                              ? 'ワードウルフ'
                              : '市民'}{' '}
                            · {player.word}
                          </span>
                        </div>
                      </div>
                      <div className="ballot-card-target">
                        <strong>{player.votesReceived}票</strong>
                      </div>
                    </motion.li>
                  ))}
              </ul>
            </section>

            <div className="result-actions">
              <Button
                onClick={() => {
                  clearHeldResult()
                  navigate(`/lobby?roomId=${encodeURIComponent(roomId ?? '')}`)
                }}
              >
                ロビーへ
              </Button>
              <Button
                variant="outline"
                onClick={() => {
                  clearHeldResult()
                  navigate('/home')
                }}
              >
                ホーム
              </Button>
            </div>
          </>
        )}

        {sendError && <p className="mt-4 text-xs">エラー: {sendError}</p>}
      </main>
    </MobileShell>
  )
}

/** 陣営の意匠。モック（src/mocks/pages/game-pages.tsx）と同じクラスを使う。 */
function FactionMark({
  wolf,
  className = '',
}: {
  wolf: boolean
  className?: string
}) {
  return wolf ? (
    <span
      className={`faction-mark faction-wolf ${className}`}
      role="img"
      aria-label="ワードウルフ"
    />
  ) : (
    <ShieldCheck
      className={`faction-mark ${className}`}
      role="img"
      aria-label="市民"
    />
  )
}

function titleFor(phase: RoomStateSnapshot['phase']): string {
  switch (phase) {
    case 'DEALING':
      return 'お題確認'
    case 'DISCUSSION':
      return '議論'
    case 'VOTING':
      return '投票'
    case 'COMEBACK':
      return '逆転モード'
    case 'RESULT':
      return '結果'
    default:
      return 'ゲーム'
  }
}

function deadlineMs(deadline: string | null): number | undefined {
  if (!deadline) return undefined
  const ms = new Date(deadline).getTime()
  return Number.isFinite(ms) ? ms : undefined
}

function GameExitButton({
  roomId,
  spectator,
}: {
  roomId?: string | null
  spectator?: boolean
}) {
  const [, navigate] = useLocation()
  const [isLeaving, setIsLeaving] = useState(false)
  const [open, setOpen] = useState(false)
  const [leaveError, setLeaveError] = useState('')

  async function handleLeave() {
    setIsLeaving(true)
    setLeaveError('')
    try {
      if (roomId) {
        const response = await api.POST('/rooms/{roomId}/leave', {
          params: { path: { roomId } },
        })
        if (response.error && response.response.status !== 404) {
          setLeaveError('退出できませんでした。もう一度お試しください。')
          return
        }
      }
      setOpen(false)
      navigate('/home')
    } catch {
      setLeaveError('通信エラーが発生しました。もう一度お試しください。')
    } finally {
      setIsLeaving(false)
    }
  }

  return (
    <AlertDialog open={open} onOpenChange={setOpen}>
      <AlertDialogTrigger
        aria-label={spectator ? '観戦をやめて退出' : 'ゲームから退出'}
        render={
          <Button
            className="text-white hover:bg-white/10 hover:text-white"
            size="icon-sm"
            variant="ghost"
          />
        }
      >
        <LogOut className="size-4" aria-hidden="true" />
      </AlertDialogTrigger>
      <AlertDialogContent>
        <AlertDialogHeader>
          <AlertDialogMedia>
            <LogOut className="size-5" aria-hidden="true" />
          </AlertDialogMedia>
          <AlertDialogTitle>
            {spectator
              ? '観戦をやめて退出しますか？'
              : 'ゲームから退出しますか？'}
          </AlertDialogTitle>
          <AlertDialogDescription>
            {spectator
              ? '観戦を終了してホーム画面へ戻ります。ゲームの進行には影響しません。'
              : '進行中に退出するとゲームが一時停止し、ほかの参加者が再接続期限まで待つことになります。'}
          </AlertDialogDescription>
        </AlertDialogHeader>
        {leaveError && (
          <p role="alert" className="text-destructive text-sm">
            {leaveError}
          </p>
        )}
        <AlertDialogFooter>
          <AlertDialogCancel disabled={isLeaving}>
            {spectator ? '観戦を続ける' : 'ゲームに戻る'}
          </AlertDialogCancel>
          <AlertDialogAction
            variant="destructive"
            disabled={isLeaving}
            onClick={(e) => {
              e.preventDefault()
              void handleLeave()
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
  )
}
