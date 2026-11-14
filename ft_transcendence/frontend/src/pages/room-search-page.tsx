import { Check, Eye, Loader2, Search, Users, XCircle } from 'lucide-react'
import { useEffect, useState, type FormEvent } from 'react'
import { Link, useLocation, useParams, useSearch } from 'wouter'

import { MobileShell } from '@/components/layout/mobile-shell'
import { Badge } from '@/components/ui/badge'
import { Button, buttonVariants } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { api } from '@/lib/api-client'
import { inviteParticipantType } from '@/lib/room-invite'
import { cn } from '@/lib/utils'
import type { components } from '@/types/api'

type RoomPreview = components['schemas']['RoomPreview']
type Genre = components['schemas']['Genre']

const genreLabels: Record<Genre, string> = {
  ROMANCE: '恋愛',
  VALUES: '本音・価値観',
  PLACE: '場所',
  FOOD: '食べ物',
  HOBBY: '趣味',
  DAILY_ITEM: '日用品',
  WORK_SCHOOL: '学校・仕事',
  ENTERTAINMENT: 'エンタメ',
}

function parseInviteCode(input: string): string | null {
  const trimmed = input.trim()
  if (!trimmed) return null

  // CASE-042 や abc-123 など直接コードが入力された場合
  if (/^[A-Za-z0-9-]+$/i.test(trimmed) && !trimmed.includes('/')) {
    return trimmed
  }

  // URL形式: https://.../join/CASE-042 または /join/CASE-042
  try {
    const url =
      trimmed.startsWith('http://') || trimmed.startsWith('https://')
        ? new URL(trimmed)
        : new URL(trimmed, window.location.origin)
    const match = url.pathname.match(/\/join\/([A-Za-z0-9-]+)/i)
    if (match) return match[1]
  } catch {
    // URLパース失敗時は null
  }

  return null
}

export function RoomSearchPage() {
  const [location, navigate] = useLocation()
  const params = useParams<{ code?: string }>()
  const search = useSearch()
  const mode = new URLSearchParams(search).get('mode')
  const routeCode =
    params?.code ||
    (location.startsWith('/join/') ? location.replace('/join/', '') : '')

  const [inviteInput, setInviteInput] = useState(
    routeCode ? `${window.location.origin}/join/${routeCode}` : '',
  )
  const invitedAs =
    mode === 'spectate'
      ? 'SPECTATOR'
      : mode === 'player'
        ? 'PLAYER'
        : inviteParticipantType(inviteInput)
  const spectateOnly = invitedAs === 'SPECTATOR'
  const playerOnly = invitedAs === 'PLAYER'
  const [roomPreview, setRoomPreview] = useState<RoomPreview | null>(null)
  const [isSearching, setIsSearching] = useState(false)
  const [isJoining, setIsJoining] = useState(false)
  const [error, setError] = useState('')
  const [joinError, setJoinError] = useState('')

  async function fetchRoomByCode(code: string) {
    setIsSearching(true)
    setError('')
    setJoinError('')
    setRoomPreview(null)

    try {
      const res = await api.GET('/invites/{inviteCode}', {
        params: { path: { inviteCode: code } },
      })

      if (res.error || !res.data) {
        if (res.response?.status === 404) {
          setError('指定された招待コードの部屋が見つかりません。')
        } else if (res.response?.status === 401) {
          navigate('/login')
        } else {
          setError('部屋情報の取得に失敗しました。')
        }
        return
      }

      setRoomPreview(res.data.data)
    } catch {
      setError('通信エラーが発生しました。')
    } finally {
      setIsSearching(false)
    }
  }

  useEffect(() => {
    if (routeCode) {
      const code = parseInviteCode(routeCode)
      if (code) {
        void fetchRoomByCode(code)
      }
    }
  }, [routeCode])

  function handleSearch(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    const code = parseInviteCode(inviteInput)
    if (!code) {
      setError('有効な招待URLまたはコード（例: CASE-042）を入力してください。')
      setRoomPreview(null)
      return
    }

    void fetchRoomByCode(code)
  }

  async function handleJoin(
    participantType: components['schemas']['ParticipantType'],
  ) {
    if (!roomPreview || isJoining) return
    setIsJoining(true)
    setJoinError('')

    try {
      const res = await api.POST('/rooms/{roomId}/join', {
        params: { path: { roomId: roomPreview.roomId } },
        body: { participantType },
      })

      if (res.error || !res.data) {
        if (res.error?.error.code === 'GAME_IN_PROGRESS') {
          setJoinError('ゲームが始まっています。観戦者として参加できます。')
          setRoomPreview({ ...roomPreview, status: 'IN_GAME', joinable: false })
        } else if (res.error?.error.code === 'INVALID_PHASE') {
          setJoinError('ホストを引き継ぐプレイヤーが必要です。')
        } else if (res.response?.status === 409) {
          setJoinError('プレイヤーは満員です。観戦者として参加できます。')
          setRoomPreview({ ...roomPreview, joinable: false })
        } else if (res.response?.status === 401) {
          navigate('/login')
        } else if (res.response?.status === 404) {
          setJoinError('この部屋は終了したか、見つかりませんでした。')
        } else {
          setJoinError('入室できませんでした。もう一度お試しください。')
        }
        return
      }

      const joinedRoom = res.data.data.room
      const destination = joinedRoom.status === 'IN_GAME' ? '/game' : '/lobby'
      navigate(`${destination}?roomId=${encodeURIComponent(joinedRoom.roomId)}`)
    } catch {
      setJoinError('通信エラーが発生しました。もう一度お試しください。')
    } finally {
      setIsJoining(false)
    }
  }

  const isPlayerFull = roomPreview
    ? roomPreview.playerCount >= roomPreview.maxPlayers
    : false
  const canJoinAsPlayer = roomPreview
    ? roomPreview.joinable && !isPlayerFull && roomPreview.status === 'WAITING'
    : false
  const canSpectate = roomPreview ? roomPreview.status !== 'CLOSED' : false

  return (
    <MobileShell
      title={spectateOnly ? '観戦する' : '招待URLで参加'}
      backHref="/home"
      bottomNav
    >
      <main className="space-y-6 px-5 py-6">
        <section>
          <h2 className="text-xl font-semibold text-balance">
            {spectateOnly ? '招待URLで観戦する' : '招待URLを入力してください'}
          </h2>
          <p className="text-muted-foreground mt-2 text-sm leading-6 text-pretty">
            {spectateOnly
              ? '招待URLまたは招待コードを入力してください。満員でも、ゲームの途中からでも観戦できます。'
              : '共有されたURLまたは招待コードから、プレイヤー参加と観戦を選べます。'}
          </p>
        </section>

        <form className="space-y-3" onSubmit={handleSearch} noValidate>
          <div>
            <Label htmlFor="invite-url">招待URL または 招待コード</Label>
            <Input
              id="invite-url"
              className="mt-2 h-11 bg-white"
              type="text"
              autoCapitalize="none"
              autoCorrect="off"
              placeholder="https://.../join/CASE-042 または CASE-042"
              value={inviteInput}
              aria-invalid={Boolean(error)}
              aria-describedby={error ? 'invite-url-error' : 'invite-url-help'}
              onChange={(event) => {
                setInviteInput(event.target.value)
                setRoomPreview(null)
                if (error) setError('')
              }}
            />
            {error ? (
              <p
                id="invite-url-error"
                className="text-destructive mt-2 text-xs"
                role="alert"
              >
                {error}
              </p>
            ) : (
              <p
                id="invite-url-help"
                className="text-muted-foreground mt-2 text-xs"
              >
                URLはホストまたはフレンドから受け取れます。
              </p>
            )}
          </div>
          <Button
            className="h-11 w-full"
            type="submit"
            disabled={!inviteInput.trim() || isSearching}
          >
            {isSearching ? (
              <Loader2
                className="mr-1 size-4 animate-spin"
                aria-hidden="true"
              />
            ) : (
              <Search className="mr-1 size-4" aria-hidden="true" />
            )}
            部屋を検索
          </Button>
        </form>

        {roomPreview && (
          <Card
            className={cn(
              'border shadow-none',
              canJoinAsPlayer
                ? 'border-primary/30 bg-white'
                : 'border-amber-200 bg-amber-50/40',
            )}
          >
            <CardContent className="space-y-4 p-4">
              <div className="flex items-center justify-between gap-3">
                {canJoinAsPlayer ? (
                  <Badge
                    variant="secondary"
                    className="bg-emerald-100 text-emerald-800 hover:bg-emerald-100"
                  >
                    <Check className="mr-1 size-3" aria-hidden="true" />
                    参加可能（空きあり）
                  </Badge>
                ) : roomPreview.status === 'CLOSED' ? (
                  <Badge variant="outline">終了</Badge>
                ) : roomPreview.status === 'IN_GAME' ? (
                  <Badge variant="outline">進行中（観戦できます）</Badge>
                ) : isPlayerFull ? (
                  <Badge variant="destructive">
                    <XCircle className="mr-1 size-3" aria-hidden="true" />
                    満員（観戦できます）
                  </Badge>
                ) : (
                  <Badge variant="outline">観戦できます</Badge>
                )}
                <span className="text-muted-foreground font-mono text-xs font-semibold tabular-nums">
                  {roomPreview.inviteCode}
                </span>
              </div>

              <div>
                <h2 className="text-lg font-semibold text-balance">
                  {roomPreview.roomName}
                </h2>
                <div className="mt-2 flex flex-wrap gap-1.5">
                  {roomPreview.genres.map((genre) => (
                    <Badge
                      key={genre}
                      variant="outline"
                      className="bg-white text-xs"
                    >
                      {genreLabels[genre] ?? genre}
                    </Badge>
                  ))}
                </div>
              </div>

              <div className="text-muted-foreground flex items-center gap-4 border-t pt-3 text-xs">
                <span className="text-foreground inline-flex items-center gap-1 font-medium">
                  <Users
                    className="text-muted-foreground size-3.5"
                    aria-hidden="true"
                  />
                  プレイヤー: {roomPreview.playerCount} /{' '}
                  {roomPreview.maxPlayers}人
                </span>
                <span className="inline-flex items-center gap-1">
                  <Eye
                    className="text-muted-foreground size-3.5"
                    aria-hidden="true"
                  />
                  観戦者: {roomPreview.spectatorCount}人
                </span>
              </div>

              {!canJoinAsPlayer && isPlayerFull && canSpectate && (
                <div className="rounded-lg bg-amber-100/70 p-2.5 text-xs text-amber-900">
                  ⚠️ プレイヤー定員（{roomPreview.maxPlayers}
                  名）に達しているため、プレイヤーとしては参加できません。観戦者として参加できます。
                </div>
              )}

              <p className="text-muted-foreground text-xs leading-5 text-pretty">
                観戦者は全員のお題と役職、会話と投票の進行を見られます。発言・投票はできません。
              </p>
              {joinError && (
                <p className="text-destructive text-sm" role="alert">
                  {joinError}
                </p>
              )}
              <div
                className={cn('grid gap-2 pt-1', !invitedAs && 'grid-cols-2')}
              >
                {!spectateOnly && (
                  <Button
                    className="w-full"
                    disabled={!canJoinAsPlayer || isJoining}
                    onClick={() => void handleJoin('PLAYER')}
                  >
                    {isJoining ? (
                      <Loader2 className="mr-1 size-4 animate-spin" />
                    ) : null}
                    プレイヤー参加
                  </Button>
                )}
                {!playerOnly && (
                  <Button
                    className="w-full"
                    variant={spectateOnly ? 'default' : 'outline'}
                    disabled={!canSpectate || isJoining}
                    onClick={() => void handleJoin('SPECTATOR')}
                  >
                    <Eye className="mr-1 size-4" aria-hidden="true" />
                    観戦する
                  </Button>
                )}
                {playerOnly && !canJoinAsPlayer && canSpectate && (
                  <Button
                    variant="outline"
                    onClick={() => void handleJoin('SPECTATOR')}
                    disabled={isJoining}
                  >
                    <Eye className="mr-1 size-4" aria-hidden="true" />
                    観戦者として入る
                  </Button>
                )}
              </div>
            </CardContent>
          </Card>
        )}

        <div className="border-t pt-5 text-center">
          <p className="text-muted-foreground text-sm">
            招待URLを持っていませんか？
          </p>
          <Link
            className={cn(
              buttonVariants({ variant: 'outline' }),
              'mt-3 h-11 w-full',
            )}
            href="/rooms/new"
          >
            新しい部屋を作る
          </Link>
        </div>
      </main>
    </MobileShell>
  )
}
