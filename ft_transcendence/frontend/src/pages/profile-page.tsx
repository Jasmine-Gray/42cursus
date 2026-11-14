import { Camera, CircleCheck, CircleX, LogOut, Trash2 } from 'lucide-react'
import { useState, type ChangeEvent, type FormEvent, useEffect } from 'react'
import { Link, useLocation } from 'wouter'

import { AvatarPicker } from '@/components/game-ui/avatar-picker'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { SectionHeader } from '@/components/game-ui/section-header'
import { StatsStrip } from '@/components/game-ui/stats-strip'
import { MobileShell } from '@/components/layout/mobile-shell'
import {
  AlertDialog,
  AlertDialogAction,
  AlertDialogCancel,
  AlertDialogContent,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogHeader,
  AlertDialogMedia,
  AlertDialogTitle,
  AlertDialogTrigger,
} from '@/components/ui/alert-dialog'
import { Badge } from '@/components/ui/badge'
import { Button, buttonVariants } from '@/components/ui/button'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { Separator } from '@/components/ui/separator'
import { avatarOptions, DEFAULT_AVATAR_URL } from '@/data/avatar-options'
import { useAuth } from '@/hooks/use-auth'
import { useMatchHistory, type MatchHistoryItem } from '@/hooks/use-match-history'
import { getGlobalMeUser, useMe } from '@/hooks/use-me'
import { cn } from '@/lib/utils'

export function ProfilePage() {
  const [profileView, setProfileView] = useState<"profile" | "stats">("profile")
  const [, navigate] = useLocation()
  const { user, stats, refreshMe, updateAvatarPreset, uploadAvatarFile, updateProfile, deleteAccount } = useMe()
  const history = useMatchHistory(RECENT_MATCH_LIMIT)

  // 戦績（stats）は useMe の共有キャッシュで、ログイン中は取り直されない。
  // ゲームを終えてから開いたときに古い数字を出さないよう、開くたびに取り直す
  useEffect(() => {
    void refreshMe()
  }, [refreshMe])
  const { logout } = useAuth()
  const [isLoggingOut, setIsLoggingOut] = useState(false)
  const [isDeletingAccount, setIsDeletingAccount] = useState(false)
  const [deleteAccountError, setDeleteAccountError] = useState('')

  const [selectedAvatar, setSelectedAvatar] = useState(
    () => getGlobalMeUser()?.avatarUrl || user?.avatarUrl || DEFAULT_AVATAR_URL,
  )
  const [profileName, setProfileName] = useState(() => getGlobalMeUser()?.displayName || user?.displayName || '')
  const [bio, setBio] = useState(() => getGlobalMeUser()?.bio || user?.bio || '')
  const [profileError, setProfileError] = useState('')
  const [profileStatus, setProfileStatus] = useState('')
  const [avatarFeedback, setAvatarFeedback] = useState<{ kind: 'error' | 'success'; text: string } | null>(null)
  const [isSavingProfile, setIsSavingProfile] = useState(false)

  useEffect(() => {
    if (user) {
      setSelectedAvatar(user.avatarUrl)
      setProfileName(user.displayName)
      setBio(user.bio || '')
    }
  }, [user])

  async function handleAvatarPresetSelect(src: string) {
    setSelectedAvatar(src)
    const option = avatarOptions.find((opt) => opt.src === src)
    const presetId = option ? option.id : src

    const result = await updateAvatarPreset(presetId)
    if (result.success) {
      setAvatarFeedback({ kind: 'success', text: 'アバタープリセットを更新しました。' })
    } else {
      setAvatarFeedback({ kind: 'error', text: result.error || 'アバターの更新に失敗しました。' })
    }
  }

  async function handleAvatarUpload(event: ChangeEvent<HTMLInputElement>) {
    const file = event.target.files?.[0]
    if (!file) return

    if (!['image/png', 'image/jpeg', 'image/webp'].includes(file.type)) {
      setAvatarFeedback({ kind: 'error', text: 'PNG・JPEG・WebP形式の画像を選択してください。' })
      return
    }

    if (file.size > 5 * 1024 * 1024) {
      setAvatarFeedback({ kind: 'error', text: '画像は5MB以下にしてください。' })
      return
    }

    const result = await uploadAvatarFile(file)
    if (result.success) {
      if (result.user?.avatarUrl) {
        setSelectedAvatar(result.user.avatarUrl)
      }
      setAvatarFeedback({ kind: 'success', text: 'プロフィール画像をアップロードしました。' })
    } else {
      setAvatarFeedback({ kind: 'error', text: result.error || 'アップロードに失敗しました。' })
    }
  }

  async function handleProfileSubmit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    if (profileName.trim().length < 2) {
      setProfileError('表示名は2文字以上で入力してください。')
      setProfileStatus('')
      return
    }
    if (profileName.trim().length > 24) {
      setProfileError('表示名は24文字以内で入力してください。')
      setProfileStatus('')
      return
    }

    setIsSavingProfile(true)
    setProfileError('')
    setProfileStatus('')

    const result = await updateProfile(profileName.trim(), bio.trim())
    setIsSavingProfile(false)

    if (result.success) {
      setProfileStatus('プロフィールを保存しました。')
    } else {
      setProfileError(result.error || 'プロフィールの保存に失敗しました。')
    }
  }

  async function handleLogout() {
    setIsLoggingOut(true)
    await logout()
    navigate('/login')
  }

  async function handleDeleteAccount() {
    setIsDeletingAccount(true)
    setDeleteAccountError('')

    const result = await deleteAccount()

    if (result.success) {
      navigate('/login')
    } else {
      setIsDeletingAccount(false)
      setDeleteAccountError(result.error)
    }
  }

  const gamesPlayed = stats?.gamesPlayed ?? 0
  const wins = stats?.wins ?? 0
  const winRate = gamesPlayed > 0 ? `${Math.round((wins / gamesPlayed) * 100)}%` : '-'

  const displayName = user?.displayName || profileName || '田中 ユウ'
  const initials = displayName.slice(0, 2).toUpperCase()
  const currentAvatar = selectedAvatar || user?.avatarUrl || DEFAULT_AVATAR_URL

  return (
    <MobileShell title="プロフィール" bottomNav>
      <main className="profile-screen space-y-6 px-5 py-5">
        <div className="profile-view-switch" aria-label="表示内容">
          <Button variant={profileView === 'profile' ? 'default' : 'outline'} aria-pressed={profileView === 'profile'} onClick={() => setProfileView('profile')}>プロフィール</Button>
          <Button variant={profileView === 'stats' ? 'default' : 'outline'} aria-pressed={profileView === 'stats'} onClick={() => setProfileView('stats')}>戦績</Button>
        </div>
        <section className="text-center">
          <PlayerAvatar
            className="mx-auto size-24 border-2 border-white shadow-sm"
            src={currentAvatar}
            initials={initials}
            name={displayName}
            decorative={false}
          />
          <h2 className="mt-3 text-balance text-xl font-semibold">{displayName}</h2>
          <p className="mt-1 text-sm text-muted-foreground">{user?.email || 'player@example.com'}</p>
        </section>

        <section hidden={profileView !== "profile"}>
          <SectionHeader title="アバターを作る" />
          <div className="mt-3">
            <AvatarPicker value={currentAvatar} onChange={handleAvatarPresetSelect} />
          </div>
          <label className={cn(buttonVariants({ variant: 'outline' }), 'mt-3 h-11 w-full cursor-pointer')}>
            <Camera className="size-4" aria-hidden="true" />
            画像をアップロード
            <input
              className="sr-only"
              type="file"
              accept="image/png,image/jpeg,image/webp"
              onChange={handleAvatarUpload}
            />
          </label>
          <p className="mt-2 text-xs text-muted-foreground">PNG・JPEG・WebP、5MBまで</p>
          {avatarFeedback && (
            <p
              className={cn(
                'mt-2 text-xs',
                avatarFeedback.kind === 'error' ? 'text-destructive' : 'text-emerald-700',
              )}
              role={avatarFeedback.kind === 'error' ? 'alert' : 'status'}
            >
              {avatarFeedback.text}
            </p>
          )}
        </section>

        <section hidden={profileView !== "stats"}>
          <SectionHeader title="戦績" />
          <div className="mt-3">
            <StatsStrip
              label="ゲーム戦績"
              large
              items={[
                { label: '対戦', value: String(gamesPlayed) },
                { label: '勝利', value: String(wins) },
                { label: '勝率', value: winRate },
              ]}
            />
          </div>
        </section>

        <section hidden={profileView !== "profile"}>
          <h2 className="text-balance text-base font-semibold">公開プロフィールを編集</h2>
          <form className="mt-3 space-y-4 rounded-xl border bg-white p-4" onSubmit={handleProfileSubmit} noValidate>
            <div>
              <Label htmlFor="profile-name">表示名</Label>
              <Input
                id="profile-name"
                className="mt-2 h-11"
                value={profileName}
                maxLength={24}
                aria-invalid={Boolean(profileError)}
                aria-describedby={profileError ? 'profile-name-error' : undefined}
                onChange={(event) => {
                  setProfileName(event.target.value)
                  if (profileError) setProfileError('')
                  if (profileStatus) setProfileStatus('')
                }}
              />
              {profileError && (
                <p id="profile-name-error" className="mt-2 text-xs text-destructive" role="alert">
                  {profileError}
                </p>
              )}
            </div>
            <div>
              <Label htmlFor="profile-bio">ひとこと</Label>
              <Input
                id="profile-bio"
                className="mt-2 h-11"
                value={bio}
                maxLength={80}
                onChange={(event) => setBio(event.target.value)}
              />
            </div>
            <Button className="w-full" type="submit" disabled={isSavingProfile}>
              {isSavingProfile ? '保存中...' : '保存'}
            </Button>
            {profileStatus && (
              <p className="text-center text-xs text-emerald-700" role="status">
                {profileStatus}
              </p>
            )}
          </form>
        </section>

        <section hidden={profileView !== "stats"}>
          <div className="flex items-center justify-between">
            <h2 className="text-balance text-base font-semibold">最近の対戦</h2>
            <span className="text-xs text-muted-foreground">直近の対戦履歴</span>
          </div>
          <div className="mt-3 overflow-hidden rounded-xl border bg-white">
            {history.error ? (
              <p className="p-4 text-sm text-destructive" role="alert">{history.error}</p>
            ) : history.isLoading ? (
              <p className="p-4 text-sm text-muted-foreground">読み込み中…</p>
            ) : history.items.length === 0 ? (
              <p className="p-4 text-sm text-muted-foreground">まだ対戦履歴がありません。</p>
            ) : (
              history.items.map((match, index) => (
                <div key={match.gameId}>
                  {index > 0 && <Separator />}
                  <HistoryItem
                    result={match.outcome === 'WIN' ? '勝利' : '敗北'}
                    role={roleLabel(match.role)}
                    title={match.roomName}
                    date={formatPlayedAt(match.playedAt)}
                  />
                </div>
              ))
            )}
          </div>
        </section>

        <section hidden={profileView !== "profile"}>
          <h2 className="text-balance text-base font-semibold">アカウント管理</h2>
          <div className="mt-3 space-y-3 rounded-xl border bg-white p-4">
            <div className="flex flex-wrap gap-x-4 gap-y-2 text-sm">
              <Link className="font-medium text-primary underline underline-offset-4" href="/privacy">
                プライバシーポリシー
              </Link>
              <Link className="font-medium text-primary underline underline-offset-4" href="/terms">
                利用規約
              </Link>
            </div>
            <Separator />
            <Button
              className="w-full"
              variant="outline"
              onClick={handleLogout}
              disabled={isLoggingOut}
            >
              <LogOut className="size-4" aria-hidden="true" />
              {isLoggingOut ? 'ログアウト中...' : 'ログアウト'}
            </Button>
            <AlertDialog>
              <AlertDialogTrigger render={<Button className="w-full" variant="outline" />}>
                <Trash2 className="size-4" aria-hidden="true" />
                アカウント削除
              </AlertDialogTrigger>
              <AlertDialogContent>
                <AlertDialogHeader>
                  <AlertDialogMedia>
                    <Trash2 className="size-5" aria-hidden="true" />
                  </AlertDialogMedia>
                  <AlertDialogTitle>アカウントを削除しますか？</AlertDialogTitle>
                  <AlertDialogDescription>
                    プロフィール、フレンド、ゲーム履歴が削除されます。この操作は取り消せません。
                  </AlertDialogDescription>
                </AlertDialogHeader>
                {deleteAccountError && (
                  <p className="text-center text-xs text-destructive" role="alert">
                    {deleteAccountError}
                  </p>
                )}
                <AlertDialogFooter>
                  <AlertDialogCancel>戻る</AlertDialogCancel>
                  <AlertDialogAction
                    variant="destructive"
                    onClick={handleDeleteAccount}
                    disabled={isDeletingAccount}
                  >
                    {isDeletingAccount ? '削除中...' : 'アカウントを削除'}
                  </AlertDialogAction>
                </AlertDialogFooter>
              </AlertDialogContent>
            </AlertDialog>
          </div>
        </section>
      </main>
    </MobileShell>
  )
}

const RECENT_MATCH_LIMIT = 5

function roleLabel(role: MatchHistoryItem['role']): string {
  return role === 'WORD_WOLF' ? '人狼' : '市民'
}

function formatPlayedAt(iso: string): string {
  const date = new Date(iso)
  return Number.isNaN(date.getTime()) ? '' : `${date.getMonth() + 1}月${date.getDate()}日`
}

function HistoryItem({
  result,
  role,
  title,
  date,
}: {
  result: '勝利' | '敗北'
  role: string
  title: string
  date: string
}) {
  const victory = result === '勝利'

  return (
    <div className="flex items-center gap-3 p-4">
      <div className="min-w-0 flex-1">
        <p className="truncate text-sm font-medium">{title}</p>
        <p className="mt-0.5 text-xs text-muted-foreground">
          {role} · {date}
        </p>
      </div>
      <Badge
        className={cn(
          victory
            ? 'border-emerald-200 bg-emerald-50 text-emerald-800'
            : 'border-red-200 bg-red-50 text-red-800',
        )}
        variant="outline"
      >
        {victory ? (
          <CircleCheck className="size-3.5" aria-hidden="true" />
        ) : (
          <CircleX className="size-3.5" aria-hidden="true" />
        )}
        {result}
      </Badge>
    </div>
  )
}
