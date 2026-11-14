import { fireEvent, render, screen } from '@testing-library/react'
import type { ReactNode } from 'react'
import { describe, expect, it, vi } from 'vitest'

import { RegisterPage } from './register-page'

vi.mock('wouter', () => ({
  Link: ({
    href,
    children,
    className,
  }: {
    href: string
    children: ReactNode
    className?: string
  }) => (
    <a href={href} className={className}>
      {children}
    </a>
  ),
  useSearch: () => '',
  useLocation: () => ['/register', vi.fn()],
}))

vi.mock('@/hooks/use-auth', () => ({
  useAuth: () => ({ register: vi.fn(), isSubmitting: false, error: null }),
}))

vi.mock('@/components/layout/mobile-shell', () => ({
  MobileShell: ({ children }: { children: ReactNode }) => <>{children}</>,
}))

describe('RegisterPage', () => {
  it('パスワード欄のプレースホルダーは入力の案内にする', () => {
    render(<RegisterPage />)

    expect(screen.getByLabelText('パスワード')).toHaveAttribute(
      'placeholder',
      'パスワードを入力してください',
    )
  })

  it('8文字未満で送信すると入力欄の下にエラー文を出し、入力欄の説明にする', async () => {
    render(<RegisterPage />)

    const password = screen.getByLabelText('パスワード')
    fireEvent.input(password, { target: { value: 'abc' } })
    fireEvent.click(screen.getByRole('button', { name: '登録してはじめる' }))

    expect(
      await screen.findByText('パスワードは8文字以上で入力してください。'),
    ).toHaveAttribute('role', 'alert')
    expect(password).toHaveAccessibleDescription(
      'パスワードは8文字以上で入力してください。',
    )
  })
})
