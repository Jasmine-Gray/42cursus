import { fireEvent, render, screen } from '@testing-library/react'
import { beforeEach, describe, expect, it, vi } from 'vitest'

import type { RoomChatMessage, RoomChannelStatus, SendErrorCode } from '@/hooks/use-room-channel'

import { ChatThread } from './chat-thread'

const sendMock = vi.fn()

let hookValue: {
  messages: RoomChatMessage[]
  status: RoomChannelStatus
  sendError: SendErrorCode | null
  send: typeof sendMock
}

vi.mock('@/hooks/use-me', () => ({
  useMe: () => ({ user: { userId: 'me', displayName: 'タナカユウ', avatarUrl: '/avatars/yu.webp', email: 'me@example.com' } }),
}))
vi.mock('@/hooks/use-room-channel', () => ({
  useRoomChannel: () => hookValue,
}))

beforeEach(() => {
  sendMock.mockReset()
  hookValue = {
    messages: [
      { messageId: 'm1', userId: 'user_aoi', displayName: 'アオイ', content: '静かな場所です', sentAt: '2026-09-06T21:04:00Z' },
      { messageId: 'm2', userId: 'me', displayName: 'サーバー名は無視', content: 'なるほど', sentAt: '2026-09-06T21:05:00Z' },
    ],
    status: 'ready',
    sendError: null,
    send: sendMock,
  }
})

describe('ChatThread', () => {
  it('メッセージを表示する（他人は displayName、自分は use-me の displayName）', () => {
    render(<ChatThread roomId="room_1" gameId="game_1" />)

    expect(screen.getByText('静かな場所です')).toBeInTheDocument()
    expect(screen.getByText('なるほど')).toBeInTheDocument()
    expect(screen.getByText('アオイ')).toBeInTheDocument()
    // 自分の発言はサーバーの displayName ではなく use-me の displayName を出す
    expect(screen.getByText('タナカユウ')).toBeInTheDocument()
    expect(screen.queryByText('サーバー名は無視')).not.toBeInTheDocument()
  })

  it('readOnly では入力欄を出さず、閲覧のみの案内を表示する', () => {
    render(<ChatThread roomId="room_1" gameId="game_1" readOnly />)

    expect(screen.queryByLabelText('メッセージ')).not.toBeInTheDocument()
    expect(screen.getByText('観戦中は閲覧のみです')).toBeInTheDocument()
  })

  it('入力して送信すると trim した本文で send を呼び、入力欄を空にする', () => {
    render(<ChatThread roomId="room_1" gameId="game_1" />)

    const textarea = screen.getByLabelText('メッセージ') as HTMLTextAreaElement
    fireEvent.change(textarea, { target: { value: '  容疑者はあの人  ' } })
    fireEvent.click(screen.getByRole('button', { name: /送信/ }))

    expect(sendMock).toHaveBeenCalledWith('容疑者はあの人')
    expect(textarea.value).toBe('')
  })

  it('空のまま送信するとエラーを出し、send は呼ばない', () => {
    render(<ChatThread roomId="room_1" gameId="game_1" />)

    fireEvent.submit(screen.getByLabelText('メッセージ').closest('form') as HTMLFormElement)

    expect(screen.getByRole('alert')).toHaveTextContent('メッセージを入力してください。')
    expect(sendMock).not.toHaveBeenCalled()
  })

  it('sendError（RATE_LIMITED）を案内文にして表示する', () => {
    hookValue.sendError = 'RATE_LIMITED'
    render(<ChatThread roomId="room_1" gameId="game_1" />)

    expect(screen.getByRole('alert')).toHaveTextContent('送信が早すぎます')
  })

  it('接続が error のときは再接続バナーを出し、送信ボタンを無効にする', () => {
    hookValue.status = 'error'
    render(<ChatThread roomId="room_1" gameId="game_1" />)

    expect(screen.getByRole('status')).toHaveTextContent('接続が不安定です')

    const textarea = screen.getByLabelText('メッセージ')
    fireEvent.change(textarea, { target: { value: 'あ' } })
    expect(screen.getByRole('button', { name: /送信/ })).toBeDisabled()
  })

  describe('キーボード送信（⌘/Ctrl + Enter で送信、Enter は改行）', () => {
    function typeMessage(value: string) {
      const textarea = screen.getByLabelText('メッセージ') as HTMLTextAreaElement
      fireEvent.change(textarea, { target: { value } })
      return textarea
    }

    it('Enter 単体では送信せず、改行（既定動作）を妨げない', () => {
      render(<ChatThread roomId="room_1" gameId="game_1" />)
      const textarea = typeMessage('一行目')

      const notPrevented = fireEvent.keyDown(textarea, { key: 'Enter' })

      expect(notPrevented).toBe(true)
      expect(sendMock).not.toHaveBeenCalled()
    })

    it('Shift + Enter でも送信しない（改行のまま）', () => {
      render(<ChatThread roomId="room_1" gameId="game_1" />)
      const textarea = typeMessage('一行目')

      fireEvent.keyDown(textarea, { key: 'Enter', shiftKey: true })

      expect(sendMock).not.toHaveBeenCalled()
    })

    it.each([
      ['Ctrl + Enter', { ctrlKey: true }],
      ['⌘ + Enter', { metaKey: true }],
    ])('%s で trim した本文を送信し、入力欄を空にする', (_label, modifier) => {
      render(<ChatThread roomId="room_1" gameId="game_1" />)
      const textarea = typeMessage('  容疑者はあの人  ')

      fireEvent.keyDown(textarea, { key: 'Enter', ...modifier })

      expect(sendMock).toHaveBeenCalledWith('容疑者はあの人')
      expect(textarea.value).toBe('')
    })

    it('IME 変換中の Enter では送信しない', () => {
      render(<ChatThread roomId="room_1" gameId="game_1" />)
      const textarea = typeMessage('ようぎしゃ')

      fireEvent.keyDown(textarea, { key: 'Enter', ctrlKey: true, isComposing: true })

      expect(sendMock).not.toHaveBeenCalled()
    })

    it('接続が ready でなければショートカットでも送信しない', () => {
      hookValue.status = 'error'
      render(<ChatThread roomId="room_1" gameId="game_1" />)
      const textarea = typeMessage('あ')

      fireEvent.keyDown(textarea, { key: 'Enter', ctrlKey: true })

      expect(sendMock).not.toHaveBeenCalled()
    })

    it('送信・改行の操作方法を注釈として表示し、入力欄に紐づける', () => {
      render(<ChatThread roomId="room_1" gameId="game_1" />)

      expect(screen.getByLabelText('メッセージ')).toHaveAccessibleDescription(
        /Ctrl \+ Enter で送信・Enter で改行/,
      )
    })
  })
})
