import type { ReactNode } from 'react'

type SectionHeaderProps = {
  title: string
  meta?: ReactNode
}

export function SectionHeader({ title, meta }: SectionHeaderProps) {
  return (
    <div className="flex items-start justify-between gap-3">
      <h2 className="text-balance text-base font-semibold">{title}</h2>
      {meta && <div className="shrink-0">{meta}</div>}
    </div>
  )
}
