// 画面02 新規登録 — レーンB。UIは _docs/mock/src/pages/auth-pages.tsx から移植し、
// 送信処理を use-auth.ts 経由の実APIに差し替えた。
import { zodResolver } from '@hookform/resolvers/zod'
import { type ReactNode } from 'react'
import { Controller, useForm } from 'react-hook-form'
import { Link, useLocation, useSearch } from 'wouter'
import { z } from 'zod'

import { GameMark } from '@/components/game-ui/game-mark'
import { MysteryHand } from '@/components/game-ui/mystery-hand'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Button } from '@/components/ui/button'
import { Checkbox } from '@/components/ui/checkbox'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { inviteReturnPath, withInviteReturn } from '@/lib/room-invite'
import { useAuth } from '@/hooks/use-auth'

const PASSWORD_MIN_LENGTH = 8

const registerSchema = z
  .object({
    displayName: z
      .string()
      .trim()
      .min(2, '表示名は2文字以上で入力してください。')
      .max(24, '表示名は24文字以内で入力してください。'),
    email: z.string().trim().email('有効なメールアドレスを入力してください。'),
    password: z
      .string()
      .min(
        PASSWORD_MIN_LENGTH,
        `パスワードは${PASSWORD_MIN_LENGTH}文字以上で入力してください。`,
      ),
    confirmPassword: z.string(),
    terms: z
      .boolean()
      .refine(Boolean, '利用規約とプライバシーポリシーへの同意が必要です。'),
  })
  .refine((values) => values.password === values.confirmPassword, {
    message: '確認用パスワードが一致しません。',
    path: ['confirmPassword'],
  })

type RegisterValues = z.infer<typeof registerSchema>

export function RegisterPage() {
  const [, navigate] = useLocation()
  const returnTo = inviteReturnPath(useSearch())
  const { register: registerUser, isSubmitting, error } = useAuth()
  const {
    control,
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<RegisterValues>({
    resolver: zodResolver(registerSchema),
    defaultValues: { terms: false },
  })

  const onSubmit = handleSubmit(async (values) => {
    const ok = await registerUser({
      displayName: values.displayName,
      email: values.email,
      password: values.password,
      termsAccepted: values.terms,
    })
    if (ok) navigate(withInviteReturn('/avatar/setup', returnTo))
  })

  return (
    <MobileShell>
      <main className="case-registration">
        <section className="case-login-hero">
          <div className="case-hero-scene" aria-hidden="true" />
          <div className="case-hero-top">
            <GameMark inverse />
          </div>
          <div className="case-hero-copy">
            <p className="case-kicker">言葉と心理の推理ゲーム</p>
            <h1 className="text-balance">
              その一言が、
              <br />
              手がかりになる。
            </h1>
            <p className="case-login-description text-pretty">
              仲間との会話から、隠れた人狼を見つけよう。
            </p>
          </div>
          <MysteryHand />
        </section>
        <div className="case-registration-form">
          <Link
            href={withInviteReturn('/login', returnTo)}
            className="text-muted-foreground hover:text-foreground text-sm"
          >
            ← ログインへ戻る
          </Link>
          <h2 className="font-heading mt-8 text-3xl font-semibold text-balance">
            アカウント作成
          </h2>
          <p className="text-muted-foreground mt-2 text-sm leading-6 text-pretty">
            表示名はゲーム中の全参加者と観戦者に公開されます。
          </p>

          <form className="mt-6 space-y-5" onSubmit={onSubmit} noValidate>
            {error && (
              <p
                className="border-destructive/30 bg-destructive/10 text-destructive rounded-md border px-3 py-2 text-sm"
                role="alert"
              >
                {error}
              </p>
            )}

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
                  errors.displayName ? 'register-name-message' : undefined
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
                  errors.email ? 'register-email-message' : undefined
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
                placeholder="パスワードを入力してください"
                type="password"
                aria-invalid={Boolean(errors.password)}
                aria-describedby={
                  errors.password ? 'register-password-message' : undefined
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
                    ? 'register-password-confirm-message'
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
                    {/* Label は既定で flex。文とリンクが別々の列に折り返すので block で1つの文として流す */}
                    <Label
                      htmlFor="register-terms"
                      className="block text-sm leading-5 font-normal text-pretty"
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
              <p
                id="register-terms-error"
                className="form-field-message text-destructive mt-2 text-xs"
                role={errors.terms?.message ? 'alert' : undefined}
              >
                {errors.terms?.message || ' '}
              </p>
            </div>

            <Button
              className="h-11 w-full"
              type="submit"
              disabled={isSubmitting}
            >
              {isSubmitting ? '登録中…' : '登録してはじめる'}
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

// 入力欄の下の1行（id: `${htmlFor}-message`）はエラーか空行を出し、
// 高さを常に確保してエラーの出入りでフォームがずれないようにする。
function FormField({ label, error, htmlFor, children }: FormFieldProps) {
  return (
    <div>
      <Label htmlFor={htmlFor}>{label}</Label>
      <div className="mt-2">{children}</div>
      <p
        id={`${htmlFor}-message`}
        className="form-field-message text-destructive mt-2 text-xs"
        role={error ? 'alert' : undefined}
      >
        {error || '\u00a0'}
      </p>
    </div>
  )
}
