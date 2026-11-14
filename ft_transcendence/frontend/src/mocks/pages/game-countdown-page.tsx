import { useCallback, useState } from 'react'
import { useLocation } from 'wouter'

import { DoorCountdown } from '@/components/game-ui/door-countdown'
import { MobileShell } from '@/components/layout/mobile-shell'

type CountdownViewpoint = 'citizen' | 'wolf' | 'spectator'

export function GameCountdownPage({
  viewpoint,
}: {
  viewpoint: CountdownViewpoint
}) {
  const [, navigate] = useLocation()
  const destination =
    viewpoint === 'spectator' ? '/game/spectator' : '/game/reveal'
  const [endsAt] = useState(() => readStartAt() + 3000)
  const onComplete = useCallback(
    () => navigate(destination),
    [destination, navigate],
  )

  return (
    <MobileShell ambientScene={false}>
      <DoorCountdown endsAt={endsAt} onComplete={onComplete} />
    </MobileShell>
  )
}

function readStartAt() {
  const value = Number(
    new URLSearchParams(window.location.search).get('startsAt'),
  )
  return Number.isFinite(value) && value > 0 ? value : Date.now()
}
