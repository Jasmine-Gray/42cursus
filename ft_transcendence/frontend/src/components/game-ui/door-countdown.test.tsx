import { act, cleanup, fireEvent, render, screen } from '@testing-library/react'
import { StrictMode } from 'react'
import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'

import { DoorCountdown } from './door-countdown'

beforeEach(() => {
  vi.spyOn(HTMLMediaElement.prototype, 'play').mockResolvedValue()
  vi.spyOn(HTMLMediaElement.prototype, 'pause').mockImplementation(() => {})
  vi.useFakeTimers()
  vi.setSystemTime(new Date('2026-09-05T12:00:00Z'))
})

afterEach(() => {
  cleanup()
  vi.useRealTimers()
  vi.restoreAllMocks()
})

describe('DoorCountdown', () => {
  it('3・2・1を表示し、開始演出で遷移期限を延ばさない', () => {
    const onComplete = vi.fn()
    render(<DoorCountdown endsAt={Date.now() + 3000} onComplete={onComplete} />)
    expect(screen.getByRole('timer')).toHaveAccessibleName('ゲーム開始まで3秒')
    act(() => vi.advanceTimersByTime(1000))
    expect(screen.getByRole('timer')).toHaveAccessibleName('ゲーム開始まで2秒')
    act(() => vi.advanceTimersByTime(1000))
    expect(screen.getByRole('timer')).toHaveAccessibleName('ゲーム開始まで1秒')
    expect(onComplete).not.toHaveBeenCalled()
    act(() => vi.advanceTimersByTime(1000))
    expect(onComplete).toHaveBeenCalledTimes(1)
    act(() => vi.advanceTimersByTime(1000))
    expect(onComplete).toHaveBeenCalledTimes(1)
  })

  it('バックグラウンドから期限後に復帰しても3秒をやり直さない', () => {
    const onComplete = vi.fn()
    render(<DoorCountdown endsAt={Date.now() + 3000} onComplete={onComplete} />)
    vi.setSystemTime(Date.now() + 10000)
    act(() => vi.advanceTimersByTime(50))
    expect(onComplete).toHaveBeenCalledTimes(1)
  })

  it('タブへの復帰時は次のタイマーを待たずに期限を確認する', () => {
    const onComplete = vi.fn()
    const { unmount } = render(
      <DoorCountdown endsAt={Date.now() + 3000} onComplete={onComplete} />,
    )
    vi.setSystemTime(Date.now() + 10000)
    act(() => document.dispatchEvent(new Event('visibilitychange')))
    expect(onComplete).toHaveBeenCalledTimes(1)
    unmount()
    act(() => document.dispatchEvent(new Event('visibilitychange')))
    expect(onComplete).toHaveBeenCalledTimes(1)
  })

  it('動画の読み込みが遅れても開始時刻に合う位置から再生する', () => {
    const onComplete = vi.fn()
    const { container } = render(
      <DoorCountdown endsAt={Date.now() + 3000} onComplete={onComplete} />,
    )
    const video = container.querySelector('video')!
    act(() => vi.advanceTimersByTime(1200))
    Object.defineProperty(video, 'readyState', { value: 1 })
    Object.defineProperty(video, 'duration', { value: 3 })
    fireEvent.loadedMetadata(video)
    expect(video.currentTime).toBeCloseTo(1.2)
    expect(video.play).toHaveBeenCalledTimes(1)
    fireEvent.ended(video)
    expect(onComplete).not.toHaveBeenCalled()
    act(() => vi.advanceTimersByTime(1800))
    expect(onComplete).toHaveBeenCalledTimes(1)
  })

  it('動画が読めなくても静止画でカウントを続け、期限に完了する', () => {
    const onComplete = vi.fn()
    const { container } = render(
      <DoorCountdown endsAt={Date.now() + 3000} onComplete={onComplete} />,
    )
    fireEvent.error(container.querySelector('video')!)
    expect(container.querySelector('video')).toBeNull()
    expect(container.querySelector('img')).toHaveAttribute(
      'src',
      '/cinematics/entrance-poster.jpg',
    )
    act(() => vi.advanceTimersByTime(3000))
    expect(onComplete).toHaveBeenCalledTimes(1)
  })

  it('イベント登録前に動画が失敗していても静止画へ切り替える', () => {
    const { container } = render(
      <DoorCountdown endsAt={Date.now() + 3000} onComplete={vi.fn()} />,
    )
    Object.defineProperty(container.querySelector('video')!, 'error', {
      value: { code: 4 },
    })
    act(() => vi.advanceTimersByTime(50))
    expect(container.querySelector('video')).toBeNull()
    expect(container.querySelector('img')).toBeInTheDocument()
  })

  it('未来の開始時刻までは動画を止めて待つ', () => {
    const { container } = render(
      <DoorCountdown endsAt={Date.now() + 3300} onComplete={vi.fn()} />,
    )
    const video = container.querySelector('video')!
    Object.defineProperty(video, 'readyState', { value: 1 })
    Object.defineProperty(video, 'duration', { value: 3 })
    fireEvent.loadedMetadata(video)
    expect(video.play).not.toHaveBeenCalled()
    act(() => vi.advanceTimersByTime(300))
    expect(video.play).toHaveBeenCalledTimes(1)
  })

  it('すでに期限を過ぎた開始イベントは即座に完了させる', () => {
    const onComplete = vi.fn()
    render(
      <StrictMode>
        <DoorCountdown endsAt={Date.now() - 1000} onComplete={onComplete} />
      </StrictMode>,
    )
    expect(onComplete).toHaveBeenCalledTimes(1)
  })
})
