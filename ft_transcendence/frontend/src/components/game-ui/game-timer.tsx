import { useEffect, useState } from 'react'

type GameTimerProps = { value: string; label?: string; urgent?: boolean; deadline?: number }

function readSeconds(value: string) {
  const [minutes, seconds] = value.split(':').map(Number)
  return Number.isFinite(minutes) && Number.isFinite(seconds)
    ? minutes * 60 + seconds
    : 0
}

export function GameTimer({
  value,
  label = '残り時間',
  urgent = false,
  deadline,
}: GameTimerProps) {
  const [remaining, setRemaining] = useState(() => readSeconds(value))
  useEffect(() => {
    // Fixture-only deadline. Production must use the server's phase deadline.
    const endsAt = deadline ?? Date.now() + readSeconds(value) * 1000
    const update = () =>
      setRemaining(Math.max(0, Math.ceil((endsAt - Date.now()) / 1000)))
    update()
    const interval = window.setInterval(update, 250)
    document.addEventListener('visibilitychange', update)
    return () => {
      window.clearInterval(interval)
      document.removeEventListener('visibilitychange', update)
    }
  }, [value, deadline])
  const urgentCount = remaining < 30
  const formatted = `${String(Math.floor(remaining / 60)).padStart(2, '0')}:${String(remaining % 60).padStart(2, '0')}`
  const ratio = Math.min(1, remaining / Math.max(1, readSeconds(value)))
  return (
    <div className="game-timer" role="timer" aria-label={`${label} ${remaining}秒`} data-urgent={urgent || urgentCount} data-finished={remaining === 0}>
      <svg className="timer-hourglass" viewBox="0 0 40 60" aria-hidden="true">
        <path d="M8 5h24M8 55h24M11 7c0 14 2 15 9 23-7 8-9 9-9 23M29 7c0 14-2 15-9 23 7 8 9 9 9 23" fill="none" stroke="currentColor" strokeWidth="2" />
        <path d="M13 11h14l-7 16z" fill="currentColor" style={{transform: `scaleY(${ratio})`, transformOrigin:'20px 27px'}} />
        <path d="M13 50h14l-7-16z" fill="currentColor" style={{transform: `scaleY(${1-ratio})`, transformOrigin:'20px 50px'}} />
      </svg>
      <div><span className="game-timer-label">{remaining === 0 ? '時間終了' : label}</span>
      <strong className="game-timer-value" aria-hidden="true"><span className="timer-digit" key={Math.ceil(remaining / 60)}>{formatted}</span></strong></div>
    </div>
  )
}
