import {
  ArrowLeft,
  Check,
  DoorClosed,
  Gamepad2,
  HatGlasses,
  Search,
  Users,
  UserMinus,
  X,
} from 'lucide-react'
import { useEffect, useRef, useState, type ReactNode } from 'react'
import { Link, useLocation } from 'wouter'

import { VictorianScene } from '@/components/game-ui/victorian-scene'
import { GameMark } from '@/components/game-ui/game-mark'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { MobilePortalProvider } from '@/components/layout/mobile-portal-context'
import { Button, buttonVariants } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { DEFAULT_AVATAR_URL } from '@/data/avatar-options'
import { useFriends } from '@/hooks/use-friends'
import { useMe } from '@/hooks/use-me'
import { useRoomInvites } from '@/hooks/use-room-invites'
import { cn } from '@/lib/utils'

type MobileShellProps = {
  children: ReactNode
  title?: string
  backHref?: string
  onBack?: () => void
  actions?: ReactNode
  bottomNav?: boolean
  darkHeader?: boolean
  ambientScene?: boolean
}

export function MobileShell({
  children,
  title,
  backHref,
  onBack,
  actions,
  bottomNav,
  darkHeader,
  ambientScene = true,
}: MobileShellProps) {
  const portalContainerRef = useRef<HTMLDivElement>(null)
  const sceneRef = useRef<HTMLDivElement>(null)
  const [sceneVisible, setSceneVisible] = useState(true)
  const [documentVisible, setDocumentVisible] = useState(true)
  const [reducedMotion, setReducedMotion] = useState(false)
  const {
    latestNotification,
    acceptInvite,
    declineInvite,
    clearLatestNotification,
    latestRemoval,
    clearRemovalNotification,
  } = useRoomInvites()
  const [, navigate] = useLocation()

  useEffect(() => {
    const preference = window.matchMedia?.('(prefers-reduced-motion: reduce)')
    const updatePreference = () =>
      setReducedMotion(preference?.matches ?? false)
    const updateVisibility = () => setDocumentVisible(!document.hidden)
    updatePreference()
    updateVisibility()
    preference?.addEventListener('change', updatePreference)
    document.addEventListener('visibilitychange', updateVisibility)
    return () => {
      preference?.removeEventListener('change', updatePreference)
      document.removeEventListener('visibilitychange', updateVisibility)
    }
  }, [])

  useEffect(() => {
    if (!sceneRef.current || typeof IntersectionObserver === 'undefined') return
    const observer = new IntersectionObserver(([entry]) => {
      setSceneVisible(entry.isIntersecting)
    })
    observer.observe(sceneRef.current)
    return () => observer.disconnect()
  }, [])

  async function handleAcceptNotification() {
    if (!latestNotification) return
    const invite = latestNotification
    clearLatestNotification()
    const res = await acceptInvite(invite.inviteId)
    if (res.success && res.data?.room) {
      navigate(`/lobby?roomId=${res.data.room.roomId}`)
    }
  }

  async function handleDeclineNotification() {
    if (!latestNotification) return
    const inviteId = latestNotification.inviteId
    clearLatestNotification()
    await declineInvite(inviteId)
  }

  return (
    <MobilePortalProvider value={portalContainerRef}>
      <div
        className="mystery-scene"
        data-cinematic={!ambientScene}
        ref={sceneRef}
        data-motion-paused={!sceneVisible || !documentVisible || reducedMotion}
      >
        <div className="scene-environment" aria-hidden="true">
          <VictorianScene animated={ambientScene} />
          {ambientScene && <div className="scene-drifting-mist" />}
          <div className="scene-vignette" />
        </div>
        <div className="game-frame">
          <div className="mobile-game-shell relative isolate z-10 flex min-h-0 flex-1 flex-col overflow-hidden">
            {latestRemoval && (
              <div role="alert" className="bg-card border-b p-4">
                <div className="flex items-start gap-3">
                  <UserMinus
                    className="text-destructive mt-1 size-5 shrink-0"
                    aria-hidden="true"
                  />
                  <div className="min-w-0 flex-1 text-sm">
                    <p className="font-semibold">部屋から退出しました</p>
                    <p className="text-muted-foreground mt-1 text-pretty break-words">
                      {latestRemoval.roomName
                        ? `「${latestRemoval.roomName}」から`
                        : '参加していた部屋から'}
                      ホストにより退出させられました。
                    </p>
                  </div>
                  <Button
                    type="button"
                    variant="ghost"
                    size="icon"
                    aria-label="退出通知を閉じる"
                    onClick={clearRemovalNotification}
                  >
                    <X className="size-4" aria-hidden="true" />
                  </Button>
                </div>
              </div>
            )}
            {latestNotification && (
              <InviteNotificationToast
                notification={latestNotification}
                onAccept={() => void handleAcceptNotification()}
                onDecline={() => void handleDeclineNotification()}
                onClose={clearLatestNotification}
              />
            )}
            {bottomNav && (
              <header className="game-menu-header">
                <Link href="/home" aria-label="ホームへ戻る">
                  <GameMark inverse />
                </Link>
                <TopNavigation />
              </header>
            )}
            {title ? (
              <header
                className={cn(
                  'case-page-header z-20 shrink-0',
                  darkHeader
                    ? 'border-brass/30 bg-night text-white'
                    : 'border-border bg-card',
                )}
              >
                <div className="case-header-parts">
                  {onBack ? (
                    <button
                      aria-label="前の画面へ戻る"
                      className={cn(
                        buttonVariants({ size: 'icon', variant: 'ghost' }),
                        'shrink-0',
                        darkHeader &&
                          'text-white hover:bg-white/10 hover:text-white',
                      )}
                      type="button"
                      onClick={onBack}
                    >
                      <ArrowLeft className="size-5" aria-hidden="true" />
                    </button>
                  ) : backHref ? (
                    <Link
                      aria-label="前の画面へ戻る"
                      className={cn(
                        buttonVariants({ size: 'icon', variant: 'ghost' }),
                        'shrink-0',
                        darkHeader &&
                          'text-white hover:bg-white/10 hover:text-white',
                      )}
                      href={backHref}
                    >
                      <ArrowLeft className="size-5" aria-hidden="true" />
                    </Link>
                  ) : null}
                  <h1 className="case-title-plaque">{title}</h1>
                  {actions}
                </div>
              </header>
            ) : null}

            <div className="case-scroll min-h-0 flex-1 overflow-y-auto">
              {children}
            </div>

            <div
              ref={portalContainerRef}
              className="pointer-events-none absolute inset-0 z-40"
              data-mobile-portal
            />
          </div>
        </div>
      </div>
    </MobilePortalProvider>
  )
}

type InviteNotification = NonNullable<
  ReturnType<typeof useRoomInvites>['latestNotification']
>

// Discord風 招待受信トースト通知（20秒で自動消去、参加・不参加タップで即座に消去）
function InviteNotificationToast({
  notification,
  onAccept,
  onDecline,
  onClose,
}: {
  notification: InviteNotification
  onAccept: () => void
  onDecline: () => void
  onClose: () => void
}) {
  return (
    <div className="animate-in fade-in slide-in-from-top-4 absolute inset-x-3 top-2 z-50 duration-300">
      <Card className="border-brass/60 bg-night relative overflow-hidden text-white shadow-2xl">
        <CardContent className="p-3.5">
          <div className="flex items-start gap-3">
            <div className="relative mt-0.5">
              <PlayerAvatar
                className="border-brass/50 size-9 border"
                src={notification.from.avatarUrl}
                initials={notification.from.displayName
                  .slice(0, 2)
                  .toUpperCase()}
                name={notification.from.displayName}
              />
              <span className="absolute -top-1 -right-1 flex size-3">
                <span className="absolute inline-flex h-full w-full animate-ping rounded-full bg-red-400 opacity-75" />
                <span className="relative inline-flex size-3 rounded-full bg-red-500" />
              </span>
            </div>

            <div className="min-w-0 flex-1">
              <div className="flex items-center gap-1.5">
                <Gamepad2 className="text-brass size-3.5" aria-hidden="true" />
                <span className="text-brass text-xs font-semibold">
                  ゲーム招待が届きました
                </span>
              </div>
              <p className="mt-0.5 truncate text-sm font-medium">
                {notification.from.displayName}さんから
              </p>
              <p className="truncate text-xs text-stone-300">
                「{notification.roomName}」への招待
              </p>

              <div className="mt-2.5 flex items-center gap-2">
                <Button
                  className="h-7 bg-emerald-600 px-3 text-xs font-medium text-white hover:bg-emerald-700"
                  size="sm"
                  onClick={onAccept}
                >
                  <Check className="mr-1 size-3.5" aria-hidden="true" />
                  参加
                </Button>
                <Button
                  className="h-7 px-2.5 text-xs text-stone-300 hover:bg-white/10 hover:text-white"
                  size="sm"
                  variant="ghost"
                  onClick={onDecline}
                >
                  不参加
                </Button>
                <Button
                  aria-label="閉じる"
                  className="ml-auto h-7 px-2 text-xs text-stone-400 hover:text-stone-200"
                  size="sm"
                  variant="ghost"
                  onClick={onClose}
                >
                  <X className="size-3.5" aria-hidden="true" />
                </Button>
              </div>
            </div>
          </div>
        </CardContent>
        {/* 20秒カウントダウン プログレスバー（keyframes: frontend/src/styles/motion.css） */}
        <div className="absolute bottom-0 left-0 h-0.5 w-full bg-white/10">
          <div
            className="bg-brass/80 h-full"
            style={{ animation: 'shrink-width 20s linear forwards' }}
          />
        </div>
      </Card>
    </div>
  )
}

export function BrandHeader() {
  const { user } = useMe()
  const avatarUrl = user?.avatarUrl || DEFAULT_AVATAR_URL
  const displayName = user?.displayName || '未設定'
  const initials = displayName.slice(0, 2).toUpperCase()

  return (
    <header className="case-brand-header border-brass/30 bg-night flex items-center justify-between border-b px-5 py-4 text-white">
      <Link
        aria-label="ホームへ戻る"
        className="focus-visible:ring-ring/50 rounded-lg outline-none focus-visible:ring-3"
        href="/home"
      >
        <GameMark inverse />
      </Link>
      <Link
        aria-label="プロフィールを開く"
        className="focus-visible:ring-ring/50 rounded-full outline-none focus-visible:ring-3"
        href="/profile"
      >
        <PlayerAvatar
          className="size-9"
          src={avatarUrl}
          initials={initials}
          name={displayName}
        />
      </Link>
    </header>
  )
}

const navItems = [
  { href: '/home', label: 'ホーム', icon: DoorClosed },
  { href: '/rooms', label: '部屋', icon: Search },
  { href: '/friends', label: 'フレンド', icon: Users },
  { href: '/profile', label: '自分', icon: HatGlasses },
]

function TopNavigation() {
  const [location] = useLocation()
  const { friendRequests } = useFriends()
  const { invites } = useRoomInvites()

  // フレンドタブの未読バッジ総数（フレンド申請 + ゲーム招待）
  const totalFriendsUnread = friendRequests.length + invites.length

  return (
    <nav
      className="case-navigation bg-card z-20 shrink-0 border-t px-2 pt-2 pb-[max(0.5rem,env(safe-area-inset-bottom))]"
      aria-label="メインナビゲーション"
    >
      {navItems.map((item) => {
        const active =
          location === item.href ||
          (item.href === '/rooms' &&
            (location.startsWith('/rooms/') || location.startsWith('/join/')))
        const Icon = item.icon
        const hasUnread = item.href === '/friends' && totalFriendsUnread > 0

        return (
          <Link
            key={item.href}
            className={cn(
              'focus-visible:ring-ring/50 relative flex min-h-12 flex-col items-center justify-center gap-1 rounded-lg text-xs font-medium outline-none focus-visible:ring-3',
              active
                ? 'bg-primary/10 text-primary'
                : 'text-muted-foreground hover:bg-muted hover:text-foreground',
            )}
            href={item.href}
            aria-current={active ? 'page' : undefined}
            onClick={(e) => {
              if (active) {
                // すでにそのタブにいる場合はページをリロードして最新データを再取得する
                e.preventDefault()
                window.location.reload()
              }
            }}
          >
            <div className="relative">
              <Icon className="size-5" aria-hidden="true" />
              {/* Discord風の赤い未読バッジ */}
              {hasUnread && (
                <span className="absolute -top-1.5 -right-2.5 flex items-center justify-center">
                  <span className="absolute inline-flex size-full animate-ping rounded-full bg-red-400 opacity-75" />
                  <span className="relative flex size-4 items-center justify-center rounded-full bg-red-500 text-[10px] font-bold text-white shadow-sm">
                    {totalFriendsUnread > 9 ? '9+' : totalFriendsUnread}
                  </span>
                </span>
              )}
            </div>
            {item.label}
          </Link>
        )
      })}
    </nav>
  )
}
