import { motion, useReducedMotion } from 'motion/react'
import { Check, Circle, Crown, WifiOff } from 'lucide-react'

import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { Button } from '@/components/ui/button'
import { Badge } from '@/components/ui/badge'
import { cn } from '@/lib/utils'
/**
 * 名簿の表示に要る最小限の形。契約の player（RoomStateSnapshot.players）からも
 * モックの Player からも渡せるよう、構造だけを定義してデータ源に依存させない。
 */
export type RosterPlayer = {
  id: string
  name: string
  initials: string
  connected: boolean
  ready: boolean
  isHost?: boolean
  isCurrentUser?: boolean
  avatarSrc: string
}

type PlayerRosterProps = {
  players: RosterPlayer[]
  selectedId?: string | null
  onSelect?: (playerId: string) => void
  onReadyChange?: () => void
  showReady?: boolean
  showRoles?: Record<string, string>
  showVotes?: Record<string, number>
}

export function PlayerRoster({
  players,
  selectedId,
  onSelect,
  showReady,
  onReadyChange,
  showRoles,
  showVotes,
}: PlayerRosterProps) {
  const reducedMotion = useReducedMotion()
  return (
    <div className="player-roster">
      {players.map((player, index) => {
        const selectable =
          Boolean(onSelect) && !player.isCurrentUser && player.connected
        const showMeta =
          !player.connected ||
          Boolean(showRoles?.[player.id]) ||
          showVotes?.[player.id] !== undefined
        const content = (
          <>
            <PlayerAvatar
              presentation={showReady ? "portrait" : "icon"}
              className="player-card-portrait size-16"
              src={player.avatarSrc}
              initials={player.initials}
              name={player.name}
            />
            <div className="player-card-identity min-w-0 flex-1">
              <div className="flex items-center gap-1.5">
                <p className="truncate text-sm font-medium">{player.name}</p>
                {player.isHost && (
                  <span className="inline-flex" title="ホスト">
                    <Crown
                      className="text-primary size-3.5"
                      aria-hidden="true"
                    />
                    <span className="sr-only">ホスト</span>
                  </span>
                )}
              </div>
              {showMeta && (
                <div className="text-muted-foreground mt-0.5 flex items-center gap-2 text-xs">
                  {!player.connected && (
                    <span className="inline-flex items-center gap-1">
                      <WifiOff className="size-3" aria-hidden="true" />
                      切断中
                    </span>
                  )}
                  {showRoles?.[player.id] && (
                    <span>{showRoles[player.id]}</span>
                  )}
                  {showVotes?.[player.id] !== undefined && (
                    <span className="font-mono tabular-nums">
                      {showVotes[player.id]}票
                    </span>
                  )}
                </div>
              )}
            </div>
            {showReady && (
              player.isCurrentUser && onReadyChange ? (
                <Button
                  size="sm"
                  variant={player.ready ? 'secondary' : 'outline'}
                  className="player-ready-state"
                  data-ready={player.ready}
                  aria-label={player.ready ? '準備完了を取り消す' : '準備完了にする'}
                  aria-pressed={player.ready}
                  onClick={onReadyChange}
                >
                  <motion.span className="ready-seal-icon" key={String(player.ready)} initial={reducedMotion ? false : { scale: 1.4, opacity: 0 }} animate={{ scale: 1, opacity: 1 }} transition={{ duration: .18, ease: 'easeOut' }}>{player.ready ? <Check className="size-4" aria-hidden="true" /> : <Circle className="size-4" aria-hidden="true" />}</motion.span>
                  {player.ready ? '準備完了' : '準備中'}
                </Button>
              ) : (
                <Badge className="player-ready-state" data-ready={player.ready} variant="outline">
                  <motion.span className="ready-seal-icon" key={String(player.ready)} initial={reducedMotion ? false : { scale: 1.4, opacity: 0 }} animate={{ scale: 1, opacity: 1 }} transition={{ duration: .18, ease: 'easeOut' }}>{player.ready ? <Check className="size-4" aria-hidden="true" /> : <Circle className="size-4" aria-hidden="true" />}</motion.span>
                  {player.ready ? '準備完了' : '準備中'}
                </Badge>
              )
            )}
            {selectedId === player.id && (
              <span
                className="bg-primary text-primary-foreground flex size-7 items-center justify-center rounded-full"
                aria-hidden="true"
              >
                <Check className="size-4" aria-hidden="true" />
              </span>
            )}
          </>
        )

        if (selectable) {
          return (
            <button
              key={player.id}
              type="button"
              className={cn(
                'focus-visible:ring-ring/50 flex w-full items-center gap-3 rounded-lg border bg-white p-3 text-left outline-none focus-visible:ring-3',
                selectedId === player.id
                  ? 'border-primary bg-primary/5'
                  : 'hover:bg-muted',
              )}
              aria-pressed={selectedId === player.id}
              onClick={() => onSelect?.(player.id)}
            >
              {content}
            </button>
          )
        }

        return (
          <motion.div
            key={player.id}
            initial={showReady && !reducedMotion ? { opacity: 0, y: 12 } : false}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: .5, delay: index * .08, ease: 'easeOut' }}
            data-current-player={player.isCurrentUser || undefined}
            className="flex items-center gap-3 rounded-lg border bg-white p-3"
          >
            {content}
          </motion.div>
        )
      })}
    </div>
  )
}
