// 画面23-24 プライバシー・利用規約 — 静的ページ（API不要）。
// 本文は content/{privacy,terms}.md に分離し、lib/legal-content.tsx の簡易パーサーで描画する。
import { Link } from 'wouter'
import type { ReactNode } from 'react'

import { MobileShell } from '@/components/layout/mobile-shell'
import { Separator } from '@/components/ui/separator'
import { renderLegalMarkdown } from '@/lib/legal-content'

import privacyMarkdown from '@/content/privacy.md?raw'
import termsMarkdown from '@/content/terms.md?raw'

export function PrivacyPage() {
  return (
    <LegalPage title="プライバシーポリシー" updated="2026年8月5日" backHref="/login">
      {renderLegalMarkdown(privacyMarkdown)}
    </LegalPage>
  )
}

export function TermsPage() {
  return (
    <LegalPage title="利用規約" updated="2026年8月5日" backHref="/login">
      {renderLegalMarkdown(termsMarkdown)}
    </LegalPage>
  )
}

function LegalPage({ title, updated, backHref, children }: { title: string; updated: string; backHref: string; children: ReactNode }) {
  return (
    <MobileShell title={title} backHref={backHref}>
      {/* 色はテーマのトークン（card / foreground / muted-foreground）で指定する。
          固定色（text-stone-700 等）は暗いテーマの背景に沈んで読めなくなる。
          main の幅は shell.css が決めるので、読みやすい行長は内側の div で制限する */}
      <main className="px-5 py-6">
        <div className="mx-auto w-full max-w-3xl">
          <p className="mb-3 text-xs text-muted-foreground">最終更新：{updated}</p>
          <div className="space-y-7 rounded-xl border bg-card p-5 text-sm leading-7 text-muted-foreground shadow-sm sm:p-8">
            {children}
            <Separator />
            <p className="text-xs text-muted-foreground">本文はft_transcendenceのゲーム仕様と運用方針を反映しています。</p>
          </div>
          <div className="mt-5 text-center text-sm">
            <Link className="font-medium text-primary underline underline-offset-4" href="/login">
              ログイン画面へ戻る
            </Link>
          </div>
        </div>
      </main>
    </MobileShell>
  )
}
