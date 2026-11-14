import { ChevronRight } from 'lucide-react'
import { Link } from 'wouter'

import { GameMark } from '@/components/game-ui/game-mark'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Badge } from '@/components/ui/badge'
import { mockScreenGroups } from '@/data/mock-data'

export function MockCatalogPage() {
  return (
    <MobileShell>
      <header className="border-b border-brass/30 bg-night px-5 pb-5 pt-6 text-white">
        <GameMark inverse />
        <Badge className="mt-5 border-white/20 bg-white/10 text-white" variant="outline">UI PROTOTYPE</Badge>
        <h1 className="mt-3 text-balance text-2xl font-semibold">画面モック一覧</h1>
        <p className="mt-2 text-pretty text-sm leading-6 text-stone-400">
          各画面は独立したURLで直接開けます。PCでもスマホ幅で表示されます。
        </p>
      </header>
      <main className="space-y-7 px-5 py-6">
        {mockScreenGroups.map((group) => (
          <section key={group.label}>
            <h2 className="text-balance text-sm font-semibold text-stone-700">{group.label}</h2>
            <div className="mt-3 overflow-hidden rounded-xl border bg-white">
              {group.screens.map((screen) => {
                const Icon = screen.icon
                return (
                  <Link
                    key={screen.href}
                    className="flex items-center gap-3 border-b p-4 outline-none last:border-b-0 hover:bg-muted focus-visible:bg-muted"
                    href={screen.href}
                  >
                    <span className="flex size-10 shrink-0 items-center justify-center rounded-lg bg-stone-100 text-stone-700"><Icon className="size-5" aria-hidden="true" /></span>
                    <div className="min-w-0 flex-1">
                      <h3 className="text-balance text-sm font-medium">{screen.title}</h3>
                      <p className="mt-0.5 truncate text-xs text-muted-foreground">{screen.description}</p>
                    </div>
                    <ChevronRight className="size-4 shrink-0 text-muted-foreground" aria-hidden="true" />
                  </Link>
                )
              })}
            </div>
          </section>
        ))}
      </main>
    </MobileShell>
  )
}
