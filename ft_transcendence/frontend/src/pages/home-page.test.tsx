import { render, screen } from '@testing-library/react'
import { beforeEach, describe, expect, it, vi } from 'vitest'

import { api } from '@/lib/api-client'
import type { components } from '@/types/api'

import { HomePage } from './home-page'

vi.mock('@/lib/api-client', () => ({
  api: { GET: vi.fn() },
  getAuthToken: () => null,
}))

const getMock = vi.mocked(api.GET)

const meData: components['schemas']['MeData'] = {
  user: {
    userId: 'u-1',
    displayName: '田中 ユウ',
    avatarUrl: '/avatars/yu.webp',
    email: 'yu@example.com',
  },
  stats: { gamesPlayed: 12, wins: 8, losses: 4 },
  friendCount: 8,
  activeRoom: null,
}

function resolveGet(data: components['schemas']['MeData']) {
  getMock.mockResolvedValue({
    data: { status: 'ok', data },
    response: new Response(null, { status: 200 }),
  } as Awaited<ReturnType<typeof api.GET>>)
}

beforeEach(() => {
  getMock.mockReset()
  window.history.pushState({}, '', '/')
})

describe('HomePage', () => {
  it('最初はローディングを表示する', () => {
    getMock.mockReturnValue(new Promise(() => {}) as ReturnType<typeof api.GET>)
    render(<HomePage />)

    expect(screen.getByText('読み込み中...')).toBeInTheDocument()
  })

  it('取得成功で本人のカードとクライアント導出の勝率を表示する', async () => {
    resolveGet(meData)
    render(<HomePage />)

    expect(
      await screen.findByRole('complementary', {
        name: 'あなたのプレイヤーカード',
      }),
    ).toBeInTheDocument()
    expect(
      screen.getByRole('heading', { name: '田中 ユウ' }),
    ).toBeInTheDocument()
    // 12戦8勝 => 67%
    expect(screen.getByText('67%')).toBeInTheDocument()
  })

  it('activeRoom があればロビー復帰カードを表示する', async () => {
    resolveGet({
      ...meData,
      activeRoom: {
        roomId: 'room_1',
        inviteCode: 'CASE-042',
        roomName: '深夜の読書会',
        status: 'WAITING',
        genres: ['PLACE', 'FOOD', 'HOBBY'],
        playerCount: 6,
        maxPlayers: 8,
        spectatorCount: 1,
        joinable: true,
      },
    })
    render(<HomePage />)

    expect(
      await screen.findByRole('heading', { name: '深夜の読書会' }),
    ).toBeInTheDocument()
    expect(screen.getByText('募集中')).toBeInTheDocument()
    expect(screen.getByRole('link', { name: /ロビーへ戻る/ })).toHaveAttribute(
      'href',
      '/lobby?roomId=room_1',
    )
  })

  it('取得失敗でエラーメッセージを表示する', async () => {
    getMock.mockResolvedValue({
      error: {
        status: 'error',
        error: { code: 'BAD_REQUEST', message: 'boom' },
      },
      response: new Response(null, { status: 400 }),
    } as Awaited<ReturnType<typeof api.GET>>)
    render(<HomePage />)

    expect(
      await screen.findByText('データの取得に失敗しました'),
    ).toBeInTheDocument()
  })
})
