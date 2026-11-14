import { useRef, useState } from 'react'
import { motion, useMotionValue, useTransform, useReducedMotion, animate } from 'motion/react'
import { ArrowUpLeft, RotateCcw } from 'lucide-react'
import { Button } from '@/components/ui/button'

type WordCardProps = {
  word: string
  category: string
  initiallyVisible?: boolean
  onVisibilityChange?: (visible: boolean) => void
}

type Point = { x: number; y: number }

// Fold the lower-right corner across the perpendicular bisector of corner/pointer.
function foldedPaper(x: number, y: number) {
  const corner = { x: 400, y: 208 }
  const normal = { x: corner.x - x, y: corner.y - y }
  const midpoint = { x: (corner.x + x) / 2, y: (corner.y + y) / 2 }
  const length = normal.x ** 2 + normal.y ** 2
  const rectangle = [{ x: 0, y: 0 }, { x: 400, y: 0 }, corner, { x: 0, y: 208 }]
  const distance = (point: Point) => (point.x - midpoint.x) * normal.x + (point.y - midpoint.y) * normal.y
  function clip(keepCover: boolean) {
    const points: Point[] = []
    rectangle.forEach((point, index) => {
      const next = rectangle[(index + 1) % rectangle.length]
      const a = distance(point), b = distance(next)
      const inside = keepCover ? a <= 0 : a >= 0
      const nextInside = keepCover ? b <= 0 : b >= 0
      if (inside) points.push(point)
      if (inside !== nextInside) {
        const fraction = a / (a - b)
        points.push({ x: point.x + (next.x - point.x) * fraction, y: point.y + (next.y - point.y) * fraction })
      }
    })
    return points
  }
  const serialize = (points: Point[]) => points.map(point => `${point.x},${point.y}`).join(' ')
  if (length < 1) return { cover: serialize(rectangle), fold: '' }
  return {
    cover: serialize(clip(true)),
    fold: serialize(clip(false).map(point => {
      const amount = 2 * distance(point) / length
      return { x: point.x - amount * normal.x, y: point.y - amount * normal.y }
    })),
  }
}

export function WordCard({ word, category, initiallyVisible = true, onVisibilityChange }: WordCardProps) {
  const [visible, setVisible] = useState(initiallyVisible)
  const [dragging, setDragging] = useState(false)
  const x = useMotionValue(initiallyVisible ? -400 : 400)
  const y = useMotionValue(initiallyVisible ? -208 : 208)
  const cover = useTransform(() => foldedPaper(x.get(), y.get()).cover)
  const fold = useTransform(() => foldedPaper(x.get(), y.get()).fold)
  const sealOpacity = useTransform(x, [0, 300, 400], [0, 0, 1])
  const reducedMotion = useReducedMotion()
  const dragged = useRef(false)
  const start = useRef({ x: 0, y: 0 })

  function reveal(next: boolean) {
    setVisible(next)
    setDragging(false)
    onVisibilityChange?.(next)
    const transition = { duration: reducedMotion ? 0 : 0.18, ease: 'easeOut' as const }
    animate(x, next ? -400 : 400, transition)
    animate(y, next ? -208 : 208, transition)
  }

  return (
    <div className="word-flip-card word-corner-card" data-revealed={visible} data-dragging={dragging}>
      <div className="word-corner-content" aria-hidden={!visible}><span className="word-theme">テーマ：{category}</span><strong>{word}</strong></div>
      <svg className="word-corner-paper" viewBox="0 0 400 208" preserveAspectRatio="none" aria-hidden="true">
        <motion.polygon points={cover} fill="#352820" stroke="#b39358" strokeWidth="1.5" />
        <motion.polygon points={fold} fill="#bda77b" stroke="#e8d5ab" strokeWidth="1" />
      </svg>
      <motion.span className="word-card-seal word-corner-seal" style={{ opacity: sealOpacity }} aria-hidden="true" />
      <Button
        variant="ghost"
        className="word-flip-handle"
        aria-label={visible ? 'お題を伏せる' : 'お題をめくる'}
        aria-pressed={visible}
        onPointerDown={(event) => {
          x.stop(); y.stop()
          dragged.current = false
          start.current = { x: event.clientX, y: event.clientY }
          event.currentTarget.setPointerCapture(event.pointerId)
        }}
        onPointerMove={(event) => {
          if (!event.currentTarget.hasPointerCapture(event.pointerId) || visible) return
          if (Math.hypot(event.clientX - start.current.x, event.clientY - start.current.y) > 5) dragged.current = true
          if (!dragged.current) return
          const bounds = event.currentTarget.parentElement!.getBoundingClientRect()
          setDragging(true)
          x.set(Math.min(400, (event.clientX - bounds.left) / bounds.width * 400))
          y.set(Math.min(208, (event.clientY - bounds.top) / bounds.height * 208))
        }}
        onPointerUp={(event) => {
          event.currentTarget.releasePointerCapture(event.pointerId)
          if (dragged.current) reveal(Math.hypot((400 - x.get()) / 400, (208 - y.get()) / 208) > .45)
        }}
        onPointerCancel={() => { reveal(visible); dragged.current = true }}
        onClick={() => {
          if (!dragged.current) reveal(!visible)
          dragged.current = false
        }}
      >{visible ? <RotateCcw aria-hidden="true" /> : <ArrowUpLeft className="word-corner-arrow" aria-hidden="true" />}</Button>
    </div>
  )
}
