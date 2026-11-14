import { useSyncExternalStore, type ReactNode } from 'react'
import { Redirect, useLocation, useSearch } from 'wouter'

import {
  getAuthToken,
  getSessionEndVersion,
  subscribeSessionEnd,
} from '@/lib/api-client'
import {
  inviteReturnPath,
  safeInvitePath,
  withInviteReturn,
} from '@/lib/room-invite'
import { HOME_PATH, LOGIN_PATH, routeAccessOf } from '@/lib/route-access'

export function AuthGate({ children }: { children: ReactNode }) {
  const [location] = useLocation()
  const search = useSearch()
  // 再描画するのはパスが変わった時と、セッションが終わった時（ログアウト・401・別タブのログアウト）だけ。
  // ログイン直後にトークンが増えても再描画しない。遷移先は各画面が決める（登録 → /avatar/setup など）。
  useSyncExternalStore(subscribeSessionEnd, getSessionEndVersion)
  const hasSession = getAuthToken() !== null
  const access = routeAccessOf(location)

  if (access === 'guest-only' && hasSession) {
    return <Redirect to={inviteReturnPath(search) ?? HOME_PATH} replace />
  }
  if (access === 'protected' && !hasSession) {
    return (
      <Redirect
        to={withInviteReturn(
          LOGIN_PATH,
          safeInvitePath(`${location}${search ? `?${search}` : ''}`),
        )}
        replace
      />
    )
  }
  return <>{children}</>
}
