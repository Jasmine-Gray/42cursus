import { Check } from 'lucide-react'

import { avatarOptions } from '@/data/avatar-options'
import { cn } from '@/lib/utils'

type AvatarPickerProps = {
  value: string
  onChange: (src: string) => void
}

export function AvatarPicker({ value, onChange }: AvatarPickerProps) {
  const groups = [
    { id: 'person', label: '人物', options: avatarOptions.filter((avatar) => avatar.category === 'person') },
    { id: 'animal', label: '相棒', options: avatarOptions.filter((avatar) => avatar.category === 'animal') },
  ] as const

  return (
    <div className="avatar-picker space-y-5" aria-label="アバタープリセット">
      {groups.map((group) => (
        <section key={group.id} role="group" aria-labelledby={`avatar-group-${group.id}`}>
          <div className="mb-2 flex items-center justify-between">
            <h3 id={`avatar-group-${group.id}`} className="text-balance text-sm font-semibold">{group.label}</h3>
            <span className="font-mono text-xs tabular-nums text-muted-foreground">{group.options.length}種類</span>
          </div>
          <div className="avatar-options-grid">
            {group.options.map((avatar) => {
              const selected = avatar.src === value
              return (
                <button
                  key={avatar.id}
                  type="button"
                  className={cn(
                    'character-option',
                    selected ? 'border-primary' : 'border-transparent hover:border-border',
                  )}
                  aria-label={`${avatar.label}のアバターを選択`}
                  aria-pressed={selected}
                  onClick={() => onChange(avatar.src)}
                >
                  <img className="character-option-art" src={avatar.src} alt="" />
                  <span className="mt-1 block truncate px-1 pb-1 text-center text-xs font-medium">{avatar.label}</span>
                  {selected && (
                    <span className="absolute right-2 top-2 flex size-6 items-center justify-center rounded-full bg-primary text-primary-foreground" aria-hidden="true">
                      <Check className="size-3.5" />
                    </span>
                  )}
                </button>
              )
            })}
          </div>
        </section>
      ))}
    </div>
  )
}
