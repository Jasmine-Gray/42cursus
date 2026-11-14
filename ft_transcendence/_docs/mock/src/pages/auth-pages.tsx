import { zodResolver } from '@hookform/resolvers/zod'
import { Eye, EyeOff, LockKeyhole, Mail } from 'lucide-react'
import { useState, type ReactNode } from 'react'
import { Controller, useForm } from 'react-hook-form'
import { Link, useLocation } from 'wouter'
import { z } from 'zod'

import { GameMark } from '@/components/game-ui/game-mark'
import { MysteryHand } from '@/components/game-ui/mystery-hand'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Button, buttonVariants } from '@/components/ui/button'
import { Checkbox } from '@/components/ui/checkbox'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { cn } from '@/lib/utils'

const loginSchema = z.object({
  email: z.string().trim().email('有効なメールアドレスを入力してください。'),
  password: z.string().min(8, 'パスワードは8文字以上で入力してください。'),
})

const registerSchema = z
  .object({
    displayName: z
      .string()
      .trim()
      .min(2, '表示名は2文字以上で入力してください。')
      .max(24, '表示名は24文字以内で入力してください。'),
    email: z.string().trim().email('有効なメールアドレスを入力してください。'),
    password: z.string().min(8, 'パスワードは8文字以上で入力してください。'),
    confirmPassword: z.string(),
    terms: z
      .boolean()
      .refine(Boolean, '利用規約とプライバシーポリシーへの同意が必要です。'),
  })
  .refine((values) => values.password === values.confirmPassword, {
    message: '確認用パスワードが一致しません。',
    path: ['confirmPassword'],
  })

type LoginValues = z.infer<typeof loginSchema>
type RegisterValues = z.infer<typeof registerSchema>

export function LoginPage() {
  const [, navigate] = useLocation()
  const [showPassword, setShowPassword] = useState(false)
  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<LoginValues>({ resolver: zodResolver(loginSchema) })

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
            <h1 className="text-balance">ひとりだけ、<br />言葉が違う。</h1>
            <p className="case-login-description">
              会話に紛れた人狼を見つける、正体隠匿ゲーム。
            </p>
          </div>
          <MysteryHand />
        </section>

        <form
          className="case-login-form space-y-5"
          onSubmit={handleSubmit(() => navigate('/home'))}
          noValidate
        >
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

          <Button className="h-12 w-full" type="submit">
            ログイン
          </Button>
        </form>

        <p className="case-register-link text-muted-foreground text-center text-sm">
          アカウントをお持ちでない方{' '}
          <Link
            className="text-primary font-medium underline-offset-4 hover:underline"
            href="/register"
          >
            新規登録
          </Link>
        </p>

        <LegalLinks />
      </main>
    </MobileShell>
  )
}

export function RegisterPage() {
  const [, navigate] = useLocation()
  const {
    control,
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<RegisterValues>({
    resolver: zodResolver(registerSchema),
    defaultValues: { terms: false },
  })

  return (
    <MobileShell>
      <main className="case-registration">
        <section className="case-login-hero">
          <div className="case-hero-scene" aria-hidden="true" />
          <div className="case-hero-top"><GameMark inverse /></div>
          <div className="case-hero-copy">
            <p className="case-kicker">言葉と心理の推理ゲーム</p>
            <h1 className="text-balance">その一言が、<br />手がかりになる。</h1>
            <p className="case-login-description text-pretty">仲間との会話から、隠れた人狼を見つけよう。</p>
          </div>
          <MysteryHand />
        </section>
        <div className="case-registration-form">
          <Link href="/login" className="text-muted-foreground text-sm hover:text-foreground">← ログインへ戻る</Link>
          <h2 className="font-heading mt-8 text-3xl font-semibold text-balance">アカウント作成</h2>
          <p className="text-muted-foreground mt-2 text-sm leading-6 text-pretty">
            表示名はゲーム中の全参加者と観戦者に公開されます。
          </p>

          <form
            className="mt-6 space-y-5"
            onSubmit={handleSubmit(() => navigate('/avatar/setup'))}
            noValidate
          >
            <FormField
              label="表示名"
              error={errors.displayName?.message}
              htmlFor="register-name"
            >
              <Input
                id="register-name"
                className="h-11"
                autoComplete="nickname"
                placeholder="田中 ユウ"
                aria-invalid={Boolean(errors.displayName)}
                aria-describedby={
                  errors.displayName ? 'register-name-error' : undefined
                }
                {...register('displayName')}
              />
            </FormField>
            <FormField
              label="メールアドレス"
              error={errors.email?.message}
              htmlFor="register-email"
            >
              <Input
                id="register-email"
                className="h-11"
                type="email"
                autoComplete="email"
                placeholder="you@example.com"
                aria-invalid={Boolean(errors.email)}
                aria-describedby={
                  errors.email ? 'register-email-error' : undefined
                }
                {...register('email')}
              />
            </FormField>
            <FormField
              label="パスワード"
              error={errors.password?.message}
              htmlFor="register-password"
            >
              <Input
                id="register-password"
                className="h-11"
                autoComplete="new-password"
                placeholder="8文字以上"
                type="password"
                aria-invalid={Boolean(errors.password)}
                aria-describedby={
                  errors.password ? 'register-password-error' : undefined
                }
                {...register('password')}
              />
            </FormField>
            <FormField
              label="パスワード（確認）"
              error={errors.confirmPassword?.message}
              htmlFor="register-password-confirm"
            >
              <Input
                id="register-password-confirm"
                className="h-11"
                autoComplete="new-password"
                placeholder="もう一度入力"
                type="password"
                aria-invalid={Boolean(errors.confirmPassword)}
                aria-describedby={
                  errors.confirmPassword
                    ? 'register-password-confirm-error'
                    : undefined
                }
                {...register('confirmPassword')}
              />
            </FormField>

            <div>
              <Controller
                control={control}
                name="terms"
                render={({ field }) => (
                  <div className="flex items-start gap-3">
                    <Checkbox
                      id="register-terms"
                      checked={field.value}
                      onCheckedChange={field.onChange}
                      aria-invalid={Boolean(errors.terms)}
                      aria-describedby={
                        errors.terms ? 'register-terms-error' : undefined
                      }
                    />
                    <Label
                      htmlFor="register-terms"
                      className="text-sm leading-5 font-normal text-pretty"
                    >
                      <Link
                        className="text-primary font-medium underline"
                        href="/terms"
                      >
                        利用規約
                      </Link>
                      と
                      <Link
                        className="text-primary font-medium underline"
                        href="/privacy"
                      >
                        プライバシーポリシー
                      </Link>
                      に同意します
                    </Label>
                  </div>
                )}
              />
              <p id="register-terms-error" className="form-field-message text-destructive mt-2 text-xs" role={errors.terms?.message ? 'alert' : undefined}>
                  {errors.terms?.message || ' '}
                </p>
            </div>

            <Button className="h-11 w-full" type="submit">
              登録してはじめる
            </Button>
          </form>
        </div>
      </main>
    </MobileShell>
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
      <Link
        className={cn(
          buttonVariants({ variant: 'link', size: 'xs' }),
          'h-auto p-0 text-xs',
        )}
        href="/mocks"
      >
        画面モック一覧
      </Link>
      <span aria-hidden="true"> · </span><a href="/credits.txt">素材クレジット</a>
    </footer>
  )
}
