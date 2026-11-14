import { useEffect, useState } from 'react'
import { Badge } from '@/components/ui/badge'
import { Button } from '@/components/ui/button'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { api } from '@/lib/api-client'
import type { components } from '@/types/api'

export function SpectatorAssignments({ gameId }: { gameId: string }) {
  const [data, setData] = useState<
    components['schemas']['SpectatorAssignments'] | null
  >(null)
  const [error, setError] = useState('')
  const [attempt, setAttempt] = useState(0)
  useEffect(() => {
    const controller = new AbortController()
    void (async () => {
      setError('')
      setData(null)
      try {
        const response = await api.GET(
          '/games/{gameId}/spectator-assignments',
          { params: { path: { gameId } }, signal: controller.signal },
        )
        if (controller.signal.aborted) return
        if (response.error || !response.data) {
          setError('お題一覧を読み込めませんでした。')
          return
        }
        setData(response.data.data)
      } catch {
        if (!controller.signal.aborted)
          setError('お題一覧を読み込めませんでした。')
      }
    })()
    return () => controller.abort()
  }, [gameId, attempt])

  return (
    <section
      aria-label="観戦者用のお題一覧"
      className="space-y-3 rounded-xl border bg-white p-4"
    >
      <div>
        <Badge variant="outline">観戦者限定</Badge>
        <h2 className="mt-2 text-base font-semibold">みんなのお題</h2>
      </div>
      {error ? (
        <div className="space-y-2">
          <p role="alert" className="text-destructive text-sm">
            {error}
          </p>
          <Button
            variant="outline"
            size="sm"
            onClick={() => setAttempt(attempt + 1)}
          >
            再読み込み
          </Button>
        </div>
      ) : !data || data.gameId !== gameId ? (
        <p role="status" className="text-muted-foreground text-sm">
          お題を読み込んでいます…
        </p>
      ) : (
        <ul className="divide-y">
          {data.players.map((player) => (
            <li key={player.userId} className="flex items-center gap-3 py-3">
              <PlayerAvatar
                className="size-10 shrink-0"
                src={player.avatarUrl}
                initials={player.displayName.slice(0, 2)}
                name={player.displayName}
              />
              <div className="min-w-0 flex-1">
                <p className="text-sm font-medium break-words">
                  {player.displayName}
                </p>
                <p className="mt-1 text-lg font-semibold break-words">
                  {player.word}
                </p>
              </div>
              <Badge
                variant={
                  player.role === 'WORD_WOLF' ? 'destructive' : 'outline'
                }
                className="shrink-0"
              >
                {player.role === 'WORD_WOLF' ? 'ウルフ' : '市民'}
              </Badge>
            </li>
          ))}
        </ul>
      )}
    </section>
  )
}
