import { fireEvent, render, screen, waitFor } from '@testing-library/react'
import { beforeEach, describe, expect, it } from 'vitest'

import { mockScreenGroups } from '@/data/mock-data'
import { GAME_START_STORAGE_KEY } from '@/lib/game-start-channel'

import App from './App'

function open(path: string) {
  window.history.pushState({}, '', path)
  render(<App />)
}

describe('mobile word-wolf mocks', () => {
  beforeEach(() => {
    window.history.pushState({}, '', '/login')
  })

  it('starts with the email and password login screen', () => {
    open('/login')

    expect(screen.getByRole('heading', { name: '捜査を再開する' })).toBeInTheDocument()
    expect(screen.getByLabelText('メールアドレス')).toHaveAttribute('autocomplete', 'email')
    expect(screen.getByLabelText('パスワード')).toHaveAttribute('autocomplete', 'current-password')
    expect(screen.getByRole('link', { name: '新規登録' })).toHaveAttribute('href', '/register')
  })

  it('shows inline validation and moves a valid mock login to home', async () => {
    open('/login')

    fireEvent.click(screen.getByRole('button', { name: 'ログイン' }))
    expect(await screen.findByText('有効なメールアドレスを入力してください。')).toBeInTheDocument()
    expect(screen.getByText('パスワードは8文字以上で入力してください。')).toBeInTheDocument()

    fireEvent.change(screen.getByLabelText('メールアドレス'), { target: { value: 'player@example.com' } })
    fireEvent.change(screen.getByLabelText('パスワード'), { target: { value: 'password123' } })
    fireEvent.click(screen.getByRole('button', { name: 'ログイン' }))

    expect(await screen.findByRole('heading', { name: '次の事件を始めよう' })).toBeInTheDocument()
  })

  it('searches a private room by invite URL instead of listing public rooms', () => {
    open('/rooms')

    expect(screen.getByRole('heading', { name: 'URLで参加' })).toBeInTheDocument()
    expect(screen.queryByText('参加できる')).not.toBeInTheDocument()

    fireEvent.change(screen.getByLabelText('招待URL'), { target: { value: 'not-a-url' } })
    fireEvent.click(screen.getByRole('button', { name: '確認' }))
    expect(screen.getByText('有効な招待URLを入力してください。')).toBeInTheDocument()

    fireEvent.change(screen.getByLabelText('招待URL'), { target: { value: 'https://game.example.com/join/CASE-042' } })
    fireEvent.click(screen.getByRole('button', { name: '確認' }))
    expect(screen.getByRole('heading', { name: '深夜の読書会' })).toBeInTheDocument()
    expect(screen.getByRole('button', { name: '参加' })).toBeEnabled()
    expect(screen.getByRole('link', { name: '観戦' })).toHaveAttribute('href', '/game/spectator')
  })

  it('opens a shared invite URL directly with the room preview', () => {
    open('/join/CASE-042')

    expect(screen.getByDisplayValue(/\/join\/CASE-042$/)).toBeInTheDocument()
    expect(screen.getByRole('heading', { name: '深夜の読書会' })).toBeInTheDocument()
  })

  it('lets the player choose a character avatar before starting', () => {
    open('/avatar/setup')

    const avatarChoices = screen.getAllByRole('button', { name: /のアバターを選択/ })
    expect(avatarChoices).toHaveLength(14)
    expect(screen.getByRole('heading', { name: 'ひと' })).toBeInTheDocument()
    expect(screen.getByRole('heading', { name: 'どうぶつ' })).toBeInTheDocument()
    expect(screen.getByRole('button', { name: 'ねこのアバターを選択' })).toHaveAttribute('aria-pressed', 'true')

    fireEvent.click(screen.getByRole('button', { name: 'ベテランのアバターを選択' }))
    expect(screen.getByRole('button', { name: 'ベテランのアバターを選択' })).toHaveAttribute('aria-pressed', 'true')
    expect(screen.getByRole('link', { name: 'このアバターではじめる' })).toHaveAttribute('href', '/home')
  })

  it('supports multiple topic genres and validates that one remains selected', () => {
    open('/rooms/new')

    const genreChoices = screen.getAllByRole('checkbox')
    expect(genreChoices).toHaveLength(6)
    expect(genreChoices.filter((choice) => choice.getAttribute('aria-checked') === 'true')).toHaveLength(3)

    fireEvent.click(screen.getByRole('checkbox', { name: /^場所ジャンル/ }))
    fireEvent.click(screen.getByRole('checkbox', { name: /^食べ物ジャンル/ }))
    fireEvent.click(screen.getByRole('checkbox', { name: /^趣味ジャンル/ }))
    fireEvent.click(screen.getByRole('button', { name: '作成' }))
    expect(screen.getByText('お題ジャンルを1つ以上選択してください。')).toBeInTheDocument()

    fireEvent.click(screen.getByRole('checkbox', { name: /^エンタメジャンル/ }))
    fireEvent.change(screen.getByLabelText('お題の近さ'), { target: { value: 'むずかしい' } })
    expect(screen.getByRole('checkbox', { name: /^エンタメジャンル/ })).toHaveAttribute('aria-checked', 'true')
    expect(screen.queryByText('1種類')).not.toBeInTheDocument()
    expect(screen.getByLabelText('お題の近さ')).toHaveValue('むずかしい')
  })

  it('toggles readiness with a single button in the lobby', () => {
    open('/lobby')

    expect(screen.getByRole('link', { name: '設定を変更' })).toHaveAttribute('href', '/rooms/new')
    const readyButton = screen.getByRole('button', { name: '準備完了' })
    expect(readyButton).toHaveAttribute('aria-pressed', 'true')
    expect(screen.queryByText('自分の準備状態')).not.toBeInTheDocument()
    expect(screen.queryByText('全員が準備すると開始できます')).not.toBeInTheDocument()

    fireEvent.click(readyButton)
    expect(screen.getByRole('button', { name: '準備する' })).toHaveAttribute('aria-pressed', 'false')
  })

  it('receives a host game-start event in another player mock', async () => {
    open('/lobby/player/wolf')

    expect(screen.getByText('ホストの開始を待っています')).toBeInTheDocument()
    const signal = { type: 'game-started', id: 'test-start', startsAt: Date.now() + 1000 }
    fireEvent(window, new StorageEvent('storage', { key: GAME_START_STORAGE_KEY, newValue: JSON.stringify(signal) }))

    expect(await screen.findByRole('timer', { name: 'ゲーム開始まで3秒' })).toBeInTheDocument()
  })

  it('publishes a game-start event from the host mock', async () => {
    open('/lobby')

    fireEvent.click(screen.getByRole('button', { name: 'ゲームを開始する' }))
    const storedSignal = JSON.parse(window.localStorage.getItem(GAME_START_STORAGE_KEY) ?? '{}')

    expect(storedSignal).toMatchObject({ type: 'game-started' })
    expect(await screen.findByRole('timer', { name: 'ゲーム開始まで3秒' })).toBeInTheDocument()
  })

  it('confirms account deletion and returns to login without exposing data export', async () => {
    open('/profile')

    expect(screen.queryByText('データをエクスポート')).not.toBeInTheDocument()
    fireEvent.click(screen.getByRole('button', { name: 'アカウント削除' }))
    expect(screen.getByRole('alertdialog')).toBeInTheDocument()
    expect(screen.getByRole('heading', { name: 'アカウントを削除しますか？' })).toBeInTheDocument()

    fireEvent.click(screen.getByRole('button', { name: 'アカウントを削除' }))
    expect(await screen.findByRole('heading', { name: '捜査を再開する' })).toBeInTheDocument()
  })

  it('keeps the citizen word private and supports the discussion chat', () => {
    open('/game/citizen')

    expect(screen.getByText('図書館')).toBeInTheDocument()
    expect(screen.queryByText('書店')).not.toBeInTheDocument()
    expect(screen.getByText('自分が多数派か少数派かは、結果発表まで分かりません。')).toBeInTheDocument()

    fireEvent.click(screen.getByRole('button', { name: 'お題を隠す' }))
    expect(screen.queryByText('図書館')).not.toBeInTheDocument()
    expect(screen.getByText('周囲を確認してから表示してください')).toBeInTheDocument()

    fireEvent.change(screen.getByLabelText('メッセージ'), { target: { value: 'そこで勉強することはありますか？' } })
    fireEvent.click(screen.getByRole('button', { name: '送信' }))
    expect(screen.getByText('そこで勉強することはありますか？')).toBeInTheDocument()
  })

  it('shows only the minority word in the word-wolf viewpoint', () => {
    open('/game/wolf')

    expect(screen.getByText('書店')).toBeInTheDocument()
    expect(screen.queryByText('図書館')).not.toBeInTheDocument()
    expect(screen.queryByText('あなたはワードウルフ')).not.toBeInTheDocument()
  })

  it('keeps the assigned word covered until the player deliberately reveals it', () => {
    open('/game/reveal/citizen')

    expect(screen.queryByText('図書館')).not.toBeInTheDocument()
    expect(screen.getByRole('button', { name: 'お題を確認してください' })).toBeDisabled()

    fireEvent.click(screen.getByRole('button', { name: 'お題を表示する' }))
    expect(screen.getByText('図書館')).toBeInTheDocument()
    expect(screen.getByRole('link', { name: '確認して議論へ' })).toHaveAttribute('href', '/game/citizen')
  })

  it('gives spectators public information without a word or chat input', () => {
    open('/game/spectator')

    expect(screen.getByRole('heading', { name: '秘密情報は表示されません' })).toBeInTheDocument()
    expect(screen.queryByText('図書館')).not.toBeInTheDocument()
    expect(screen.queryByText('書店')).not.toBeInTheDocument()
    expect(screen.getByText('観戦中は閲覧のみです')).toBeInTheDocument()
    expect(screen.queryByLabelText('メッセージ')).not.toBeInTheDocument()
  })

  it('requires confirmation before sending an irreversible vote', async () => {
    open('/game/vote')

    fireEvent.click(screen.getByRole('button', { name: /ミナト/ }))
    fireEvent.click(screen.getByRole('button', { name: 'ミナトに投票する' }))

    expect(await screen.findByRole('alertdialog')).toBeInTheDocument()
    expect(screen.getByRole('heading', { name: 'ミナトで確定しますか？' })).toBeInTheDocument()

    fireEvent.click(screen.getByRole('button', { name: '投票を確定' }))
    await waitFor(() => expect(screen.getByRole('heading', { name: '市民側の勝利' })).toBeInTheDocument())
  })

  it('publishes both words and all roles only on the result screen', () => {
    open('/game/result')

    expect(screen.getByText('図書館')).toBeInTheDocument()
    expect(screen.getByText('書店')).toBeInTheDocument()
    expect(screen.getAllByText('市民')).toHaveLength(5)
    expect(screen.getAllByText('ワードウルフ')).toHaveLength(1)
  })

  it('provides a direct link catalog for independently sharing every mock', () => {
    open('/mocks')

    expect(screen.getByRole('heading', { name: '画面モック一覧' })).toBeInTheDocument()
    expect(screen.getByRole('link', { name: /ログイン/ })).toHaveAttribute('href', '/login')
    expect(screen.getByRole('link', { name: /観戦者視点/ })).toHaveAttribute('href', '/game/spectator')
    expect(screen.getByRole('link', { name: /結果発表/ })).toHaveAttribute('href', '/game/result')
    expect(screen.getByRole('link', { name: /プライバシー/ })).toHaveAttribute('href', '/privacy')
  })

  it('renders every catalog entry as a direct route', () => {
    for (const { screens } of mockScreenGroups) {
      for (const mock of screens) {
        window.history.pushState({}, '', mock.href)
        const view = render(<App />)
        expect(screen.queryByText('指定されたモックはありません')).not.toBeInTheDocument()
        view.unmount()
      }
    }
  })
})
