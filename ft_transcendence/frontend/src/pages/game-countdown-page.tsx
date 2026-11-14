import { DoorCountdown } from '@/components/game-ui/door-countdown'
import { MobileShell } from '@/components/layout/mobile-shell'

export function GameCountdownPage({
  endsAt,
  onComplete,
}: {
  endsAt: number
  onComplete: () => void
}) {
  return (
    <MobileShell ambientScene={false}>
      <DoorCountdown endsAt={endsAt} onComplete={onComplete} />
    </MobileShell>
  )
}
