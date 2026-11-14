import { describe, expect, it } from 'vitest'

import { routeAccessOf } from './route-access'

describe('routeAccessOf', () => {
  it.each(['/', '/login', '/register', '/login/'])('%s は未ログイン専用（guest-only）', (path) => {
    expect(routeAccessOf(path)).toBe('guest-only')
  })

  it.each(['/privacy', '/terms', '/mock/game', '/mock/lobby/player', '/dev/as', '/dev/as/minato'])(
    '%s は誰でも見られる（public）',
    (path) => {
      expect(routeAccessOf(path)).toBe('public')
    },
  )

  it.each(['/home', '/avatar/setup', '/rooms', '/rooms/new', '/join/ABC123', '/lobby', '/game', '/friends', '/profile', '/unknown'])(
    '%s はログイン必須（protected）',
    (path) => {
      expect(routeAccessOf(path)).toBe('protected')
    },
  )

  it('前方一致は区切り（/）単位で見る（/terms-old は public にしない）', () => {
    expect(routeAccessOf('/terms-old')).toBe('protected')
    expect(routeAccessOf('/mockup')).toBe('protected')
  })
})
