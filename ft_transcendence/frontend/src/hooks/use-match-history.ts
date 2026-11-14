import { useEffect, useState } from 'react'

import { api } from '@/lib/api-client'
import type { components } from '@/types/api'

export type MatchHistoryItem = components['schemas']['MatchHistoryItem']

type MatchHistory = {
  items: MatchHistoryItem[]
  isLoading: boolean
  error: string | null
}

export function useMatchHistory(limit: number): MatchHistory {
  const [history, setHistory] = useState<MatchHistory>({
    items: [],
    isLoading: true,
    error: null,
  })

  useEffect(() => {
    let cancelled = false

    void (async () => {
      try {
        const { data, error } = await api.GET('/users/me/games', {
          params: { query: { page: 1, limit } },
        })
        if (cancelled) return
        if (error || !data || data.status !== 'ok') {
          setHistory({ items: [], isLoading: false, error: '対戦履歴を読み込めませんでした。' })
          return
        }
        setHistory({ items: data.data.items, isLoading: false, error: null })
      } catch {
        if (!cancelled) {
          setHistory({ items: [], isLoading: false, error: '対戦履歴を読み込めませんでした。' })
        }
      }
    })()

    return () => {
      cancelled = true
    }
  }, [limit])

  return history
}
