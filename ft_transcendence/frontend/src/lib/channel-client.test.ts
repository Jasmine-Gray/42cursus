import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'

import { createSocket, socketEndpoint, socketParams } from './channel-client'

const socketCtor = vi.fn()

vi.mock('phoenix', () => ({
  Socket: class {
    constructor(endpoint: string, opts: unknown) {
      socketCtor(endpoint, opts)
    }
  },
}))

beforeEach(() => {
  localStorage.clear()
  sessionStorage.clear()
  socketCtor.mockReset()
})

afterEach(() => {
  localStorage.clear()
  sessionStorage.clear()
})

describe('socketEndpoint', () => {
  it('VITE_API_BASE 未指定なら同一オリジンの /socket', () => {
    expect(socketEndpoint(undefined)).toBe('/socket')
    expect(socketEndpoint('/')).toBe('/socket')
  })

  it('http(s) の API ベースは ws(s) に変えて /socket を付ける', () => {
    expect(socketEndpoint('http://localhost:4000')).toBe('ws://localhost:4000/socket')
    expect(socketEndpoint('https://example.test/')).toBe('wss://example.test/socket')
  })
})

describe('socketParams / createSocket', () => {
  it('token はタブ単位（sessionStorage）を優先し、無ければブラウザ共通（localStorage）を使う', () => {
    localStorage.setItem('word_wolf_auth_token', 'tok-browser')
    expect(socketParams()).toEqual({ token: 'tok-browser' })

    sessionStorage.setItem('word_wolf_auth_token_tab', 'dev-token-minato')
    expect(socketParams()).toEqual({ token: 'dev-token-minato' })
  })

  it('トークンが無ければ空文字（BE は :error で拒否する）', () => {
    expect(socketParams()).toEqual({ token: '' })
  })

  it('createSocket は /socket に params 関数を渡す（再接続時に再評価される）', () => {
    sessionStorage.setItem('word_wolf_auth_token_tab', 'dev-token-yu')
    createSocket()

    expect(socketCtor).toHaveBeenCalledTimes(1)
    const [endpoint, opts] = socketCtor.mock.calls[0] as [string, { params: () => { token: string } }]
    expect(endpoint).toBe('/socket')
    expect(typeof opts.params).toBe('function')
    expect(opts.params()).toEqual({ token: 'dev-token-yu' })
  })
})
