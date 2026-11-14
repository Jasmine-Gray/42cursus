import { act, cleanup, fireEvent, render, screen } from '@testing-library/react'
import { afterEach, beforeEach, expect, it, vi } from 'vitest'
import { ComebackPanel } from './comeback-panel'
const players = [
  {
    userId: 'wolf',
    displayName: 'ウルフさん',
    avatarUrl: '/avatars/yu.webp',
    ready: true,
    connected: true,
  },
]
const comeback = { gameId: 'game', wolfUserId: 'wolf', content: '' }
const updateComebackDraft = vi.fn()
const submitComeback = vi.fn()
const props = {
  players,
  comeback,
  updateComebackDraft,
  submitComeback,
  status: 'ready' as const,
  sendError: null,
}
beforeEach(() => {
  vi.clearAllMocks()
  vi.useFakeTimers()
  submitComeback.mockResolvedValue(true)
})
afterEach(() => {
  cleanup()
  vi.useRealTimers()
})
it('打ち直した最新の下書きだけを配信し、確定時は最新文字を送る', async () => {
  render(<ComebackPanel {...props} meId="wolf" />)
  const input = screen.getByLabelText('市民側のお題を入力')
  fireEvent.change(input, { target: { value: 'パ' } })
  fireEvent.change(input, { target: { value: 'パル' } })
  act(() => vi.advanceTimersByTime(180))
  expect(updateComebackDraft).toHaveBeenCalledExactlyOnceWith('パル')
  fireEvent.change(input, { target: { value: 'パルム' } })
  await act(async () => {
    fireEvent.click(screen.getByRole('button', { name: 'この回答で勝負する' }))
  })
  expect(submitComeback).toHaveBeenCalledExactlyOnceWith('パルム')
  act(() => vi.advanceTimersByTime(180))
  expect(updateComebackDraft).toHaveBeenCalledTimes(1)
  expect(screen.getByRole('button', { name: '回答しました' })).toBeDisabled()
})
it('IME変換中のEnterで回答を確定しない', () => {
  render(<ComebackPanel {...props} meId="wolf" />)
  const input = screen.getByLabelText('市民側のお題を入力')
  fireEvent.compositionStart(input)
  fireEvent.change(input, { target: { value: 'は' } })
  fireEvent.submit(input.closest('form')!)
  expect(submitComeback).not.toHaveBeenCalled()
})
it('他の参加者は入力文字をリアルタイムで見られるが回答できない', () => {
  const view = render(<ComebackPanel {...props} meId="citizen" />)
  expect(screen.queryByRole('textbox')).not.toBeInTheDocument()
  view.rerender(
    <ComebackPanel
      {...props}
      meId="citizen"
      comeback={{ ...comeback, content: '<script>実行しない</script>' }}
    />,
  )
  expect(screen.getByRole('status')).toHaveTextContent(
    '<script>実行しない</script>',
  )
  expect(document.querySelector('script')).toBeNull()
})
it('再接続時は公開状態の下書きを復元する', () => {
  render(
    <ComebackPanel
      {...props}
      meId="wolf"
      comeback={{ ...comeback, content: '書きかけ' }}
    />,
  )
  expect(screen.getByLabelText('市民側のお題を入力')).toHaveValue('書きかけ')
})
