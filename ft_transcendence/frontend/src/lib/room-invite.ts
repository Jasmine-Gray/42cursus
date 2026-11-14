import type { components } from '@/types/api'

type ParticipantType = components['schemas']['ParticipantType']

export function roomInviteUrl(
  inviteCode: string,
  participantType: ParticipantType,
) {
  const url = new URL(
    `/join/${encodeURIComponent(inviteCode)}`,
    window.location.origin,
  )
  url.searchParams.set(
    'mode',
    participantType === 'SPECTATOR' ? 'spectate' : 'player',
  )
  return url.toString()
}

export function inviteParticipantType(input: string): ParticipantType | null {
  try {
    const url = new URL(input, window.location.origin)
    if (!/^\/join\/[a-z0-9-]+$/i.test(url.pathname)) return null
    if (url.searchParams.get('mode') === 'spectate') return 'SPECTATOR'
    if (url.searchParams.get('mode') === 'player') return 'PLAYER'
  } catch {
    // コードだけの入力や入力途中では、参加方法を固定しない。
  }
  return null
}

/** 認証後に戻す先は、このサイトの招待画面だけに限定する。 */
export function safeInvitePath(input: string | null): string | null {
  if (!input || !/^\/join\/[a-z0-9-]+(?:\?.*)?$/i.test(input)) return null
  const url = new URL(input, window.location.origin)
  const mode = url.searchParams.get('mode')
  return (
    url.pathname +
    (mode === 'player' || mode === 'spectate' ? `?mode=${mode}` : '')
  )
}

export function inviteReturnPath(search: string): string | null {
  return safeInvitePath(new URLSearchParams(search).get('returnTo'))
}

export function withInviteReturn(
  path: string,
  invitePath: string | null,
): string {
  return invitePath
    ? `${path}?returnTo=${encodeURIComponent(invitePath)}`
    : path
}
