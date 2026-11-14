// 画面04 ホーム — レーンD。UI は _docs/mock/src/pages/home-pages.tsx の HomePage を移植し、
// ハードコード値を契約 GET /users/me（生成型 MeData）に差し替えたもの。
import { ChevronRight, Eye, Users } from 'lucide-react'
import { useEffect, useRef, useState } from 'react'
import { Link, useLocation } from 'wouter'

import { PlayMenu } from '@/components/game-ui/play-menu'
import { CurrentPlayerCard } from '@/components/game-ui/current-player-card'
import { SectionHeader } from '@/components/game-ui/section-header'
import { StatsStrip } from '@/components/game-ui/stats-strip'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Badge } from '@/components/ui/badge'
import { buttonVariants } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { api } from '@/lib/api-client'
import { cn } from '@/lib/utils'
import type { components } from '@/types/api'

type MeData = components['schemas']['MeData']
type RoomPreview = components['schemas']['RoomPreview']
type RoomStatus = components['schemas']['RoomStatus']
type UserStats = components['schemas']['UserStats']

const roomStatusLabel: Record<RoomStatus, string> = {
  WAITING: '募集中',
  IN_GAME: '進行中',
  CLOSED: '終了',
}

export function HomePage() {
  const [data, setData] = useState<MeData | null>(null)
  const [isLoading, setIsLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)
  const [, navigate] = useLocation()
  const timerRef = useRef<ReturnType<typeof setTimeout> | null>(null)

  useEffect(() => {
    let ignore = false
    let retryCount = 0

    const fetchMe = () => {
      api
        .GET('/users/me')
        .then((res) => {
          if (ignore) return

          if (res.error || !res.data) {
            if (res.response?.status === 401) {
              navigate('/login')
              return
            }
            if (
              retryCount < 3 &&
              (!res.response || res.response.status >= 500)
            ) {
              retryCount += 1
              timerRef.current = setTimeout(fetchMe, 2000)
              return
            }
            setError('データの取得に失敗しました')
            setIsLoading(false)
            return
          }

          setData(res.data.data)
          setIsLoading(false)
        })
        .catch(() => {
          if (ignore) return
          if (retryCount < 3) {
            retryCount += 1
            timerRef.current = setTimeout(fetchMe, 2000)
            return
          }
          setError('通信エラーが発生しました')
          setIsLoading(false)
        })
    }

    fetchMe()

    return () => {
      ignore = true
      if (timerRef.current) clearTimeout(timerRef.current)
    }
  }, [navigate])

  if (isLoading) {
    return (
      <MobileShell bottomNav>
        <main className="flex min-h-[50vh] items-center justify-center p-5">
          <p className="text-muted-foreground animate-pulse text-sm">
            読み込み中...
          </p>
        </main>
      </MobileShell>
    )
  }

  if (error || !data) {
    return (
      <MobileShell bottomNav>
        <main className="flex min-h-[50vh] flex-col items-center justify-center gap-4 p-5 text-center">
          <p className="text-destructive text-sm">
            {error ?? 'エラーが発生しました'}
          </p>
          <button
            type="button"
            className={cn(buttonVariants({ variant: 'outline', size: 'sm' }))}
            onClick={() => window.location.reload()}
          >
            再読み込み
          </button>
        </main>
      </MobileShell>
    )
  }

  return (
    <MobileShell bottomNav>
      <main className="case-home hunt-home space-y-6 px-5 py-6">
        <CurrentPlayerCard
          displayName={data.user.displayName}
          avatarUrl={data.user.avatarUrl}
        />
        {data.activeRoom && <ActiveRoomCard room={data.activeRoom} />}
        <PlayMenu showSpectate={false} />
        <RecentStatsSection stats={data.stats} friendCount={data.friendCount} />
      </main>
    </MobileShell>
  )
}

function ActiveRoomCard({ room }: { room: RoomPreview }) {
  return (
    <Card className="case-active-room border-brass/40 bg-night text-white shadow-none">
      <CardContent className="p-5">
        <div>
          <Badge
            className="border-white/20 bg-white/10 text-white"
            variant="outline"
          >
            {roomStatusLabel[room.status]}
          </Badge>
          <h2 className="mt-3 text-xl font-semibold text-balance">
            {room.roomName}
          </h2>
          <p className="mt-1 font-mono text-sm text-stone-400">
            {room.inviteCode} · {room.genres.length}ジャンル
          </p>
        </div>

        <div className="mt-5 flex items-center gap-4 pt-4 text-sm text-stone-300">
          <span className="inline-flex items-center gap-1.5 font-medium">
            <Users className="size-4 text-stone-400" aria-hidden="true" />
            {room.playerCount} / {room.maxPlayers}人
          </span>
          <span className="inline-flex items-center gap-1.5 text-stone-400">
            <Eye className="size-4" aria-hidden="true" />
            観戦 {room.spectatorCount}人
          </span>
        </div>

        <Link
          className={cn(
            buttonVariants(),
            'mt-5 h-11 w-full bg-white text-stone-950 hover:bg-stone-200',
          )}
          href={`${room.status === 'IN_GAME' ? '/game' : '/lobby'}?roomId=${encodeURIComponent(room.roomId)}`}
        >
          {room.status === 'IN_GAME' ? 'ゲーム画面へ戻る' : 'ロビーへ戻る'}
          <ChevronRight className="size-4" aria-hidden="true" />
        </Link>
      </CardContent>
    </Card>
  )
}

function RecentStatsSection({
  stats,
  friendCount,
}: {
  stats: UserStats
  friendCount: number
}) {
  // 勝率は契約上サーバーが持たない導出値。ここで計算する。
  const winRate =
    stats.gamesPlayed > 0
      ? `${Math.round((stats.wins / stats.gamesPlayed) * 100)}%`
      : '—'

  return (
    <section className="home-stats-section">
      <SectionHeader title="最近の戦績" />
      <div className="mt-3">
        <StatsStrip
          label="最近の戦績"
          large
          items={[
            { label: '対戦', value: String(stats.gamesPlayed) },
            { label: '勝率', value: winRate },
            { label: 'フレンド', value: String(friendCount) },
          ]}
        />
      </div>
    </section>
  )
}
