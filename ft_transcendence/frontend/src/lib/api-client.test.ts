/// <reference types="node" />

import { File as NodeFile } from 'node:buffer'
import { afterAll, afterEach, beforeAll, beforeEach, describe, expect, it, vi } from 'vitest'

type ApiClientModule = typeof import('./api-client')

let client: ApiClientModule
const fetchMock = vi.fn()

function okResponse() {
  return new Response(JSON.stringify({ status: 'ok', data: { status: 'ok' } }), {
    status: 200,
    headers: { 'Content-Type': 'application/json' },
  })
}

function sentRequest(): Request {
  const [input] = fetchMock.mock.calls[0] as [Request]
  return input
}

beforeAll(async () => {
  vi.stubGlobal('File', NodeFile)
  // NodeのResponseから同じ実装のFormDataを得る。jsdomとの混在を避ける。
  const formData = await new Response('', {
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
  }).formData()
  vi.stubGlobal('FormData', formData.constructor)
  // openapi-fetch は createClient 時点の globalThis.fetch を掴むので、読み込む前に差し替える。
  // Node の Request は相対 URL を受け付けないので、テストでは絶対 URL を向き先にする。
  vi.stubGlobal('fetch', fetchMock)
  vi.stubEnv('VITE_API_BASE', 'http://backend.test')
  vi.resetModules()
  client = await import('./api-client')
})

afterAll(() => {
  vi.unstubAllGlobals()
  vi.unstubAllEnvs()
})

beforeEach(() => {
  localStorage.clear()
  sessionStorage.clear()
  fetchMock.mockReset()
  fetchMock.mockResolvedValue(okResponse())
})

afterEach(() => {
  localStorage.clear()
  sessionStorage.clear()
})

describe('api-client の認証トークン', () => {
  it('setAuthToken で保存したトークンを getAuthToken が返す', () => {
    expect(client.getAuthToken()).toBeNull()
    client.setAuthToken('tok-1')
    expect(client.getAuthToken()).toBe('tok-1')
    client.setAuthToken(null)
    expect(client.getAuthToken()).toBeNull()
  })

  it('タブ単位のトークン（setTabAuthToken）は通常のトークンより優先され、別タブには漏れない', async () => {
    client.setAuthToken('tok-login')
    client.setTabAuthToken('dev-token-minato')

    expect(client.getAuthToken()).toBe('dev-token-minato')
    expect(sessionStorage.getItem('word_wolf_auth_token_tab')).toBe('dev-token-minato')
    expect(localStorage.getItem('word_wolf_auth_token')).toBe('tok-login') // localStorage 側は上書きしない

    await client.api.GET('/users/me')
    expect(sentRequest().headers.get('authorization')).toBe('Bearer dev-token-minato')
  })

  it('通常ログイン（setAuthToken）はタブ単位の上書きを解除して本人を確定させる', () => {
    client.setTabAuthToken('dev-token-minato')
    client.setAuthToken('tok-login')

    expect(client.getAuthToken()).toBe('tok-login')
    expect(sessionStorage.getItem('word_wolf_auth_token_tab')).toBeNull()

    client.setAuthToken(null) // ログアウトで両方消える
    expect(client.getAuthToken()).toBeNull()
  })

  it('保存したトークンを Authorization: Bearer として全リクエストに付ける', async () => {
    client.setAuthToken('tok-1')

    await client.api.GET('/users/me')

    expect(fetchMock).toHaveBeenCalledTimes(1)
    expect(sentRequest().url).toBe('http://backend.test/users/me')
    expect(sentRequest().headers.get('authorization')).toBe('Bearer tok-1')
  })

  it('トークンが無ければ Authorization を付けない', async () => {
    await client.api.GET('/health')

    expect(sentRequest().headers.get('authorization')).toBeNull()
  })

  it('multipart は bodySerializer の FormData をそのまま送る（Content-Type は JSON にしない）', async () => {
    client.setAuthToken('tok-1')
    // Request/FormDataはNode実装。jsdomのFileを混ぜるとNode 24の送信時に拒否される。
    const file = new File([new Uint8Array([137, 80, 78, 71])], 'avatar.png', { type: 'image/png' })

    await client.api.PUT('/users/me/avatar', {
      body: { file: file.name },
      bodySerializer: () => {
        const formData = new FormData()
        formData.append('file', file)
        return formData
      },
    })

    const request = sentRequest()
    expect(request.method).toBe('PUT')
    expect(request.headers.get('authorization')).toBe('Bearer tok-1')
    expect(request.headers.get('content-type') ?? '').not.toContain('application/json')
    const body = await request.formData()
    const uploaded = body.get('file') as File
    expect(uploaded).toMatchObject({ name: 'avatar.png', type: 'image/png' })
    expect(new Uint8Array(await uploaded.arrayBuffer())).toEqual(new Uint8Array([137, 80, 78, 71]))
  })
})

function errorResponse(status: number, code: string) {
  return new Response(JSON.stringify({ status: 'error', error: { code, message: code } }), {
    status,
    headers: { 'Content-Type': 'application/json' },
  })
}

describe('api-client のセッション終了', () => {
  it('401 UNAUTHENTICATED なら送ったトークンを捨て、セッション終了を通知する', async () => {
    const listener = vi.fn()
    const unsubscribe = client.subscribeSessionEnd(listener)
    client.setAuthToken('tok-expired')
    fetchMock.mockResolvedValueOnce(errorResponse(401, 'UNAUTHENTICATED'))

    await client.api.GET('/users/me')

    expect(client.getAuthToken()).toBeNull()
    expect(localStorage.getItem('word_wolf_auth_token')).toBeNull()
    expect(listener).toHaveBeenCalledTimes(1)
    unsubscribe()
  })

  it('タブ単位のトークンが拒否されたら、それだけを捨てて通常のトークンに戻る', async () => {
    client.setAuthToken('tok-login')
    client.setTabAuthToken('dev-token-expired')
    fetchMock.mockResolvedValueOnce(errorResponse(401, 'UNAUTHENTICATED'))

    await client.api.GET('/users/me')

    expect(client.getAuthToken()).toBe('tok-login')
  })

  it('401 INVALID_CREDENTIALS（ログイン失敗）ではトークンを捨てない', async () => {
    const listener = vi.fn()
    const unsubscribe = client.subscribeSessionEnd(listener)
    client.setTabAuthToken('dev-token-minato')
    fetchMock.mockResolvedValueOnce(errorResponse(401, 'INVALID_CREDENTIALS'))

    await client.api.POST('/auth/login', { body: { email: 'a@example.com', password: 'wrong-pass' } })

    expect(client.getAuthToken()).toBe('dev-token-minato')
    expect(listener).not.toHaveBeenCalled()
    unsubscribe()
  })

  it('ログアウト（setAuthToken(null)）はセッション終了を通知し、ログイン（トークンを保存）は通知しない', () => {
    const listener = vi.fn()
    const unsubscribe = client.subscribeSessionEnd(listener)

    client.setAuthToken('tok-1')
    expect(listener).not.toHaveBeenCalled()

    client.setAuthToken(null)
    expect(listener).toHaveBeenCalledTimes(1)
    unsubscribe()
  })
})
