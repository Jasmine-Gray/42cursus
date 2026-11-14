// 向き先は VITE_API_BASE で切り替える（統合の動線: _docs/dev/conventions.html §08）。
//   段階2: VITE_API_BASE=http://localhost:4010 npm run dev   ← prism mock（make mock）
//   段階3: VITE_API_BASE=http://localhost:4000 npm run dev   ← 実BE直
//   未指定: 同一オリジン（= Caddy 経由の本結合。開発時は vite.config.ts の proxy が :4000 へ転送）
// コード側に mock 分岐を書かない。
import createClient, { type Middleware } from 'openapi-fetch'
import type { paths } from '@/types/api'

const TOKEN_STORAGE_KEY = 'word_wolf_auth_token'
// タブ単位の上書き。sessionStorage はタブごとに独立なので、同じブラウザの別タブを別ユーザーにできる。
// 書くのは開発用ページ /dev/as/<name>（pages/dev-login-page.tsx）だけ。通常ログインは localStorage に書く。
const TAB_TOKEN_STORAGE_KEY = 'word_wolf_auth_token_tab'

// サーバーに拒否された（401 UNAUTHENTICATED）トークン。VITE_DEV_AUTH_TOKEN は消せないので、ここで無効扱いにする
let rejectedToken: string | null = null

export function getAuthToken(): string | null {
  const token = readTabToken() ?? readStoredToken() ?? devAuthToken()
  return token !== null && token === rejectedToken ? null : token
}

// 通常のログイン / ログアウト。タブ単位の上書きも消して、このタブの本人を確定させる。
export function setAuthToken(token: string | null): void {
  writeTabToken(null)
  writeStoredToken(token)
  if (token === null) notifySessionEnd()
}

// このタブだけのトークンを読む（開発用ページの状態表示に使う。通常の読出は getAuthToken）。
export function getTabAuthToken(): string | null {
  return readTabToken()
}

// このタブだけを別ユーザーにする（開発用）。null で解除。
export function setTabAuthToken(token: string | null): void {
  writeTabToken(token)
  if (token === null) notifySessionEnd()
}

// --- セッション終了の通知（ログアウト・401・別タブでのログアウト）--------------------
// ログイン（トークンの保存）では通知しない。AuthGate はこれを購読して /login へ戻す。
let sessionEndVersion = 0
const sessionEndListeners = new Set<() => void>()

function notifySessionEnd(): void {
  sessionEndVersion += 1
  sessionEndListeners.forEach((listener) => listener())
}

export function getSessionEndVersion(): number {
  return sessionEndVersion
}

export function subscribeSessionEnd(listener: () => void): () => void {
  sessionEndListeners.add(listener)
  return () => {
    sessionEndListeners.delete(listener)
  }
}

if (typeof window !== 'undefined') {
  // 別タブでログアウトした（localStorage のトークンが消えた）
  window.addEventListener('storage', (event) => {
    if (event.key === TOKEN_STORAGE_KEY && event.newValue === null) notifySessionEnd()
  })
}

function rejectToken(token: string): void {
  if (token === rejectedToken) return // 並行リクエストの 401 で何度も通知しない
  rejectedToken = token
  if (readTabToken() === token) writeTabToken(null)
  if (readStoredToken() === token) writeStoredToken(null)
  notifySessionEnd()
}

async function isSessionRejected(response: Response): Promise<boolean> {
  if (response.status !== 401) return false
  try {
    const body: unknown = await response.clone().json()
    if (typeof body !== 'object' || body === null || !('error' in body)) return false
    const { error } = body
    // INVALID_CREDENTIALS（ログイン失敗）はセッションの失効ではないので除く
    return typeof error === 'object' && error !== null && 'code' in error && error.code === 'UNAUTHENTICATED'
  } catch {
    return false
  }
}

function writeStoredToken(token: string | null): void {
  try {
    if (token) {
      localStorage.setItem(TOKEN_STORAGE_KEY, token)
    } else {
      localStorage.removeItem(TOKEN_STORAGE_KEY)
    }
  } catch {
    // 保存できなくてもアプリの動作自体は継続する
  }
}

function writeTabToken(token: string | null): void {
  try {
    if (token) {
      sessionStorage.setItem(TAB_TOKEN_STORAGE_KEY, token)
    } else {
      sessionStorage.removeItem(TAB_TOKEN_STORAGE_KEY)
    }
  } catch {
    // sessionStorage が使えない環境では何もしない
  }
}

function readTabToken(): string | null {
  try {
    return sessionStorage.getItem(TAB_TOKEN_STORAGE_KEY)
  } catch {
    return null
  }
}

function readStoredToken(): string | null {
  try {
    return localStorage.getItem(TOKEN_STORAGE_KEY)
  } catch {
    // プライベートブラウジング等で localStorage が使えない場合は未ログイン扱い
    return null
  }
}

// 開発時のみ: VITE_DEV_AUTH_TOKEN を設定するとログインせずにシードユーザーとして動かせる
// （WordWolf.DevSeeds の 田中 ユウ は dev-token-yu）。未設定なら通常どおりログインを要求する。
function devAuthToken(): string | null {
  if (!import.meta.env.DEV) return null
  const token: unknown = import.meta.env.VITE_DEV_AUTH_TOKEN
  return typeof token === 'string' && token.length > 0 ? token : null
}

const authMiddleware: Middleware = {
  async onRequest({ request }) {
    const token = getAuthToken()
    if (token) {
      request.headers.set('Authorization', `Bearer ${token}`)
    }
    return request
  },
  // セッションが失効していたら（401 UNAUTHENTICATED）送ったトークンを捨てる。AuthGate が /login へ戻す
  async onResponse({ request, response }) {
    const sentToken = request.headers.get('Authorization')?.replace(/^Bearer /, '')
    if (sentToken && (await isSessionRejected(response))) rejectToken(sentToken)
    return response
  },
}

export const api = createClient<paths>({
  baseUrl: import.meta.env.VITE_API_BASE ?? '/',
})

api.use(authMiddleware)
