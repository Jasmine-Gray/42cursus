// guest-only: ログイン済みなら /home へ戻す / public: 誰でも / protected: 未ログインなら /login へ戻す
export type RouteAccess = 'guest-only' | 'public' | 'protected'

export const HOME_PATH = '/home'
export const LOGIN_PATH = '/login'

const GUEST_ONLY_PATHS = ['/', LOGIN_PATH, '/register']

// 規約は登録画面からリンクする（未ログインで読める必要がある）。
// /mock はデザイン確認用の固定データ画面、/dev は開発用ログイン（DEV ビルドのみ）。
const PUBLIC_PATH_PREFIXES = ['/privacy', '/terms', '/mock', '/dev']

export function routeAccessOf(path: string): RouteAccess {
  const normalized = path.length > 1 ? path.replace(/\/+$/, '') : path
  if (GUEST_ONLY_PATHS.includes(normalized)) return 'guest-only'
  const isPublic = PUBLIC_PATH_PREFIXES.some(
    (prefix) => normalized === prefix || normalized.startsWith(`${prefix}/`),
  )
  return isPublic ? 'public' : 'protected'
}
