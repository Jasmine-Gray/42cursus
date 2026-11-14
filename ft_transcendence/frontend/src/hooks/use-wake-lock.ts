import { useEffect } from 'react'

export function useWakeLock(enabled: boolean): void {
  useEffect(() => {
    if (!enabled || typeof navigator === 'undefined' || !('wakeLock' in navigator)) return

    let sentinel: WakeLockSentinel | null = null
    let cancelled = false

    const acquire = async () => {
      if (document.visibilityState !== 'visible') return
      try {
        const next = await navigator.wakeLock.request('screen')
        if (cancelled) {
          void next.release()
          return
        }
        sentinel = next
      } catch {
        // 省電力モード・権限ポリシー等で拒否された。画面はふつうにスリープするだけなので続行する
      }
    }

    const handleVisibility = () => {
      if (document.visibilityState === 'visible') void acquire()
    }

    void acquire()
    document.addEventListener('visibilitychange', handleVisibility)

    return () => {
      cancelled = true
      document.removeEventListener('visibilitychange', handleVisibility)
      void sentinel?.release()
      sentinel = null
    }
  }, [enabled])
}
