import { LockKeyhole, LogOut, Mail, Plug, Unplug, Users } from 'lucide-react'
import type { Socket } from 'phoenix'
import { useEffect, useRef, useState, type FormEvent } from 'react'
import { Link, useLocation, useParams } from 'wouter'

import { SectionHeader } from '@/components/game-ui/section-header'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { useAuth } from '@/hooks/use-auth'
import { getAuthToken, getTabAuthToken, setTabAuthToken } from '@/lib/api-client'
import { createSocket, socketEndpoint } from '@/lib/channel-client'

// backend/lib/word_wolf/dev_seeds.ex の users/1 と揃える（name = dev-token-<name> の <name>）
export const devSeedUsers = [
  { name: 'yu', label: '田中 ユウ', note: 'CASE-042 ホスト・3戦2勝' },
  { name: 'minato', label: 'ミナト', note: 'CASE-042 参加' },
  { name: 'aoi', label: 'アオイ', note: 'CASE-042 参加' },
  { name: 'ren', label: 'レン', note: 'CASE-042 参加' },
  { name: 'yuna', label: 'ユナ', note: 'CASE-042 参加' },
  { name: 'kei', label: 'ケイ', note: 'CASE-042 参加' },
  { name: 'nao', label: 'ナオ', note: 'CASE-042 観戦' },
  { name: 'hana', label: 'ハナ', note: 'CASE-099 投票中' },
  { name: 'tomo', label: 'トモ', note: 'CASE-099 投票中' },
  { name: 'rio', label: 'リオ', note: 'CASE-099 投票中' },
  { name: 'sora', label: 'ソラ', note: 'CASE-099 投票中' },
  { name: 'mei', label: 'メイ', note: '部屋なし' },
] as const

const DEV_TOKEN_PREFIX = 'dev-token-'

export function devTokenFor(name: string): string {
  return `${DEV_TOKEN_PREFIX}${name}`
}

// このタブのトークンを人が読める形に。シードなら名前、登録ユーザーなら「登録ユーザー」、無ければ null
export function describeTabToken(token: string | null): string | null {
  if (!token) return null
  if (token.startsWith(DEV_TOKEN_PREFIX)) {
    const name = token.slice(DEV_TOKEN_PREFIX.length)
    const seed = devSeedUsers.find((u) => u.name === name)
    return seed ? `シード: ${seed.label}（${name}）` : `シード: ${name}`
  }
  return '登録ユーザー（メール+パスワードでこのタブにログイン）'
}

export function DevLoginPage() {
  const { name } = useParams<{ name?: string }>()
  const [, navigate] = useLocation()
  const known = name ? devSeedUsers.some((u) => u.name === name) : false

  useEffect(() => {
    if (!name || !known) return
    setTabAuthToken(devTokenFor(name))
    navigate('/home', { replace: true })
  }, [name, known, navigate])

  if (name && known) return null

  return (
    <MobileShell title="開発用ログイン（このタブだけ）" backHref="/login">
      <main className="space-y-6 px-5 py-6">
        {name && !known && (
          <p className="rounded-md border border-destructive/30 bg-destructive/10 px-3 py-2 text-sm text-destructive" role="alert">
            {name} はシードにいません。下から選ぶか、登録済みユーザーでログインしてください。
          </p>
        )}

        <TabIdentity />
        <SocketProbe />
        <SeedUserList />
        <TabLoginForm />

        <p className="text-xs leading-5 text-muted-foreground">
          ここで作るログイン状態は sessionStorage（タブ単位）に入り、他のタブや画面01 の通常ログインには影響しません。
          別のタブでこのページを開けば、同じブラウザで別のプレイヤーになれます。
        </p>
      </main>
    </MobileShell>
  )
}

function TabIdentity() {
  const [tabToken, setTabToken] = useState<string | null>(() => getTabAuthToken())
  const description = describeTabToken(tabToken)

  function handleLogout() {
    setTabAuthToken(null)
    setTabToken(null)
  }

  return (
    <section aria-labelledby="dev-tab-identity" className="rounded-xl border bg-white p-4">
      <h2 id="dev-tab-identity" className="text-sm font-semibold">
        このタブのログイン状態
      </h2>
      <p className="mt-2 text-sm">
        {description ?? (
          <span className="text-muted-foreground">未設定（ブラウザ共通のログイン状態をそのまま使います）</span>
        )}
      </p>
      <div className="mt-3 flex flex-wrap gap-2">
        <Link className="text-sm font-medium text-primary underline-offset-4 hover:underline" href="/home">
          この状態でホームを開く
        </Link>
        {tabToken && (
          <Button type="button" size="sm" variant="outline" onClick={handleLogout}>
            <LogOut className="size-4" aria-hidden="true" />
            このタブだけログアウト
          </Button>
        )}
      </div>
    </section>
  )
}

// WebSocket が「このタブの本人」で繋がることを見せる。チャンネルには入らない（ゲーム実装はレーンA/B）。
// 成功 = user_socket.ex の connect/3 が token を受理（101）。失敗 = 403（トークンなし / 無効）。
export type SocketProbeState = 'idle' | 'connecting' | 'open' | 'error' | 'closed'

export function describeSocketProbe(state: SocketProbeState): string {
  switch (state) {
    case 'idle':
      return '未接続'
    case 'connecting':
      return '接続中…'
    case 'open':
      return '接続済み（この本人で /socket が受理された）'
    case 'error':
      return '拒否された（トークンなし・無効。BE は 403 を返している）'
    case 'closed':
      return '切断された'
  }
}

function SocketProbe() {
  const [state, setState] = useState<SocketProbeState>('idle')
  const socketRef = useRef<Socket | null>(null)

  useEffect(() => {
    return () => {
      socketRef.current?.disconnect()
    }
  }, [])

  function connect() {
    socketRef.current?.disconnect()
    const socket = createSocket()
    socket.onOpen(() => setState('open'))
    socket.onError(() => {
      // 403 のあと phoenix.js は再接続を繰り返すので、見せる用途では 1 回で止める
      socket.disconnect()
      setState('error')
    })
    socket.onClose(() => setState((prev) => (prev === 'open' ? 'closed' : prev)))
    socketRef.current = socket
    setState('connecting')
    socket.connect()
  }

  function disconnect() {
    socketRef.current?.disconnect()
    socketRef.current = null
    setState('idle')
  }

  const token = getAuthToken()
  const tokenLabel = token ? (token.startsWith(DEV_TOKEN_PREFIX) ? token : `${token.slice(0, 6)}…（登録ユーザー）`) : 'なし'

  return (
    <section aria-labelledby="dev-socket-probe" className="rounded-xl border bg-white p-4">
      <h2 id="dev-socket-probe" className="text-sm font-semibold">
        WebSocket 接続テスト
      </h2>
      <p className="mt-2 text-sm">
        状態: <span data-testid="socket-state">{describeSocketProbe(state)}</span>
      </p>
      <p className="mt-1 font-mono text-xs text-muted-foreground">
        {socketEndpoint()}/websocket?token={tokenLabel}
      </p>
      <div className="mt-3 flex flex-wrap gap-2">
        <Button type="button" size="sm" onClick={connect} disabled={state === 'connecting'}>
          <Plug className="size-4" aria-hidden="true" />
          この本人で接続
        </Button>
        {(state === 'open' || state === 'connecting') && (
          <Button type="button" size="sm" variant="outline" onClick={disconnect}>
            <Unplug className="size-4" aria-hidden="true" />
            切断
          </Button>
        )}
      </div>
      <p className="mt-3 text-xs leading-5 text-muted-foreground">
        別タブで別のユーザーとして同じ操作をすると、DevTools の Network に token の違う /socket/websocket が 101 で並びます。
        チャンネル（room:* / user:*）はまだ無いので、ここでは接続の受理までを確認します。
      </p>
    </section>
  )
}

function SeedUserList() {
  return (
    <section aria-labelledby="dev-seed-users">
      <SectionHeader
        title="シードユーザーで入る"
        meta={
          <span className="inline-flex items-center gap-1 text-xs text-muted-foreground">
            <Users className="size-3.5" aria-hidden="true" />
            パスワードは全員 password123
          </span>
        }
      />
      <ul id="dev-seed-users" className="mt-3 divide-y rounded-xl border bg-white">
        {devSeedUsers.map((u) => (
          <li key={u.name}>
            <Link
              className="flex items-center gap-3 p-4 outline-none hover:bg-muted focus-visible:ring-3 focus-visible:ring-ring/50"
              href={`/dev/as/${u.name}`}
            >
              <span className="min-w-0 flex-1">
                <span className="block text-sm font-semibold">{u.label}</span>
                <span className="mt-1 block text-xs text-muted-foreground">
                  {u.name} · {u.note}
                </span>
              </span>
              <span className="font-mono text-xs text-muted-foreground">{devTokenFor(u.name)}</span>
            </Link>
          </li>
        ))}
      </ul>
    </section>
  )
}

// 自分で登録したプレイヤーで「このタブだけ」ログインする。中身は画面01 と同じ useAuth().login で、
// 保存先だけ scope: 'tab'（sessionStorage）にしている。
function TabLoginForm() {
  const [, navigate] = useLocation()
  const { login, isSubmitting, error } = useAuth({ scope: 'tab' })
  const [email, setEmail] = useState('')
  const [password, setPassword] = useState('')

  async function handleSubmit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    const ok = await login(email.trim(), password)
    if (ok) navigate('/home')
  }

  return (
    <section aria-labelledby="dev-tab-login">
      <SectionHeader title="登録済みユーザーでこのタブだけログイン" />
      <form className="mt-3 space-y-4 rounded-xl border bg-white p-4" onSubmit={handleSubmit} noValidate>
        {error && (
          <p className="rounded-md border border-destructive/30 bg-destructive/10 px-3 py-2 text-sm text-destructive" role="alert">
            {error}
          </p>
        )}
        <div>
          <Label htmlFor="dev-tab-email">メールアドレス</Label>
          <div className="relative mt-2">
            <Mail className="pointer-events-none absolute left-3 top-1/2 size-4 -translate-y-1/2 text-muted-foreground" aria-hidden="true" />
            <Input
              id="dev-tab-email"
              className="h-11 bg-white pl-10"
              type="email"
              autoComplete="email"
              placeholder="you@example.com"
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              required
            />
          </div>
        </div>
        <div>
          <Label htmlFor="dev-tab-password">パスワード</Label>
          <div className="relative mt-2">
            <LockKeyhole className="pointer-events-none absolute left-3 top-1/2 size-4 -translate-y-1/2 text-muted-foreground" aria-hidden="true" />
            <Input
              id="dev-tab-password"
              className="h-11 bg-white pl-10"
              type="password"
              autoComplete="current-password"
              placeholder="8文字以上"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              required
            />
          </div>
        </div>
        <Button className="h-11 w-full" type="submit" disabled={isSubmitting || !email || !password}>
          {isSubmitting ? 'ログイン中…' : 'このタブだけログイン'}
        </Button>
        <p className="text-xs text-muted-foreground">
          アカウントが無ければ{' '}
          <Link className="font-medium text-primary underline-offset-4 hover:underline" href="/register">
            新規登録
          </Link>
          （こちらはブラウザ共通のログインになります）
        </p>
      </form>
    </section>
  )
}
