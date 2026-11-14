import { cn } from '@/lib/utils'

type StatItem = {
  label: string
  value: string
  mono?: boolean
}

type StatsStripProps = {
  items: [StatItem, StatItem, StatItem]
  label: string
  large?: boolean
}

export function StatsStrip({ items, label, large }: StatsStripProps) {
  return (
    <div
      className="game-stats grid grid-cols-3 gap-3 py-4 text-center"
      aria-label={label}
    >
      {items.map((item) => (
        <div key={item.label}>
          <p
            className={cn(
              'font-semibold tabular-nums',
              large && 'text-3xl',
              item.mono !== false && 'font-mono',
            )}
          >
            {item.value}
          </p>
          <p className="text-muted-foreground mt-2 text-xs">{item.label}</p>
        </div>
      ))}
    </div>
  )
}
