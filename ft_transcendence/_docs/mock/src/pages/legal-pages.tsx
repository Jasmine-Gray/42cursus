import { Link } from 'wouter'
import type { ReactNode } from 'react'

import { MobileShell } from '@/components/layout/mobile-shell'
import { Separator } from '@/components/ui/separator'

export function PrivacyPage() {
  return (
    <LegalPage title="プライバシーポリシー" updated="2026年8月5日" backHref="/login">
      <LegalSection title="1. 取得する情報">
        <p>本サービスは、アカウント登録時にメールアドレス、パスワードのハッシュ、表示名を取得します。プロフィール機能では、任意でアバター画像と自己紹介を保存します。</p>
        <p>ゲーム利用時には、参加した部屋、役割、投票、勝敗、接続状態、公開チャットの内容と送信時刻を記録する場合があります。</p>
      </LegalSection>
      <LegalSection title="2. 利用目的">
        <ul>
          <li>本人確認、ログイン、アカウント管理</li>
          <li>リアルタイム対戦、再接続、観戦機能の提供</li>
          <li>対戦履歴と統計の表示、不正利用の防止</li>
          <li>障害調査、セキュリティ対策、品質改善</li>
        </ul>
      </LegalSection>
      <LegalSection title="3. 公開範囲と秘密情報">
        <p>表示名、アバター、オンライン状態、ゲーム中の公開チャットは、同じ部屋の参加者および観戦者に表示されます。ゲーム中のお題は本人だけに配信し、権限のない利用者には送信しません。</p>
      </LegalSection>
      <LegalSection title="4. 保存期間と削除">
        <p>アカウント情報は退会まで、ゲーム履歴は最終利用から2年間、公開チャットはゲーム終了から30日間、セキュリティログは90日間保持した後、安全に削除します。利用者はプロフィール画面から情報の訂正とアカウント削除を行えます。</p>
      </LegalSection>
      <LegalSection title="5. セキュリティ">
        <p>通信はHTTPS/WSSで暗号化し、パスワードはソルト付きハッシュとして保存します。認証CookieにはHttpOnly、Secure、SameSite属性を設定し、秘密情報をブラウザのLocal Storageへ保存しません。</p>
      </LegalSection>
      <LegalSection title="6. お問い合わせ">
        <p>個人情報に関する請求や質問は、プロジェクトの <a className="font-medium text-primary underline underline-offset-4" href="https://github.com/otnk666/ft_transcendence/issues">GitHub Issues</a> で受け付けます。公開したくない情報は記載せず、本人確認が必要な請求は登録メールを使って個別に案内します。</p>
      </LegalSection>
    </LegalPage>
  )
}

export function TermsPage() {
  return (
    <LegalPage title="利用規約" updated="2026年8月5日" backHref="/login">
      <LegalSection title="1. サービスの内容">
        <p>本サービスは、複数人でリアルタイムに議論と投票を行うワードウルフゲーム、観戦、プロフィール、フレンド機能を提供します。</p>
      </LegalSection>
      <LegalSection title="2. アカウント">
        <p>利用者は正確な情報を登録し、認証情報を自分の責任で管理してください。同じ利用者が複数のアカウントやタブを使い、1つのゲームへ重複参加する行為は禁止します。</p>
      </LegalSection>
      <LegalSection title="3. ゲームの公平性">
        <ul>
          <li>自分のお題を画面共有や外部連絡で直接伝えないこと</li>
          <li>観戦で得た情報を進行中の参加者へ伝えないこと</li>
          <li>切断、連打、複数タブを利用して投票や進行を妨害しないこと</li>
        </ul>
      </LegalSection>
      <LegalSection title="4. チャット利用">
        <p>脅迫、差別、嫌がらせ、個人情報の投稿、違法行為への勧誘は禁止します。違反が確認された場合、対象メッセージの拒否、利用制限、アカウント停止を行うことがあります。</p>
      </LegalSection>
      <LegalSection title="5. 中断と免責">
        <p>通信障害、保守、切断猶予の超過などによりゲームを勝敗なしで中止する場合があります。本サービスは教育プロジェクトとして提供され、継続提供や完全な無停止を保証するものではありません。</p>
      </LegalSection>
      <LegalSection title="6. 規約の変更">
        <p>重要な変更はサービス内で事前に通知します。変更後も利用を継続した場合、更新後の規約に同意したものとして扱います。</p>
      </LegalSection>
    </LegalPage>
  )
}

function LegalPage({
  title,
  updated,
  backHref,
  children,
}: {
  title: string
  updated: string
  backHref: string
  children: ReactNode
}) {
  return (
    <MobileShell title={title} backHref={backHref}>
      <main className="px-5 py-6">
        <p className="mb-3 text-xs text-muted-foreground">最終更新：{updated}</p>
        <div className="space-y-6 rounded-xl border bg-white p-5 text-sm leading-7 text-stone-700">
          {children}
          <Separator />
          <p className="text-xs text-muted-foreground">
            本文はft_transcendenceのゲーム仕様と運用方針を反映しています。
          </p>
        </div>
        <div className="mt-5 text-center text-sm">
          <Link className="font-medium text-primary underline underline-offset-4" href="/login">ログイン画面へ戻る</Link>
        </div>
      </main>
    </MobileShell>
  )
}

function LegalSection({ title, children }: { title: string; children: ReactNode }) {
  return (
    <section className="space-y-2">
      <h2 className="text-balance font-semibold text-stone-950">{title}</h2>
      <div className="space-y-2 text-pretty [&_ul]:list-disc [&_ul]:space-y-1 [&_ul]:pl-5">{children}</div>
    </section>
  )
}
