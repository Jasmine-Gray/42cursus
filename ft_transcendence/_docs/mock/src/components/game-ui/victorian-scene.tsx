import { FlameFrames } from '@/components/game-ui/flame-frames'

export function VictorianScene({ animated = true }: { animated?: boolean }) {
  return <svg className="victorian-scene" viewBox="0 0 1672 941" preserveAspectRatio="xMidYMid slice" aria-hidden="true" data-animated={animated}>
    <image href="/scenes/victorian-alley.png" width="1672" height="941" />
    {[0, 1].map(layer => <g key={layer} className="street-travel-layer" data-layer={layer}>
      <image href="/scenes/victorian-alley.png" width="1672" height="941" />
      {animated && <><FlameFrames lamp="near" /><FlameFrames lamp="middle" /><FlameFrames lamp="far" /></>}
    </g>)}
  </svg>
}
