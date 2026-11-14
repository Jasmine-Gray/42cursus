import { Socket } from 'phoenix'

import { getAuthToken } from '@/lib/api-client'

// VITE_API_BASE が http(s)://host なら ws(s)://host/socket、未指定なら同一オリジンの /socket（Caddy / vite proxy が :4000 へ流す）
export function socketEndpoint(apiBase: string | undefined = import.meta.env.VITE_API_BASE): string {
  if (!apiBase || apiBase === '/') return '/socket'
  return `${apiBase.replace(/^http/, 'ws').replace(/\/$/, '')}/socket`
}

// 接続パラメータ。関数で渡すと再接続のたびに評価されるので、タブのトークンが変わっても追従する。
export function socketParams(): { token: string } {
  return { token: getAuthToken() ?? '' }
}

export function createSocket(): Socket {
  return new Socket(socketEndpoint(), { params: socketParams })
}
