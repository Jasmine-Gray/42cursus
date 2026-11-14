import {
  BriefcaseBusiness,
  Clapperboard,
  Gamepad2,
  MapPin,
  PackageOpen,
  Utensils,
} from 'lucide-react'
import type { Dispatch, SetStateAction } from 'react'
import { Checkbox } from '@/components/ui/checkbox'
import { cn } from '@/lib/utils'
import type { components } from '@/types/api'

type Genre = components['schemas']['RoomGenre']

export const genreOptions: { id: Genre; name: string; icon: typeof MapPin }[] =
  [
    { id: 'PLACE', name: '場所', icon: MapPin },
    { id: 'FOOD', name: '食べ物', icon: Utensils },
    { id: 'HOBBY', name: '趣味', icon: Gamepad2 },
    { id: 'DAILY_ITEM', name: '日用品', icon: PackageOpen },
    { id: 'WORK_SCHOOL', name: '学校・仕事', icon: BriefcaseBusiness },
    { id: 'ENTERTAINMENT', name: 'エンタメ', icon: Clapperboard },
  ]

export function RoomTopicFields({
  selectedGenres,
  onGenresChange,
  comebackEnabled,
  onComebackChange,
  genreError = '',
  onEdited,
  disabled = false,
  idPrefix,
  showComeback = true,
}: {
  selectedGenres: Genre[]
  onGenresChange: Dispatch<SetStateAction<Genre[]>>
  comebackEnabled: boolean
  onComebackChange: (enabled: boolean) => void
  genreError?: string
  onEdited?: () => void
  disabled?: boolean
  idPrefix: string
  showComeback?: boolean
}) {
  return (
    <>
      <fieldset
        disabled={disabled}
        aria-describedby={genreError ? `${idPrefix}-genre-error` : undefined}
      >
        <legend className="text-sm font-medium">
          お題ジャンル（複数選択可）
        </legend>
        <div className="mt-3 grid grid-cols-2 gap-2">
          {genreOptions.map((genre) => {
            const selected = selectedGenres.includes(genre.id)
            const Icon = genre.icon
            return (
              <label
                key={genre.id}
                className={cn(
                  'flex cursor-pointer items-center gap-2 rounded-lg border bg-white p-3 transition-colors',
                  selected && 'border-primary bg-primary/5',
                )}
              >
                <Checkbox
                  checked={selected}
                  onCheckedChange={(checked) => {
                    onGenresChange((current) =>
                      checked
                        ? [...current, genre.id]
                        : current.filter((item) => item !== genre.id),
                    )
                    onEdited?.()
                  }}
                  aria-label={`${genre.name}ジャンル`}
                />
                <span className="flex min-w-0 items-center gap-1.5 text-sm font-medium">
                  <Icon
                    className="text-primary size-4 shrink-0"
                    aria-hidden="true"
                  />
                  {genre.name}
                </span>
              </label>
            )
          })}
        </div>
        {genreError && (
          <p
            id={`${idPrefix}-genre-error`}
            className="text-destructive mt-2 text-xs"
            role="alert"
          >
            {genreError}
          </p>
        )}
      </fieldset>

      {showComeback && (
        <div className="rounded-xl border bg-white p-4">
          <label className="flex cursor-pointer items-center gap-3">
            <Checkbox
              disabled={disabled}
              checked={comebackEnabled}
              onCheckedChange={(checked) => onComebackChange(checked === true)}
              aria-describedby={`${idPrefix}-comeback-help`}
            />
            <span className="font-medium">逆転モード</span>
          </label>
          <p
            id={`${idPrefix}-comeback-help`}
            className="text-muted-foreground mt-3 text-sm leading-6"
          >
            見つかったウルフが30秒以内に市民のお題を当てたら逆転勝利。回答は1回、入力途中の文字も全員に見えます。
          </p>
        </div>
      )}
    </>
  )
}
