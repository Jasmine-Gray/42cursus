import { useState } from 'react'
import { Input } from '@/components/ui/input'
import { formatRoomDuration } from '@/components/game-ui/room-timing-fields'

export const settingsSelectClassName =
  'border-input focus-visible:ring-ring h-11 w-36 rounded-lg border bg-white px-3 text-sm font-medium tabular-nums focus-visible:ring-2 disabled:opacity-50'

export function RoomDurationSelect({
  id,
  label,
  min,
  max,
  step,
  value,
  onChange,
  disabled,
}: {
  id: string
  label: string
  min: number
  max: number
  step: number
  value: number
  onChange: (milliseconds: number) => void
  disabled: boolean
}) {
  const [custom, setCustom] = useState(
    value / 1000 < min ||
      value / 1000 > max ||
      (value / 1000 - min) % step !== 0,
  )
  return (
    <div className="w-36 space-y-2">
      <select
        id={id}
        className={settingsSelectClassName}
        disabled={disabled}
        value={custom ? 'custom' : value}
        onChange={(event) => {
          const nextCustom = event.target.value === 'custom'
          setCustom(nextCustom)
          if (!nextCustom) onChange(Number(event.target.value))
        }}
      >
        {Array.from(
          { length: (max - min) / step + 1 },
          (_, index) => (min + index * step) * 1000,
        ).map((ms) => (
          <option key={ms} value={ms}>
            {formatRoomDuration(ms)}
          </option>
        ))}
        <option value="custom">秒数を指定</option>
      </select>
      {custom && (
        <>
          <Input
            type="number"
            min={min}
            max={max}
            step={1}
            disabled={disabled}
            aria-label={`${label}（秒）`}
            aria-describedby={`${id}-help`}
            className="h-11 tabular-nums"
            value={Number.isFinite(value) ? value / 1000 : ''}
            onChange={(event) =>
              onChange(
                event.target.value === ''
                  ? NaN
                  : Number(event.target.value) * 1000,
              )
            }
          />
          <p id={`${id}-help`} className="text-muted-foreground text-xs">
            {min}〜{max}秒
          </p>
        </>
      )}
    </div>
  )
}
