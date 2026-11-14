import { renderHook, waitFor } from '@testing-library/react'
import { beforeEach, describe, expect, it, vi } from 'vitest'

import { api } from '@/lib/api-client'

import { useMatchHistory } from './use-match-history'

vi.mock('@/lib/api-client', () => ({ api: { GET: vi.fn() } }))
const getMock = vi.mocked(api.GET)

const item = {
  gameId: 'game_1',
  roomName: '深夜の読書会',
  role: 'CITIZEN' as const,
  outcome: 'WIN' as const,
  playedAt: '2026-09-26T10:17:12Z',
}

beforeEach(() => {
  vi.clearAllMocks()
})

describe('useMatchHistory', () => {
  it('新しい順の対戦履歴を limit 件だけ取得する', async () => {
    getMock.mockResolvedValue({
      data: { status: 'ok', data: { items: [item], meta: { total: 1, page: 1, limit: 5 } } },
    } as Awaited<ReturnType<typeof api.GET>>)

    const { result } = renderHook(() => useMatchHistory(5))

    await waitFor(() => expect(result.current.isLoading).toBe(false))
    expect(getMock).toHaveBeenCalledWith('/users/me/games', {
      params: { query: { page: 1, limit: 5 } },
    })
    expect(result.current.items).toEqual([item])
    expect(result.current.error).toBeNull()
  })

  it('取得に失敗したらエラーを立て、履歴は空にする', async () => {
    getMock.mockResolvedValue({
      error: { status: 'error', error: { code: 'INTERNAL_ERROR', message: 'x' } },
    } as Awaited<ReturnType<typeof api.GET>>)

    const { result } = renderHook(() => useMatchHistory(5))

    await waitFor(() => expect(result.current.isLoading).toBe(false))
    expect(result.current.items).toEqual([])
    expect(result.current.error).toBe('対戦履歴を読み込めませんでした。')
  })
})
