import { Camera, Check, CircleCheck, CircleX, Trash2,  X } from 'lucide-react'
import { useState, type ChangeEvent, type FormEvent } from 'react'
import { Link, useLocation } from 'wouter'

import { MobileShell } from '@/components/layout/mobile-shell'
import { CharacterSetup } from '@/components/game-ui/character-setup'
import { AvatarPicker } from '@/components/game-ui/avatar-picker'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { SectionHeader } from '@/components/game-ui/section-header'
import { StatsStrip } from '@/components/game-ui/stats-strip'
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
import { Card, CardContent } from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { Separator } from '@/components/ui/separator'
import { cn } from '@/lib/utils'

const friends = [
  { name: 'アオイ', initials: 'AO', avatarSrc: '/avatars/aoi.webp', status: 'オンライン', online: true },
  { name: 'レン', initials: 'RE', avatarSrc: '/avatars/ren.webp', status: 'ゲーム中', online: true },
  { name: 'ユナ', initials: 'YU', avatarSrc: '/avatars/yuna.webp', status: '2時間前', online: false },
  { name: 'ケイ', initials: 'KE', avatarSrc: '/avatars/kei.webp', status: '昨日', online: false },
]

export function FriendsPage() {
  const [request, setRequest] = useState<'pending' | 'accepted' | 'declined'>('pending')
  const [query, setQuery] = useState('')
  const [invited, setInvited] = useState<string[]>([])
  const list = request === 'accepted' ? [...friends, { name: 'ソラ', initials: 'SO', avatarSrc: '/avatars/minato.webp', status: 'オンライン', online: true }] : friends
  return (
    <MobileShell title="フレンド" bottomNav>
      <main className="space-y-6 px-5 py-5">
        <section>
          <div className="flex items-center justify-between">
            <h2 className="text-balance text-base font-semibold">フレンド申請</h2>
            <Badge variant="secondary">{request === "pending" ? "1件" : "0件"}</Badge>
          </div>
          {request === "pending" ? <Card className="mt-3 shadow-none">
            <CardContent className="flex items-center gap-3 p-4">
              <PlayerAvatar className="size-11" src="/avatars/minato.webp" initials="SO" name="ソラ" />
              <div className="min-w-0 flex-1">
                <p className="truncate text-sm font-medium">ソラ</p>
                <p className="text-xs text-muted-foreground">3分前に申請</p>
              </div>
              <Button onClick={() => setRequest("accepted")} aria-label="ソラさんの申請を承認" size="icon" variant="secondary"><Check className="size-4" aria-hidden="true" /></Button>
              <Button onClick={() => setRequest("declined")} aria-label="ソラさんの申請を拒否" size="icon" variant="ghost"><X className="size-4" aria-hidden="true" /></Button>
            </CardContent>
          </Card> : <p className="mt-4 text-sm text-muted-foreground" role="status">{request === "accepted" ? "ソラさんをフレンドに追加しました。" : "申請を拒否しました。"}</p>}
        </section>

        <section>
          <div className="flex items-center justify-between">
            <h2 className="text-balance text-base font-semibold">フレンド一覧</h2>
            <span className="text-xs text-muted-foreground">{list.length}人</span>
          </div>
          <div className="mt-3 overflow-hidden rounded-xl border bg-white">
            {list.filter(friend => friend.name.includes(query.trim())).map((friend, index) => (
              <div key={friend.name}>
                {index > 0 && <Separator />}
                <div className="flex items-center gap-3 p-4">
                  <div className="relative">
                    <PlayerAvatar className="size-11" src={friend.avatarSrc} initials={friend.initials} name={friend.name} />
                    <span className={cn('absolute bottom-0 right-0 size-2.5 rounded-full border-2 border-white', friend.online ? 'bg-emerald-500' : 'bg-stone-300')} aria-hidden="true" />
                  </div>
                  <div className="min-w-0 flex-1">
                    <p className="truncate text-sm font-medium">{friend.name}</p>
                    <p className="text-xs text-muted-foreground">{friend.status}</p>
                  </div>
                  <Button onClick={() => setInvited(current => [...current, friend.name])} disabled={invited.includes(friend.name) || !friend.online || friend.status === 'ゲーム中'} size="sm" variant="outline">
                    {invited.includes(friend.name) ? "招待済み" : "招待"}
                  </Button>
                </div>
              </div>
            ))}
          </div>
        </section>

        <label className="block text-sm">フレンドを表示名で絞り込む<input className="mt-2 h-11 w-full rounded-lg bg-white px-3" value={query} onChange={event => setQuery(event.target.value)} placeholder="表示名を入力" /></label>
      </main>
    </MobileShell>
  )
}

export function AvatarSetupPage() {
  const [, navigate] = useLocation()
  return (
    <MobileShell title="キャラクター選択" backHref="/register">
      <CharacterSetup initialSrc="/avatars/yu.webp" displayName="田中 ユウ" onSave={async (src, file) => {
        const saved = file ? await new Promise<string>((resolve, reject) => {
          const reader = new FileReader()
          reader.onload = () => resolve(String(reader.result))
          reader.onerror = () => reject(new Error('画像を読み込めませんでした。'))
          reader.readAsDataURL(file)
        }) : src
        localStorage.setItem('mock-character', saved)
        navigate('/home')
      }} />
    </MobileShell>
  )
}

export function ProfilePage() {
  const [profileView, setProfileView] = useState<"profile" | "stats">("profile")
  const [, navigate] = useLocation()
  const [selectedAvatar, setSelectedAvatar] = useState('/avatars/yu.webp')
  const [profileName, setProfileName] = useState('田中 ユウ')
  const [bio, setBio] = useState('推理ゲームが好きです')
  const [profileError, setProfileError] = useState('')
  const [profileStatus, setProfileStatus] = useState('')
  const [avatarFeedback, setAvatarFeedback] = useState<{ kind: 'error' | 'success'; text: string } | null>(null)

  function handleAvatarChange(event: ChangeEvent<HTMLInputElement>) {
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
    setAvatarFeedback({ kind: 'success', text: `${file.name}を選択しました。保存時に反映されます。` })
  }

  function handleProfileSubmit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    if (profileName.trim().length < 2) {
      setProfileError('表示名は2文字以上で入力してください。')
      setProfileStatus('')
      return
    }
    setProfileError('')
    setProfileStatus('プロフィールを保存しました。')
  }

  return (
    <MobileShell title="プロフィール" bottomNav>
      <main className="profile-screen space-y-6 px-5 py-5">
        <div className="profile-view-switch" aria-label="表示内容">
          <Button variant={profileView === 'profile' ? 'default' : 'outline'} aria-pressed={profileView === 'profile'} onClick={() => setProfileView('profile')}>プロフィール</Button>
          <Button variant={profileView === 'stats' ? 'default' : 'outline'} aria-pressed={profileView === 'stats'} onClick={() => setProfileView('stats')}>戦績</Button>
        </div>
        <section className="text-center">
          <PlayerAvatar className="mx-auto size-24 border-2 border-white shadow-sm" src={selectedAvatar} initials="YU" name="田中 ユウ" decorative={false} />
          <h2 className="mt-3 text-balance text-xl font-semibold">田中 ユウ</h2>
          <p className="mt-1 text-sm text-muted-foreground">@tanaka_yu</p>
        </section>

        <section hidden={profileView !== "profile"}>
          <SectionHeader title="アバターを作る" />
          <div className="mt-3"><AvatarPicker value={selectedAvatar} onChange={(src) => {
            setSelectedAvatar(src)
            setAvatarFeedback({ kind: 'success', text: 'アバタープリセットを選択しました。' })
          }} /></div>
          <label className={cn(buttonVariants({ variant: 'outline' }), 'mt-3 h-11 w-full cursor-pointer')}>
            <Camera className="size-4" aria-hidden="true" />画像をアップロード
            <input className="sr-only" type="file" accept="image/png,image/jpeg,image/webp" onChange={handleAvatarChange} />
          </label>
          <p className="mt-2 text-xs text-muted-foreground">PNG・JPEG・WebP、5MBまで</p>
          {avatarFeedback && (
            <p className={cn('mt-2 text-xs', avatarFeedback.kind === 'error' ? 'text-destructive' : 'text-emerald-700')} role={avatarFeedback.kind === 'error' ? 'alert' : 'status'}>
              {avatarFeedback.text}
            </p>
          )}
        </section>

        <section hidden={profileView !== "stats"}>
          <SectionHeader title="戦績" />
          <div className="mt-3"><StatsStrip label="ゲーム戦績" large items={[{ label: '対戦', value: '12' }, { label: '勝利', value: '8' }, { label: '勝率', value: '67%' }]} /></div>
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
              {profileError && <p id="profile-name-error" className="mt-2 text-xs text-destructive" role="alert">{profileError}</p>}
            </div>
            <div>
              <Label htmlFor="profile-bio">ひとこと</Label>
              <Input id="profile-bio" className="mt-2 h-11" value={bio} maxLength={80} onChange={(event) => setBio(event.target.value)} />
            </div>
            <Button className="w-full" type="submit">保存</Button>
            {profileStatus && <p className="text-center text-xs text-emerald-700" role="status">{profileStatus}</p>}
          </form>
        </section>

        <section hidden={profileView !== "stats"}>
          <div className="flex items-center justify-between">
            <h2 className="text-balance text-base font-semibold">最近の対戦</h2>
            <span className="text-xs text-muted-foreground">直近3件</span>
          </div>
          <div className="mt-3 overflow-hidden rounded-xl border bg-white">
            <HistoryItem result="勝利" role="市民" title="深夜の読書会" date="8月4日" />
            <Separator />
            <HistoryItem result="敗北" role="人狼" title="甘いもの捜査班" date="8月2日" />
            <Separator />
            <HistoryItem result="勝利" role="市民" title="週末の目撃証言" date="7月30日" />
          </div>
        </section>

        <section hidden={profileView !== "profile"}>
          <h2 className="text-balance text-base font-semibold">アカウント管理</h2>
          <div className="mt-3 space-y-3 rounded-xl border bg-white p-4">
            <div className="flex flex-wrap gap-x-4 gap-y-2 text-sm">
              <Link className="font-medium text-primary underline underline-offset-4" href="/privacy">プライバシーポリシー</Link>
              <Link className="font-medium text-primary underline underline-offset-4" href="/terms">利用規約</Link>
            </div>
            <Separator />
            <AlertDialog>
              <AlertDialogTrigger render={<Button className="w-full" variant="outline" />}>
                <Trash2 className="size-4" aria-hidden="true" />アカウント削除
              </AlertDialogTrigger>
              <AlertDialogContent>
                <AlertDialogHeader>
                  <AlertDialogMedia><Trash2 className="size-5" aria-hidden="true" /></AlertDialogMedia>
                  <AlertDialogTitle>アカウントを削除しますか？</AlertDialogTitle>
                  <AlertDialogDescription>プロフィール、フレンド、ゲーム履歴が削除されます。この操作は取り消せません。</AlertDialogDescription>
                </AlertDialogHeader>
                <AlertDialogFooter>
                  <AlertDialogCancel>戻る</AlertDialogCancel>
                  <AlertDialogAction variant="destructive" onClick={() => navigate('/login')}>アカウントを削除</AlertDialogAction>
                </AlertDialogFooter>
              </AlertDialogContent>
            </AlertDialog>
          </div>
        </section>
      </main>
    </MobileShell>
  )
}

function HistoryItem({ result, role, title, date }: { result: '勝利' | '敗北'; role: string; title: string; date: string }) {
  const victory = result === '勝利'

  return (
    <div className="flex items-center gap-3 p-4">
      <div className="min-w-0 flex-1">
        <p className="truncate text-sm font-medium">{title}</p>
        <p className="mt-0.5 text-xs text-muted-foreground">{role} · {date}</p>
      </div>
      <Badge
        className={cn(victory ? 'border-emerald-200 bg-emerald-50 text-emerald-800' : 'border-red-200 bg-red-50 text-red-800')}
        variant="outline"
      >
        {victory ? <CircleCheck className="size-3.5" aria-hidden="true" /> : <CircleX className="size-3.5" aria-hidden="true" />}
        {result}
      </Badge>
    </div>
  )
}
