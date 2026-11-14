import { act, render, screen } from '@testing-library/react'
import { afterEach, describe, expect, it } from 'vitest'
import { Route, Router, Switch } from 'wouter'
import { memoryLocation } from 'wouter/memory-location'

import { setAuthToken } from '@/lib/api-client'

import { AuthGate } from './auth-gate'

function renderAt(path: string) {
  const location = memoryLocation({ path, record: true })
  render(
    <Router hook={location.hook}>
      <AuthGate>
        <Switch>
          <Route path="/login">ログイン画面</Route>
          <Route path="/register">登録画面</Route>
          <Route path="/join/:code">招待画面</Route>
          <Route path="/home">ホーム画面</Route>
          <Route path="/friends">フレンド画面</Route>
          <Route path="/privacy">プライバシー画面</Route>
          <Route>404画面</Route>
        </Switch>
      </AuthGate>
    </Router>,
  )
  return location
}

afterEach(() => {
  localStorage.clear()
  sessionStorage.clear()
})

describe('AuthGate', () => {
  describe('セッションなし', () => {
    it.each(['/home', '/friends', '/unknown'])(
      '%s は /login へ戻す（履歴は置き換え）',
      (path) => {
        const location = renderAt(path)

        expect(screen.getByText('ログイン画面')).toBeInTheDocument()
        expect(location.history).toEqual(['/login'])
      },
    )

    it.each([
      ['/login', 'ログイン画面'],
      ['/register', '登録画面'],
      ['/privacy', 'プライバシー画面'],
    ])('%s はそのまま表示する', (path, text) => {
      renderAt(path)

      expect(screen.getByText(text)).toBeInTheDocument()
    })
  })

  describe('セッションあり', () => {
    it.each(['/login', '/register'])(
      '%s は /home へ戻す（履歴は置き換え）',
      (path) => {
        setAuthToken('tok-1')
        const location = renderAt(path)

        expect(screen.getByText('ホーム画面')).toBeInTheDocument()
        expect(location.history).toEqual(['/home'])
      },
    )

    it.each([
      ['/friends', 'フレンド画面'],
      ['/privacy', 'プライバシー画面'],
      ['/unknown', '404画面'],
    ])('%s はそのまま表示する', (path, text) => {
      setAuthToken('tok-1')
      renderAt(path)

      expect(screen.getByText(text)).toBeInTheDocument()
    })

    it('表示中にセッションが終わる（ログアウト等）と /login へ戻す', () => {
      setAuthToken('tok-1')
      renderAt('/friends')
      expect(screen.getByText('フレンド画面')).toBeInTheDocument()

      act(() => setAuthToken(null))

      expect(screen.getByText('ログイン画面')).toBeInTheDocument()
    })
  })

  it('ログイン直後（トークンが増えた時）は再描画で /home へ飛ばさない（遷移先は各画面が決める）', () => {
    renderAt('/register')

    act(() => setAuthToken('tok-new'))

    expect(screen.getByText('登録画面')).toBeInTheDocument()
  })
})

it('未ログインの観戦招待はログイン後の戻り先に保持する', () => {
  const location = renderAt('/join/CASE-042?mode=spectate')
  expect(screen.getByText('ログイン画面')).toBeInTheDocument()
  expect(location.history).toEqual([
    '/login?returnTo=%2Fjoin%2FCASE-042%3Fmode%3Dspectate',
  ])
})

it('ログイン済みで戻り先つきの認証画面を開くと招待へ戻る', () => {
  setAuthToken('tok-1')
  const location = renderAt(
    '/login?returnTo=%2Fjoin%2FCASE-042%3Fmode%3Dspectate',
  )
  expect(screen.getByText('招待画面')).toBeInTheDocument()
  expect(location.history).toEqual(['/join/CASE-042?mode=spectate'])
})
