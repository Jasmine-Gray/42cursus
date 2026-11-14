import { useEffect, useState } from 'react'
import { Dialog } from '@base-ui/react/dialog'
import { Button } from '@/components/ui/button'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { useMobilePortalContainer } from '@/components/layout/mobile-portal-context'
import { useFriends } from '@/hooks/use-friends'
import { api } from '@/lib/api-client'
import type { components } from '@/types/api'

export function ParticipantProfileButton({
  userId,
  displayName,
  currentUserId,
}: {
  userId: string
  displayName: string
  currentUserId?: string
}) {
  const [open, setOpen] = useState(false)
  const container = useMobilePortalContainer()
  return (
    <Dialog.Root open={open} onOpenChange={setOpen}>
      <Dialog.Trigger
        render={
          <Button
            variant="ghost"
            size="sm"
            className="h-auto max-w-full truncate px-1 py-1 text-inherit"
          />
        }
        aria-label={`${displayName}さんのプロフィール`}
      >
        {displayName}
      </Dialog.Trigger>
      <Dialog.Portal container={container ?? undefined}>
        <Dialog.Backdrop className="pointer-events-auto fixed inset-0 z-50 bg-black/40" />
        <Dialog.Popup className="bg-popover text-popover-foreground pointer-events-auto fixed top-1/2 left-1/2 z-50 grid max-h-dvh w-full max-w-xs -translate-x-1/2 -translate-y-1/2 gap-4 overflow-y-auto rounded-xl p-5 shadow-lg outline-none">
          <Dialog.Title className="text-lg font-semibold text-balance">
            プロフィール
          </Dialog.Title>
          <Dialog.Description className="sr-only">
            参加者のプロフィールとフレンド登録
          </Dialog.Description>
          {open && (
            <ParticipantProfile
              key={userId}
              userId={userId}
              currentUserId={currentUserId}
            />
          )}
          <Dialog.Close render={<Button variant="outline" />}>
            閉じる
          </Dialog.Close>
        </Dialog.Popup>
      </Dialog.Portal>
    </Dialog.Root>
  )
}

function ParticipantProfile({
  userId,
  currentUserId,
}: {
  userId: string
  currentUserId?: string
}) {
  const [profile, setProfile] = useState<
    components['schemas']['UserProfile'] | null
  >(null)
  const [loadError, setLoadError] = useState('')
  const [attempt, setAttempt] = useState(0)
  const [busy, setBusy] = useState(false)
  const [actionError, setActionError] = useState('')
  const {
    friends,
    friendRequests,
    sentRequests,
    isLoading,
    error,
    refresh,
    sendFriendRequest,
    acceptFriendRequest,
  } = useFriends()

  useEffect(() => {
    let active = true
    void api
      .GET('/users/{userId}', { params: { path: { userId } } })
      .then(({ data, error: failure }) => {
        if (!active) return
        if (failure || !data)
          setLoadError('プロフィールを取得できませんでした。')
        else setProfile(data.data)
      })
      .catch(() => {
        if (active) setLoadError('通信エラーが発生しました。')
      })
    return () => {
      active = false
    }
  }, [userId, attempt])

  if (loadError)
    return (
      <div className="space-y-3">
        <p role="alert">{loadError}</p>
        <Button
          onClick={() => {
            setLoadError('')
            setAttempt(attempt + 1)
          }}
        >
          再読み込み
        </Button>
      </div>
    )
  if (!profile) return <p role="status">読み込んでいます…</p>

  const friend = friends.some((item) => item.user.userId === userId)
  const sent = sentRequests.some((item) => item.targetUser.userId === userId)
  const incoming = friendRequests.find((item) => item.from.userId === userId)
  const self = currentUserId === userId

  async function requestFriend() {
    if (!profile || busy) return
    setBusy(true)
    setActionError('')
    try {
      const response = incoming
        ? await acceptFriendRequest(incoming.requestId)
        : await sendFriendRequest(profile)
      if (!response.success)
        setActionError(response.error ?? 'フレンド申請に失敗しました。')
    } catch {
      setActionError('通信エラーが発生しました。もう一度お試しください。')
    } finally {
      setBusy(false)
    }
  }

  return (
    <>
      <div className="flex items-center gap-3">
        <PlayerAvatar
          className="size-16"
          src={profile.avatarUrl}
          name={profile.displayName}
          initials={profile.displayName.slice(0, 2)}
        />
        <p className="min-w-0 font-semibold break-words">
          {profile.displayName}
        </p>
      </div>
      <div>
        <h3 className="text-muted-foreground mb-1 text-xs">ひとこと</h3>
        <p className="text-sm break-words whitespace-pre-wrap">
          {profile.bio || 'ひとことはまだありません。'}
        </p>
      </div>
      {!self && (
        <>
          {error ? (
            <div className="space-y-2">
              <p role="alert" className="text-destructive text-sm">
                {error}
              </p>
              <Button variant="outline" onClick={() => void refresh()}>
                フレンド情報を再読み込み
              </Button>
            </div>
          ) : (
            <Button
              disabled={isLoading || busy || friend || sent}
              onClick={() => void requestFriend()}
            >
              {isLoading
                ? 'フレンド情報を確認中…'
                : busy
                  ? '送信中…'
                  : friend
                    ? 'フレンド登録済み'
                    : sent
                      ? 'フレンド申請済み'
                      : incoming
                        ? 'フレンド申請を承認'
                        : 'フレンド申請する'}
            </Button>
          )}
          {actionError && (
            <p role="alert" className="text-destructive text-sm">
              {actionError}
            </p>
          )}
        </>
      )}
    </>
  )
}
