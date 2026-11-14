import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import type { components } from '@/types/api'

export type RoomTiming = Pick<
  components['schemas']['RoomSettings'],
  'discussionMs' | 'voteMs'
>

export function validRoomTiming(value: RoomTiming) {
  return (
    Number.isInteger(value.discussionMs) &&
    value.discussionMs >= 30000 &&
    value.discussionMs <= 900000 &&
    Number.isInteger(value.voteMs) &&
    value.voteMs >= 10000 &&
    value.voteMs <= 180000
  )
}

export function formatRoomDuration(ms: number) {
  const seconds = Math.floor(ms / 1000)
  return `${Math.floor(seconds / 60)}:${String(seconds % 60).padStart(2, '0')}`
}

export function RoomTimingFields({
  value,
  onChange,
  disabled,
  idPrefix,
}: {
  value: RoomTiming
  onChange: (value: RoomTiming) => void
  disabled?: boolean
  idPrefix: string
}) {
  return (
    <fieldset disabled={disabled} className="space-y-3">
      <legend className="text-sm font-medium">時間設定</legend>
      <div className="grid grid-cols-2 gap-3">
        {(
          [
            ['discussionMs', '議論時間（秒）', 30, 900, '30秒〜15分'],
            ['voteMs', '投票時間（秒）', 10, 180, '10秒〜3分'],
          ] as const
        ).map(([key, label, min, max, hint]) => (
          <div key={key}>
            <Label htmlFor={`${idPrefix}-${key}`}>{label}</Label>
            <Input
              id={`${idPrefix}-${key}`}
              className="mt-2 h-11 bg-white tabular-nums"
              type="number"
              min={min}
              max={max}
              step={1}
              value={Number.isFinite(value[key]) ? value[key] / 1000 : ''}
              onChange={(event) =>
                onChange({
                  ...value,
                  [key]:
                    event.target.value === ''
                      ? NaN
                      : Number(event.target.value) * 1000,
                })
              }
              aria-describedby={`${idPrefix}-${key}-help`}
            />
            <p
              id={`${idPrefix}-${key}-help`}
              className="text-muted-foreground mt-1 text-xs"
            >
              {hint}
            </p>
          </div>
        ))}
      </div>
    </fieldset>
  )
}
