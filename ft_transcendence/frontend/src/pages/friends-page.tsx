import { Check, Gamepad2, Plus, RotateCw, Search, UserPlus, X } from 'lucide-react'
import { useState, type FormEvent } from 'react'
import { Link, useLocation } from 'wouter'

import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { RemoveFriendButton } from '@/components/game-ui/remove-friend-button'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Badge } from '@/components/ui/badge'
import { Button } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { Separator } from '@/components/ui/separator'
import { useFriends, type UserPublic } from '@/hooks/use-friends'
import { useMe } from '@/hooks/use-me'
import { useRoomInvites } from '@/hooks/use-room-invites'
import { cn } from '@/lib/utils'

export function FriendsPage() {
  const [, navigate] = useLocation()
  const { activeRoom, refreshMe } = useMe()
  const {
    friends,
    friendRequests,
    sentRequests,
    searchResults,
    isSearching,
    isLoading: isFriendsLoading,
    error: friendsError,
    refresh: refreshFriends,
    searchUsers,
    clearSearchResults,
    sendFriendRequest,
    acceptFriendRequest,
    declineFriendRequest,
    removeFriend,
  } = useFriends()

  const {
    invites: gameInvites,
    refreshInvites,
    inviteFriend,
    acceptInvite,
    declineInvite,
  } = useRoomInvites()

  const [isRefreshing, setIsRefreshing] = useState(false)
  const [showSearch, setShowSearch] = useState(false)
  const [searchQuery, setSearchQuery] = useState('')
  const [requestStatusMap, setRequestStatusMap] = useState<Record<string, string>>({})
  const [gameInviteStatusMap, setGameInviteStatusMap] = useState<Record<string, string>>({})
  const [actionFeedback, setActionFeedback] = useState<string | null>(null)

  async function handleRefreshAll() {
    setIsRefreshing(true)
    await Promise.all([refreshFriends(), refreshInvites(), refreshMe()])
    setIsRefreshing(false)
  }

  async function handleSearchSubmit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    if (searchQuery.trim().length < 2) return
    await searchUsers(searchQuery)
  }

  async function handleSendRequest(user: UserPublic) {
    setRequestStatusMap((prev) => ({ ...prev, [user.userId]: 'sending' }))
    const result = await sendFriendRequest(user)
    if (result.success) {
      setRequestStatusMap((prev) => ({ ...prev, [user.userId]: 'sent' }))
      setActionFeedback(`${user.displayName}さんにフレンド申請を送信しました。`)
    } else {
      setRequestStatusMap((prev) => ({ ...prev, [user.userId]: 'error' }))
      setActionFeedback(result.error || '申請の送信に失敗しました。')
    }
  }

  async function handleAcceptFriend(requestId: string, name: string) {
    const result = await acceptFriendRequest(requestId)
    if (result.success) {
      setActionFeedback(`${name}さんとフレンドになりました！`)
    } else {
      setActionFeedback(result.error || '承認に失敗しました。')
    }
  }

  async function handleDeclineFriend(requestId: string, name: string) {
    const result = await declineFriendRequest(requestId)
    if (result.success) {
      setActionFeedback(`${name}さんの申請を拒否しました。`)
    } else {
      setActionFeedback(result.error || '拒否に失敗しました。')
    }
  }

  async function handleRemoveFriend(friendUser: UserPublic) {
    const result = await removeFriend(friendUser.userId)
    if (result.success) {
      setRequestStatusMap((prev) => {
        const next = { ...prev }
        delete next[friendUser.userId]
        return next
    })
      setActionFeedback(`${friendUser.displayName}さんとのフレンドを解除しました。`)
    }
    return result
  }

  // フレンドへゲーム（部屋）招待を送信
  async function handleInviteToGame(friendUser: UserPublic) {
    if (gameInviteStatusMap[friendUser.userId] === 'sending') return
    if (!activeRoom) {
      setActionFeedback('現在参加中の部屋がありません。先に「部屋を作る」またはロビーから部屋を作成してください。')
      return
    }

    setGameInviteStatusMap((prev) => ({ ...prev, [friendUser.userId]: 'sending' }))
    const res = await inviteFriend(activeRoom.roomId, friendUser.userId)

    if (res.success) {
      setGameInviteStatusMap((prev) => ({ ...prev, [friendUser.userId]: 'invited' }))
      setActionFeedback(`${friendUser.displayName}さんを「${activeRoom.roomName}」に招待しました！`)
    } else {
      setGameInviteStatusMap((prev) => ({ ...prev, [friendUser.userId]: 'error' }))
      setActionFeedback(res.error || '招待の送信に失敗しました。')
    }
  }

  // 受信したゲーム招待を承認してロビーへ参加
  async function handleAcceptGameInvite(inviteId: string) {
    const res = await acceptInvite(inviteId)
    if (res.success) {
      navigate('/lobby')
    } else {
      setActionFeedback(res.error || '参加に失敗しました。')
    }
  }

  // 受信したゲーム招待を辞退
  async function handleDeclineGameInvite(inviteId: string, roomName: string) {
    const res = await declineInvite(inviteId)
    if (res.success) {
      setActionFeedback(`「${roomName}」への招待を辞退しました。`)
    } else {
      setActionFeedback(res.error || '辞退に失敗しました。')
    }
  }

  return (
    <MobileShell
      title="フレンド"
      bottomNav
      actions={
        <Button
          aria-label="画面を更新"
          className="size-9 p-0"
          disabled={isRefreshing || isFriendsLoading}
          size="icon"
          variant="ghost"
          onClick={() => void handleRefreshAll()}
        >
          <RotateCw className={cn('size-4', isRefreshing && 'animate-spin')} />
        </Button>
      }
    >
      <main className="space-y-6 px-5 py-5">
        {actionFeedback && (
          <div className="rounded-lg bg-emerald-50 p-3 text-xs text-emerald-800 border border-emerald-200">
            {actionFeedback}
          </div>
        )}

        {friendsError && (
          <div className="rounded-lg bg-red-50 p-3 text-xs text-destructive border border-red-200">
            {friendsError}
          </div>
        )}

        {/* 現在参加中の部屋情報（招待先ルーム） */}
        {activeRoom ? (
          <div className="rounded-xl border border-brass/40 bg-card p-4 shadow-sm">
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-1.5">
                <Gamepad2 className="size-4 text-brass" />
                <span className="text-xs font-semibold text-brass">参加中のルーム</span>
              </div>
              <Badge variant="outline" className="font-mono text-[10px] tracking-wide">
                {activeRoom.inviteCode}
              </Badge>
            </div>
            <div className="mt-2 flex items-center justify-between">
              <div>
                <p className="text-sm font-semibold text-foreground">{activeRoom.roomName}</p>
                <p className="text-xs text-muted-foreground">フレンド一覧の「招待」ボタンで招待できます</p>
              </div>
              <Button size="sm" variant="outline" className="h-8 text-xs" onClick={() => navigate('/lobby')}>
                ロビーへ
              </Button>
            </div>
          </div>
        ) : (
          <div className="rounded-xl border border-dashed border-border bg-card/50 p-3.5 text-center">
            <p className="text-xs text-muted-foreground">
              参加中の部屋がありません。部屋を作成するとフレンドを招待できます。
            </p>
            <div className="mt-2 flex justify-center">
              <Link href="/rooms/new">
                <Button size="sm" variant="outline" className="h-7 text-xs">
                  <Plus className="size-3.5 mr-1" />
                  部屋を作る
                </Button>
              </Link>
            </div>
          </div>
        )}

        {/* 受信したゲーム招待一覧 */}
        {gameInvites.length > 0 && (
          <section>
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-1.5">
                <Gamepad2 className="size-4 text-brass" />
                <h2 className="text-balance text-base font-semibold">ゲームへの招待</h2>
              </div>
              <Badge variant="destructive">{gameInvites.length}件</Badge>
            </div>

            <div className="mt-3 space-y-2">
              {gameInvites.map((invite) => {
                const initials = invite.from.displayName.slice(0, 2).toUpperCase()
                return (
                  <Card key={invite.inviteId} className="border-brass/50 bg-stone-50 shadow-none">
                    <CardContent className="p-4">
                      <div className="flex items-center gap-3">
                        <PlayerAvatar
                          className="size-11 border border-brass/40"
                          src={invite.from.avatarUrl}
                          initials={initials}
                          name={invite.from.displayName}
                        />
                        <div className="min-w-0 flex-1">
                          <div className="flex items-center gap-2">
                            <p className="truncate text-sm font-semibold">{invite.roomName}</p>
                            <Badge className="text-[10px]" variant="outline">{invite.inviteCode}</Badge>
                          </div>
                          <p className="truncate text-xs text-muted-foreground mt-0.5">
                            {invite.from.displayName}さんからの招待
                          </p>
                        </div>
                      </div>

                      <div className="mt-3 flex items-center justify-end gap-2">
                        <Button
                          className="h-8 px-3 text-xs"
                          variant="ghost"
                          onClick={() => void handleDeclineGameInvite(invite.inviteId, invite.roomName)}
                        >
                          辞退
                        </Button>
                        <Button
                          className="h-8 px-4 text-xs"
                          onClick={() => void handleAcceptGameInvite(invite.inviteId)}
                        >
                          <Check className="size-3.5 mr-1" />
                          参加する
                        </Button>
                      </div>
                    </CardContent>
                  </Card>
                )
              })}
            </div>
          </section>
        )}

        {/* フレンド申請一覧 */}
        <section>
          <div className="flex items-center justify-between">
            <h2 className="text-balance text-base font-semibold">フレンド申請</h2>
            <Badge variant="secondary">{friendRequests.length}件</Badge>
          </div>

          {friendRequests.length === 0 ? (
            <p className="mt-3 text-xs text-muted-foreground">新しいフレンド申請はありません</p>
          ) : (
            <div className="mt-3 space-y-2">
              {friendRequests.map((req) => {
                const initials = req.from.displayName.slice(0, 2).toUpperCase()
                return (
                  <Card key={req.requestId} className="shadow-none">
                    <CardContent className="flex items-center gap-3 p-4">
                      <PlayerAvatar
                        className="size-11"
                        src={req.from.avatarUrl}
                        initials={initials}
                        name={req.from.displayName}
                      />
                      <div className="min-w-0 flex-1">
                        <p className="truncate text-sm font-medium">{req.from.displayName}</p>
                        <p className="text-xs text-muted-foreground">フレンド申請が届いています</p>
                      </div>
                      <Button
                        aria-label={`${req.from.displayName}さんの申請を承認`}
                        size="icon"
                        variant="secondary"
                        onClick={() => handleAcceptFriend(req.requestId, req.from.displayName)}
                      >
                        <Check className="size-4" aria-hidden="true" />
                      </Button>
                      <Button
                        aria-label={`${req.from.displayName}さんの申請を拒否`}
                        size="icon"
                        variant="ghost"
                        onClick={() => handleDeclineFriend(req.requestId, req.from.displayName)}
                      >
                        <X className="size-4" aria-hidden="true" />
                      </Button>
                    </CardContent>
                  </Card>
                )
              })}
            </div>
          )}
        </section>

        {/* 送信したフレンド申請（承認待ち） */}
        {sentRequests.length > 0 && (
          <section>
            <div className="flex items-center justify-between">
              <h2 className="text-balance text-base font-semibold">送信した申請（承認待ち）</h2>
              <Badge variant="outline">{sentRequests.length}件</Badge>
            </div>

            <div className="mt-3 space-y-2">
              {sentRequests.map((req) => {
                const initials = req.targetUser.displayName.slice(0, 2).toUpperCase()
                return (
                  <Card key={req.targetUser.userId} className="border-stone-200 bg-stone-50/60 shadow-none">
                    <CardContent className="flex items-center gap-3 p-4">
                      <PlayerAvatar
                        className="size-11 opacity-90"
                        src={req.targetUser.avatarUrl}
                        initials={initials}
                        name={req.targetUser.displayName}
                      />
                      <div className="min-w-0 flex-1">
                        <p className="truncate text-sm font-medium">{req.targetUser.displayName}</p>
                        <p className="text-xs text-muted-foreground">相手の承認待ちです</p>
                      </div>
                      <Badge variant="secondary" className="text-xs font-normal">
                        申請中
                      </Badge>
                    </CardContent>
                  </Card>
                )
              })}
            </div>
          </section>
        )}

        {/* フレンド一覧 */}
        <section>
          <div className="flex items-center justify-between">
            <h2 className="text-balance text-base font-semibold">フレンド一覧</h2>
            <span className="text-xs text-muted-foreground">{friends.length}人</span>
          </div>

          {isFriendsLoading ? (
            <p className="mt-3 text-xs text-muted-foreground">読み込み中...</p>
          ) : friends.length === 0 ? (
            <div className="mt-3 rounded-xl border bg-white p-6 text-center">
              <p className="text-sm font-medium">まだフレンドがいません</p>
              <p className="mt-1 text-xs text-muted-foreground">
                表示名で検索してフレンド申請を送ってみましょう。
              </p>
            </div>
          ) : (
            <div className="mt-3 overflow-hidden rounded-xl border bg-white">
              {friends.map((friend, index) => {
                const isOnline = friend.presence.state === 'ONLINE'
                const isGame = friend.presence.state === 'IN_GAME'
                const initials = friend.user.displayName.slice(0, 2).toUpperCase()
                const statusLabel =
                  friend.presence.state === 'ONLINE'
                    ? 'オンライン'
                    : friend.presence.state === 'IN_GAME'
                      ? 'ゲーム中'
                      : 'オフライン'

                const inviteState = gameInviteStatusMap[friend.user.userId]

                return (
                  <div key={friend.user.userId}>
                    {index > 0 && <Separator />}
                    <div className="flex items-center gap-3 p-4">
                      <div className="relative">
                        <PlayerAvatar
                          className="size-11"
                          src={friend.user.avatarUrl}
                          initials={initials}
                          name={friend.user.displayName}
                        />
                        <span
                          className={cn(
                            'absolute bottom-0 right-0 size-2.5 rounded-full border-2 border-white',
                            isOnline || isGame ? 'bg-emerald-500' : 'bg-stone-300',
                          )}
                          aria-hidden="true"
                        />
                      </div>
                      <div className="min-w-0 flex-1">
                        <p className="truncate text-sm font-medium">{friend.user.displayName}</p>
                        <p className="text-xs text-muted-foreground">{statusLabel}</p>
                      </div>

                      <Button
                        disabled={inviteState === 'sending'}
                        size="sm"
                        variant="outline"
                        onClick={() => void handleInviteToGame(friend.user)}
                      >
                        {inviteState === 'sending' ? '送信中…' : inviteState === 'invited' ? '再招待' : '招待'}
                      </Button>
                      <RemoveFriendButton
                        friend={friend.user}
                        onRemove={() => handleRemoveFriend(friend.user)}
                      />
                    </div>
                  </div>
                )
              })}
            </div>
          )}
        </section>

        {/* ユーザー検索・フレンド申請セクション */}
        <section className="space-y-3">
          {!showSearch ? (
            <Button
              className="h-11 w-full"
              variant="outline"
              onClick={() => setShowSearch(true)}
            >
              <UserPlus className="size-4" aria-hidden="true" />
              表示名で検索
            </Button>
          ) : (
            <div className="space-y-4 rounded-xl border bg-white p-4">
              <div className="flex items-center justify-between">
                <h3 className="text-sm font-semibold">ユーザー検索</h3>
                <Button
                  size="sm"
                  variant="ghost"
                  onClick={() => {
                    setShowSearch(false)
                    setSearchQuery('')
                    clearSearchResults()
                  }}
                >
                  閉じる
                </Button>
              </div>

              <form onSubmit={handleSearchSubmit} className="flex gap-2">
                <Input
                  className="h-10 flex-1"
                  placeholder="表示名（2文字以上）"
                  value={searchQuery}
                  onChange={(e) => {
                    const val = e.target.value
                    setSearchQuery(val)
                    if (val.trim().length < 2) {
                      clearSearchResults()
                    }
                  }}
                />
                <Button type="submit" disabled={isSearching || searchQuery.trim().length < 2}>
                  <Search className="size-4" />
                </Button>
              </form>

              {isSearching && <p className="text-xs text-muted-foreground">検索中...</p>}

              {searchResults.length > 0 && (
                <div className="space-y-2 pt-2">
                  <p className="text-xs font-medium text-muted-foreground">
                    検索結果 ({searchResults.length}件)
                  </p>
                  {searchResults.map((user) => {
                    const status = requestStatusMap[user.userId]
                    const initials = user.displayName.slice(0, 2).toUpperCase()
                    const isAlreadyFriend = friends.some((f) => f.user.userId === user.userId)

                    return (
                      <div
                        key={user.userId}
                        className="flex items-center justify-between rounded-lg border p-3"
                      >
                        <div className="flex items-center gap-3">
                          <PlayerAvatar
                            className="size-9"
                            src={user.avatarUrl}
                            initials={initials}
                            name={user.displayName}
                          />
                          <p className="text-sm font-medium">{user.displayName}</p>
                        </div>

                        {isAlreadyFriend ? (
                          <Badge variant="outline">フレンド</Badge>
                        ) : status === 'sent' ? (
                          <Badge variant="secondary">申請済み</Badge>
                        ) : (
                          <Button
                            size="sm"
                            disabled={status === 'sending'}
                            onClick={() => handleSendRequest(user)}
                          >
                            {status === 'sending' ? '送信中...' : 'フレンド申請'}
                          </Button>
                        )}
                      </div>
                    )
                  })}
                </div>
              )}
            </div>
          )}
        </section>
      </main>
    </MobileShell>
  )
}
