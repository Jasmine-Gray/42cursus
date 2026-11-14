import type { FormEventHandler, ReactNode } from 'react'
import { ArrowUpRight } from 'lucide-react'
import { MobileShell } from '@/components/layout/mobile-shell'
import { PlayMenu } from '@/components/game-ui/play-menu'
import { Button } from '@/components/ui/button'

type RoomMenuLayoutProps = {
  mode: 'join' | 'create'
  children: ReactNode
  preview: ReactNode
  actionLabel: string
  actionDisabled?: boolean
  secondaryAction?: ReactNode
  onSubmit: FormEventHandler<HTMLFormElement>
}

export function RoomMenuLayout({
  mode,
  children,
  preview,
  actionLabel,
  actionDisabled,
  secondaryAction,
  onSubmit,
}: RoomMenuLayoutProps) {
  return (
    <MobileShell
      title={mode === 'join' ? 'URLで参加' : '部屋を作る'}
      backHref="/home"
      bottomNav
    >
      <main className="room-menu">
        <PlayMenu />
        <form className="room-menu-form" onSubmit={onSubmit} noValidate>
          <div className="room-menu-fields">{children}</div>
          <aside className="room-menu-preview" aria-label="部屋情報">
            {preview}
          </aside>
          <div className="room-menu-actions">
            {secondaryAction}
            <Button
              className="hunt-play-action room-menu-confirm"
              data-tone="accent"
              type="submit"
              disabled={actionDisabled}
            >
              {actionLabel}
              <ArrowUpRight aria-hidden="true" />
            </Button>
          </div>
        </form>
      </main>
    </MobileShell>
  )
}
