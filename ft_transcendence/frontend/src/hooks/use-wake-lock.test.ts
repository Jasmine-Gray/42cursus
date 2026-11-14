import { renderHook, waitFor } from '@testing-library/react'
import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'

import { useWakeLock } from './use-wake-lock'

const release = vi.fn(async () => undefined)
const request = vi.fn(async () => ({ release }))

function setVisibility(state: DocumentVisibilityState) {
  Object.defineProperty(document, 'visibilityState', { configurable: true, get: () => state })
  document.dispatchEvent(new Event('visibilitychange'))
}

beforeEach(() => {
  vi.clearAllMocks()
  Object.defineProperty(navigator, 'wakeLock', { configurable: true, value: { request } })
  setVisibility('visible')
})

afterEach(() => {
  Reflect.deleteProperty(navigator, 'wakeLock')
})

describe('useWakeLock', () => {
  it('有効なら画面のスリープ抑止を取り、アンマウントで解放する', async () => {
    const { unmount } = renderHook(() => useWakeLock(true))
    await waitFor(() => expect(request).toHaveBeenCalledWith('screen'))

    unmount()

    await waitFor(() => expect(release).toHaveBeenCalled())
  })

  it('画面に戻った（visibilitychange で visible）ら取り直す', async () => {
    renderHook(() => useWakeLock(true))
    await waitFor(() => expect(request).toHaveBeenCalledTimes(1))

    // 別アプリへ切り替える・スリープすると、ブラウザが自動で解放する
    setVisibility('hidden')
    setVisibility('visible')

    await waitFor(() => expect(request).toHaveBeenCalledTimes(2))
  })

  it('無効なら取らない', () => {
    renderHook(() => useWakeLock(false))
    expect(request).not.toHaveBeenCalled()
  })

  it('Wake Lock に対応していないブラウザでは何もしない（落ちない）', () => {
    Reflect.deleteProperty(navigator, 'wakeLock')
    expect(() => renderHook(() => useWakeLock(true))).not.toThrow()
  })

  it('拒否されても（省電力モード等）例外を外へ出さない', async () => {
    request.mockRejectedValueOnce(new Error('NotAllowedError'))
    renderHook(() => useWakeLock(true))
    await waitFor(() => expect(request).toHaveBeenCalled())
  })
})
