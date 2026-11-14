import { useEffect, useRef, useState } from 'react'
import {
  motion,
  useAnimationFrame,
  useMotionValue,
  useReducedMotion,
  useTransform,
} from 'motion/react'

type DoorCountdownProps = { endsAt: number; onComplete: () => void }

function timeRemaining(endsAt: number) {
  return Math.max(0, endsAt - Date.now())
}

export function DoorCountdown({ endsAt, onComplete }: DoorCountdownProps) {
  const [remaining, setRemaining] = useState(() => timeRemaining(endsAt))
  const [failed, setFailed] = useState(false)
  const video = useRef<HTMLVideoElement>(null)
  const completedAt = useRef<number | null>(null)
  const reducedMotion = useReducedMotion()
  const count = Math.max(1, Math.min(3, Math.ceil(remaining / 1000)))
  const progress = useMotionValue(1 - Math.min(1, timeRemaining(endsAt) / 3000))
  const darkness = useTransform(progress, [0.78, 0.87], [0, 1])

  useAnimationFrame(() => {
    if (!reducedMotion)
      progress.set(1 - Math.min(1, timeRemaining(endsAt) / 3000))
  })

  useEffect(() => {
    const current = video.current
    let disposed = false
    let playRequested = false

    function update() {
      const next = timeRemaining(endsAt)
      setRemaining(next)
      if (next === 0 && completedAt.current !== endsAt) {
        completedAt.current = endsAt
        onComplete()
      }
      if (!current) return
      if (current.error) {
        setFailed(true)
        return
      }
      if (document.hidden || next === 0 || next > 3000) {
        current.pause()
        playRequested = false
        return
      }
      if (current.readyState < 1) return
      // A late download or restored tab joins the shared clock, never restarts it.
      const elapsed = Math.min(
        3 - next / 1000,
        Math.max(0, current.duration - 0.02),
      )
      if (!current.seeking && Math.abs(current.currentTime - elapsed) > 0.2) {
        current.currentTime = elapsed
      }
      if (!playRequested) {
        playRequested = true
        current.play()?.catch(() => {
          if (!disposed) setFailed(true)
        })
      }
    }

    update()
    const interval = window.setInterval(update, 50)
    current?.addEventListener('loadedmetadata', update)
    document.addEventListener('visibilitychange', update)
    return () => {
      disposed = true
      window.clearInterval(interval)
      current?.removeEventListener('loadedmetadata', update)
      document.removeEventListener('visibilitychange', update)
      current?.pause()
    }
  }, [endsAt, onComplete, reducedMotion, failed])

  return (
    <main className="door-stage game-entry" data-count={count}>
      <div className="entry-film" aria-hidden="true">
        {reducedMotion || failed ? (
          <img
            className="entry-video"
            src="/cinematics/entrance-poster.jpg"
            alt=""
          />
        ) : (
          <video
            ref={video}
            className="entry-video"
            src="/cinematics/entrance.mp4"
            poster="/cinematics/entrance-poster.jpg"
            muted
            playsInline
            preload="auto"
            onError={() => setFailed(true)}
          />
        )}
      </div>
      <div className="entry-copy">
        <div
          className="entry-count"
          role="timer"
          aria-live="assertive"
          aria-atomic="true"
          aria-label={`ゲーム開始まで${count}秒`}
        >
          <motion.span
            key={count}
            className="entry-number"
            aria-hidden="true"
            initial={false}
            animate={reducedMotion ? { opacity: 1 } : { opacity: [0, 1, 1, 0] }}
            transition={{
              duration: 1,
              times: [0, 0.12, 0.8, 1],
              ease: 'easeOut',
            }}
          >
            {count}
          </motion.span>
        </div>
      </div>
      <motion.div
        className="entry-blackout"
        aria-hidden="true"
        style={{
          opacity: reducedMotion ? (remaining <= 400 ? 1 : 0) : darkness,
        }}
      />
    </main>
  )
}
