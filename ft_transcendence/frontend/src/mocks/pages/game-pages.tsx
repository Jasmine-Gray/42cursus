import { ResultCinematic } from '@/components/game-ui/result-cinematic'
import { AlertTriangle, Gavel, LogOut, ShieldCheck, Users } from 'lucide-react'
import { useState } from 'react'
import { motion, useReducedMotion } from 'motion/react'
import { Link, useLocation } from 'wouter'

import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { MobileShell } from '@/components/layout/mobile-shell'
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
import { Badge } from '@/components/ui/badge'
import { Button, buttonVariants } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { Progress } from '@/components/ui/progress'
import { cn } from '@/lib/utils'
import { ChatThread } from '@/mocks/components/game-ui/chat-thread'
import { GameTimer } from '@/components/game-ui/game-timer'
import { PlayerRoster } from '@/components/game-ui/player-roster'
import { WordCard } from '@/components/game-ui/word-card'
import {
  chatMessages,
  players,
  resultRoles,
  spectatorPlayers,
  voteResults,
  revealedVotes,
} from '@/mocks/data/mock-data'

type DiscussionViewpoint = 'citizen' | 'wolf' | 'spectator'

function discussionDeadline() {
  const saved = Number(sessionStorage.getItem('mock-discussion-deadline'))
  if (saved > 0) return saved
  const deadline = Date.now() + 300_000
  sessionStorage.setItem('mock-discussion-deadline', String(deadline))
  return deadline
}

export function WordRevealPage({
  viewpoint,
}: {
  viewpoint: Exclude<DiscussionViewpoint, 'spectator'>
}) {
  const [revealed, setRevealed] = useState(false)
  const [deadline] = useState(discussionDeadline)
  const word = viewpoint === 'wolf' ? '書店' : '図書館'

  return (
    <MobileShell title="お題確認" backHref="/lobby" darkHeader actions={<GameTimer value="05:00" deadline={deadline} />}>
      <main className="word-reveal-screen px-5 py-6">
        <div>
          <h2 className="mt-3 text-2xl font-semibold text-balance">
            お題をめくる
          </h2>
        </div>

        <div className="mt-7">
          <WordCard
            word={word}
            category="場所"
            initiallyVisible={false}
            onVisibilityChange={setRevealed}
          />
        </div>

        <div className="word-reveal-action">
          {revealed ? <Link className={buttonVariants({size: 'sm'})} href="/mock/game">議論へ</Link> : <Button size="sm" disabled>議論へ</Button>}
        </div>
      </main>
    </MobileShell>
  )
}

export function GameDiscussionPage({
  viewpoint,
}: {
  viewpoint: DiscussionViewpoint
}) {
  const [deadline] = useState(discussionDeadline)
  const spectator = viewpoint === 'spectator'
  const word = viewpoint === 'wolf' ? '書店' : '図書館'
  const visiblePlayers = spectator ? spectatorPlayers : players
  const visibleMessages = chatMessages

  return (
    <MobileShell
      title="議論"
      darkHeader
      actions={
        <div className="flex items-center gap-1">
          <GameTimer value="05:00" deadline={deadline} />
          <GameExitButton />
        </div>
      }
    >
      <main className="discussion-board space-y-6 px-5 py-5">
        <aside className="discussion-sidebar">
          {spectator ? (
            <Card className="border-stone-300 bg-stone-100 shadow-none">
              <CardContent className="flex items-start gap-3 p-4">
                <div>
                  <Badge variant="outline">観戦者</Badge>
                  <h2 className="mt-2 font-semibold text-balance">
                    秘密情報は表示されません
                  </h2>
                  <p className="text-muted-foreground mt-1 text-sm leading-6 text-pretty">
                    参加者、公開チャット、フェーズ、残り時間だけを閲覧できます。
                  </p>
                </div>
              </CardContent>
            </Card>
          ) : (
            <WordCard word={word} category="場所" />
          )}

          <section className="discussion-players">
            <div className="flex items-center justify-between">
              <h2 className="text-base font-semibold text-balance">参加者</h2>
              <span className="text-muted-foreground inline-flex items-center gap-1.5 text-xs">
                <Users className="size-4" aria-hidden="true" />
                6人{spectator ? ' · 観戦 2人' : ''}
              </span>
            </div>
            <div className="mt-3 grid grid-cols-3 gap-2">
              {visiblePlayers.map((player) => (
                <div
                  key={player.id}
                  className="min-w-0 rounded-lg border bg-white px-2 py-3 text-center"
                >
                  <PlayerAvatar
                    className="mx-auto size-12"
                    src={player.avatarSrc}
                    initials={player.initials}
                    name={player.name}
                  />
                  <p className="mt-2 truncate text-xs font-medium">
                    {player.name}
                  </p>
                </div>
              ))}
            </div>
          </section>
        </aside>
        <ChatThread initialMessages={visibleMessages} readOnly={spectator} />

        <div className="discussion-next">
          <Link
            className={cn(
              buttonVariants({ variant: spectator ? 'outline' : 'default' }),
              'h-11 w-full',
            )}
            href={spectator ? '/game/spectator/vote' : '/game/vote'}
          >
            {spectator ? '投票を観戦' : '投票へ'}
          </Link>
        </div>
      </main>
    </MobileShell>
  )
}

export function PausedGamePage() {
  const disconnectedPlayer =
    players.find((player) => player.id === 'kei') ?? players[0]

  return (
    <MobileShell
      title="ゲームを一時停止中"
      darkHeader
      actions={<GameExitButton />}
    >
      <main className="flex min-h-[calc(100dvh-4rem)] flex-col justify-center px-6 py-10 text-center">
        <PlayerAvatar
          className="mx-auto size-16 border-2 border-white shadow-sm"
          src={disconnectedPlayer.avatarSrc}
          initials={disconnectedPlayer.initials}
          name={disconnectedPlayer.name}
        />
        <h2 className="mt-5 text-2xl font-semibold text-balance">
          ケイさんの再接続を待っています
        </h2>
        <p className="text-muted-foreground mt-3 text-sm leading-6 text-pretty">
          議論とタイマーを停止しました。20秒以内に復帰すると同じ状態から再開します。
        </p>

        <div className="mx-auto mt-8 w-full max-w-xs rounded-xl border bg-white p-5">
          <p className="text-muted-foreground text-xs">再接続の残り時間</p>
          <p className="mt-1 font-mono text-4xl font-semibold tabular-nums">
            00:14
          </p>
          <Progress aria-label="再接続猶予の残り" className="mt-4" value={70} />
        </div>

        <div className="bg-muted text-muted-foreground mt-8 flex items-start gap-2 rounded-lg border p-3 text-left text-xs leading-5">
          <AlertTriangle
            className="mt-0.5 size-4 shrink-0"
            aria-hidden="true"
          />
          期限までに復帰しない場合、このゲームは勝敗なしで中止され、全員がロビーへ戻ります。
        </div>
      </main>
    </MobileShell>
  )
}

export function VotePage() {
  const [selectedId, setSelectedId] = useState<string | null>(null)
  const [, navigate] = useLocation()
  const selectedPlayer = players.find((player) => player.id === selectedId)

  return (
    <MobileShell
      title="投票"
      darkHeader
      actions={
        <div className="flex items-center gap-1">
          <GameTimer value="00:24" label="投票締切" urgent />
          <GameExitButton />
        </div>
      }
    >
      <main className="space-y-5 px-5 py-5">
        <div className="border-primary/20 bg-primary/5 rounded-xl border p-4">
          <div className="flex items-start gap-3">
            <Gavel
              className="text-primary mt-0.5 size-5 shrink-0"
              aria-hidden="true"
            />
            <div>
              <h2 className="font-semibold text-balance">
                人狼だと思う人物へ投票
              </h2>
              <p className="mt-1 text-sm leading-6 text-pretty text-stone-600">
                投票は1人1回で、確定後は変更できません。自分自身には投票できません。
              </p>
            </div>
          </div>
        </div>

        <PlayerRoster
          players={players}
          selectedId={selectedId}
          onSelect={setSelectedId}
        />

        <div className="bg-background sticky bottom-0 -mx-5 border-t px-5 pt-3 pb-4">
          <p className="text-muted-foreground mb-2 truncate text-center text-xs">
            {selectedPlayer
              ? `選択中：${selectedPlayer.name}`
              : '現在 4 / 6人が投票済み'}
          </p>
          <AlertDialog>
            <AlertDialogTrigger
              disabled={!selectedId}
              render={<Button className="h-11 w-full" />}
            >
              {selectedPlayer
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
                <AlertDialogAction onClick={() => navigate('/mock/game/result')}>
                  投票を確定
                </AlertDialogAction>
              </AlertDialogFooter>
            </AlertDialogContent>
          </AlertDialog>
        </div>
      </main>
    </MobileShell>
  )
}

export function SpectatorVotePage() {
  return (
    <MobileShell
      title="投票を観戦中"
      darkHeader
      actions={
        <div className="flex items-center gap-1">
          <GameTimer value="00:24" label="投票締切" urgent />
          <GameExitButton />
        </div>
      }
    >
      <main className="space-y-6 px-5 py-5">
        <section className="rounded-xl border bg-white p-4">
          <div className="flex items-center justify-between gap-3">
            <div>
              <Badge variant="outline">観戦者</Badge>
              <h2 className="mt-2 font-semibold text-balance">
                投票の完了を待っています
              </h2>
              <p className="text-muted-foreground mt-1 text-xs leading-5 text-pretty">
                投票先と得票数は結果発表まで公開されません。
              </p>
            </div>
            <p className="shrink-0 font-mono text-lg font-semibold tabular-nums">
              4 / 6
            </p>
          </div>
          <Progress aria-label="投票完了率" className="mt-4" value={67} />
        </section>

        <section>
          <div className="flex items-center justify-between">
            <h2 className="text-base font-semibold text-balance">
              参加プレイヤー
            </h2>
            <span className="text-muted-foreground text-xs">観戦 2人</span>
          </div>
          <div className="mt-3">
            <PlayerRoster players={spectatorPlayers} />
          </div>
        </section>

        <ChatThread initialMessages={chatMessages} readOnly />

        <Link
          className={cn(buttonVariants({ variant: 'outline' }), 'h-11 w-full')}
          href="/mock/game/result"
        >
          結果を見る
        </Link>
      </main>
    </MobileShell>
  )
}

function FactionMark({ wolf, className = '' }: { wolf: boolean; className?: string }) {
  return wolf
    ? <span className={`faction-mark faction-wolf ${className}`} role="img" aria-label="人狼" />
    : <ShieldCheck className={`faction-mark ${className}`} role="img" aria-label="市民" />
}

export function ResultPage() {
  const reducedMotion = useReducedMotion()
  const [showResult, setShowResult] = useState(false)
  const wordWolf = players.find(player => player.id === 'minato') ?? players[0]
  const currentPlayer = players.find(player => player.isCurrentUser) ?? players[0]
  const currentIsWolf = resultRoles.find(result => result.playerId === currentPlayer.id)?.role === '人狼'
  const mostVoted = players.reduce((leading, player) => voteResults[player.id] > voteResults[leading.id] ? player : leading)
  const wolfWon = mostVoted.id !== wordWolf.id
  const won = currentIsWolf === wolfWon

  if (!showResult) {
    return (
      <MobileShell title="開票" darkHeader>
        <ResultCinematic players={players} votes={revealedVotes} selectedPlayer={mostVoted} voteCount={voteResults[mostVoted.id]} currentPlayer={currentPlayer} won={won} onComplete={() => setShowResult(true)} />
      </MobileShell>
    )
  }

  return (
    <MobileShell title="結果" backHref="/home" darkHeader>
      <main className="result-screen">
        <section className="result-personal" data-won={won} aria-label="自分の勝敗">
          <FactionMark wolf={currentIsWolf} className="result-emblem" />
          <p className="result-faction">{currentIsWolf ? '人狼' : '市民'}</p>
          <h1>{won ? 'VICTORY' : 'DEFEAT'}</h1>
          <p className="result-verdict">{won ? '勝利' : '敗北'}</p>
          <div className="result-self">
            <PlayerAvatar className="size-12" src={currentPlayer.avatarSrc} initials={currentPlayer.initials} name={currentPlayer.name} />
            <strong>{currentPlayer.name}</strong>
          </div>
        </section>

        <section className="result-words" aria-label="陣営とお題">
          {[false, true].map(wolf => (
            <div key={String(wolf)} data-winner={wolf === wolfWon}>
              <FactionMark wolf={wolf} />
              <div><p>{wolf ? '人狼' : '市民'} · {wolf ? '1人' : '5人'}</p><strong>{wolf ? '書店' : '図書館'}</strong></div>
              {wolf === wolfWon && <span className="result-winner-tag">勝利</span>}
            </div>
          ))}
        </section>

        <section className="result-ballots" aria-labelledby="revealed-votes-title">
          <h2 id="revealed-votes-title">投票結果</h2>
          <ul className="result-ballot-cards">
            {revealedVotes.map((vote, index) => {
              const voter = players.find(player => player.id === vote.voterId)!
              const target = players.find(player => player.id === vote.targetId)!
              const wolf = resultRoles.find(result => result.playerId === voter.id)?.role === '人狼'
              return (
                <motion.li
                  className="result-ballot-card"
                  key={voter.id}
                  data-current-player={voter.isCurrentUser || undefined}
                  initial={reducedMotion ? false : { opacity: 0, y: 10 }}
                  whileInView={{ opacity: 1, y: 0 }}
                  viewport={{ once: true, amount: .2 }}
                  transition={{ duration: .35, delay: reducedMotion ? 0 : index * .09, ease: 'easeOut' }}
                  whileHover={{ backgroundColor: voter.isCurrentUser ? '#705332' : '#3a2b1d', transition: { duration: .16, delay: 0 } }}
                >
                  <div className="ballot-card-voter">
                    <PlayerAvatar className="size-10" src={voter.avatarSrc} initials={voter.initials} name={voter.name} />
                    <div><strong>{voter.name}</strong><span><FactionMark wolf={wolf} />{wolf ? '人狼' : '市民'} · 得票 {voteResults[voter.id]}票</span></div>
                  </div>
                  <div className="ballot-card-target">
                    <span className="vote-direction" aria-hidden="true">→</span><span>投票先</span>
                    <PlayerAvatar className="size-7" src={target.avatarSrc} initials={target.initials} name={target.name} />
                    <strong>{target.name}</strong>
                  </div>
                </motion.li>
              )
            })}
          </ul>
        </section>

        <div className="result-actions">
          <Link className={buttonVariants()} href="/lobby">再戦</Link>
          <Link className={buttonVariants({ variant: 'outline' })} href="/home">ホーム</Link>
          <span>4 / 6人が再戦を希望</span>
        </div>
      </main>
    </MobileShell>
  )
}

function GameExitButton() {
  const [, navigate] = useLocation()

  return (
    <AlertDialog>
      <AlertDialogTrigger
        aria-label="ゲームから退出"
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
          <AlertDialogTitle>ゲームから退出しますか？</AlertDialogTitle>
          <AlertDialogDescription>
            進行中に退出するとゲームが一時停止し、ほかの参加者が再接続期限まで待つことになります。
          </AlertDialogDescription>
        </AlertDialogHeader>
        <AlertDialogFooter>
          <AlertDialogCancel>ゲームに戻る</AlertDialogCancel>
          <AlertDialogAction
            variant="destructive"
            onClick={() => navigate('/home')}
          >
            退出する
          </AlertDialogAction>
        </AlertDialogFooter>
      </AlertDialogContent>
    </AlertDialog>
  )
}
