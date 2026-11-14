// 画面01 ログイン — レーンB。UIは _docs/mock/src/pages/auth-pages.tsx から移植し、
// 送信処理を use-auth.ts 経由の実APIに差し替えた。
import { zodResolver } from '@hookform/resolvers/zod'
import { Eye, EyeOff, LockKeyhole, Mail } from 'lucide-react'
import { useState, type ReactNode } from 'react'
import { useForm } from 'react-hook-form'
import { Link, useLocation, useSearch } from 'wouter'
import { z } from 'zod'

import { GameMark } from '@/components/game-ui/game-mark'
import { MysteryHand } from '@/components/game-ui/mystery-hand'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { inviteReturnPath, withInviteReturn } from '@/lib/room-invite'
import { useAuth } from '@/hooks/use-auth'

const loginSchema = z.object({
  email: z.string().trim().email('有効なメールアドレスを入力してください。'),
  password: z.string().min(8, 'パスワードは8文字以上で入力してください。'),
})

type LoginValues = z.infer<typeof loginSchema>

export function LoginPage() {
  const [, navigate] = useLocation()
  const returnTo = inviteReturnPath(useSearch())
  const { login, isSubmitting, error } = useAuth()
  const [showPassword, setShowPassword] = useState(false)
  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<LoginValues>({ resolver: zodResolver(loginSchema) })

  const onSubmit = handleSubmit(async (values) => {
    const ok = await login(values.email, values.password)
    if (ok) navigate(returnTo ?? '/home')
  })

  return (
    <MobileShell>
      <main className="case-login min-h-full">
        <section className="case-login-hero">
          <div className="case-hero-scene" aria-hidden="true" />
          <div className="case-hero-top">
            <GameMark inverse />
            <span className="case-document-label">221B</span>
          </div>
          <div className="case-hero-copy">
            <p className="case-kicker">言葉と心理の推理ゲーム</p>
            <h1 className="text-balance">
              ひとりだけ、
              <br />
              言葉が違う。
            </h1>
            <p className="case-login-description">
              会話に紛れた人狼を見つける、正体隠匿ゲーム。
            </p>
          </div>
          <MysteryHand />
        </section>

        <form
          className="case-login-form space-y-5"
          onSubmit={onSubmit}
          noValidate
        >
          {error && (
            <p
              className="border-destructive/30 bg-destructive/10 text-destructive rounded-md border px-3 py-2 text-sm"
              role="alert"
            >
              {error}
            </p>
          )}

          <FormField
            label="メールアドレス"
            error={errors.email?.message}
            htmlFor="login-email"
          >
            <div className="relative">
              <Mail
                className="text-muted-foreground pointer-events-none absolute top-1/2 left-3 size-4 -translate-y-1/2"
                aria-hidden="true"
              />
              <Input
                id="login-email"
                className="bg-card h-12 pl-10"
                type="email"
                autoComplete="email"
                placeholder="you@example.com"
                aria-invalid={Boolean(errors.email)}
                aria-describedby={
                  errors.email ? 'login-email-error' : undefined
                }
                {...register('email')}
              />
            </div>
          </FormField>

          <FormField
            label="パスワード"
            error={errors.password?.message}
            htmlFor="login-password"
          >
            <div className="relative">
              <LockKeyhole
                className="text-muted-foreground pointer-events-none absolute top-1/2 left-3 size-4 -translate-y-1/2"
                aria-hidden="true"
              />
              <Input
                id="login-password"
                className="bg-card h-12 pr-12 pl-10"
                autoComplete="current-password"
                placeholder="8文字以上"
                type={showPassword ? 'text' : 'password'}
                aria-invalid={Boolean(errors.password)}
                aria-describedby={
                  errors.password ? 'login-password-error' : undefined
                }
                {...register('password')}
              />
              <Button
                type="button"
                aria-label={
                  showPassword ? 'パスワードを隠す' : 'パスワードを表示する'
                }
                className="absolute top-1/2 right-1.5 -translate-y-1/2"
                onClick={() => setShowPassword((current) => !current)}
                size="icon-sm"
                variant="ghost"
              >
                {showPassword ? (
                  <EyeOff className="size-4" aria-hidden="true" />
                ) : (
                  <Eye className="size-4" aria-hidden="true" />
                )}
              </Button>
            </div>
          </FormField>

          <div className="flex justify-end">
            <button
              className="text-primary text-sm font-medium underline-offset-4 hover:underline"
              type="button"
            >
              パスワードを忘れた方
            </button>
          </div>

          <Button className="h-12 w-full" type="submit" disabled={isSubmitting}>
            {isSubmitting ? 'ログイン中…' : 'ログイン'}
          </Button>
        </form>

        <p className="case-register-link text-muted-foreground text-center text-sm">
          アカウントをお持ちでない方{' '}
          <Link
            className="text-primary font-medium underline-offset-4 hover:underline"
            href={withInviteReturn('/register', returnTo)}
          >
            新規登録
          </Link>
        </p>

        <LegalLinks />
      </main>
    </MobileShell>
  )
}

function LegalLinks() {
  return (
    <footer className="case-legal text-muted-foreground mt-auto text-center text-xs">
      <Link className="underline underline-offset-4" href="/privacy">
        プライバシー
      </Link>
      <span aria-hidden="true"> · </span>
      <Link className="underline underline-offset-4" href="/terms">
        利用規約
      </Link>
      <span aria-hidden="true"> · </span>
      <a href="/credits.txt">素材クレジット</a>
    </footer>
  )
}

type FormFieldProps = {
  label: string
  error?: string
  htmlFor: string
  children: ReactNode
}

function FormField({ label, error, htmlFor, children }: FormFieldProps) {
  return (
    <div>
      <Label htmlFor={htmlFor}>{label}</Label>
      <div className="mt-2">{children}</div>
      <p
        id={`${htmlFor}-error`}
        className="form-field-message text-destructive mt-2 text-xs"
        role={error ? 'alert' : undefined}
      >
        {error || '\u00a0'}
      </p>
    </div>
  )
}
