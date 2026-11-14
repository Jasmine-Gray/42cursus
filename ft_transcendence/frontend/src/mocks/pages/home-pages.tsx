import {
  BriefcaseBusiness,
  Check,
  Copy,
  Clapperboard,
  Gamepad2,
  MapPin,
  PackageOpen,
  DoorOpen,
  ArrowUpRight,
  Utensils,
} from 'lucide-react'
import { useEffect, useState, type FormEvent } from 'react'
import { Link, useLocation } from 'wouter'

import { SectionHeader } from '@/components/game-ui/section-header'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { RoomMenuLayout } from '@/components/game-ui/room-menu-layout'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Badge } from '@/components/ui/badge'
import { Button, buttonVariants } from '@/components/ui/button'
import { Checkbox } from '@/components/ui/checkbox'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { cn } from '@/lib/utils'
import { PlayerRoster } from '@/components/game-ui/player-roster'
import { players } from '@/mocks/data/mock-data'
import {
  publishGameStart,
  subscribeToGameStart,
  type GameViewpoint,
} from '@/mocks/lib/game-start-channel'

const genreOptions = [
  { name: '場所', icon: MapPin },
  { name: '食べ物', icon: Utensils },
  { name: '趣味', icon: Gamepad2 },
  { name: '日用品', icon: PackageOpen },
  { name: '学校・仕事', icon: BriefcaseBusiness },
  { name: 'エンタメ', icon: Clapperboard },
]

export function RoomsPage() {
  const [location, navigate] = useLocation()
  const directInvitePath = /^\/join\/[a-z0-9-]+$/i.test(location)
    ? location
    : ''
  const [inviteUrl, setInviteUrl] = useState(
    directInvitePath ? `${window.location.origin}${directInvitePath}` : '',
  )
  const [error, setError] = useState('')
  const [found, setFound] = useState(Boolean(directInvitePath))

  function handleSearch(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    if (found) {
      navigate('/lobby')
      return
    }
    try {
      const parsedUrl = new URL(inviteUrl.trim())
      const validPath = /^\/join\/[a-z0-9-]+$/i.test(parsedUrl.pathname)
      const localDevelopment =
        ['localhost', '127.0.0.1'].includes(parsedUrl.hostname) &&
        parsedUrl.protocol === 'http:'
      if ((parsedUrl.protocol !== 'https:' && !localDevelopment) || !validPath)
        throw new Error('invalid invite URL')
      setError('')
      setFound(true)
    } catch {
      setFound(false)
      setError('有効な招待URLを入力してください。')
    }
  }

  return (
    <RoomMenuLayout
      mode="join"
      onSubmit={handleSearch}
      actionLabel={found ? '参加' : '確認'}
      actionDisabled={!inviteUrl.trim()}
      secondaryAction={
        found && (
          <Link
            className={cn(
              buttonVariants({ variant: 'outline' }),
              'hunt-play-action room-menu-secondary',
            )}
            href="/mock/game/spectator"
          >
            観戦
            <ArrowUpRight className="size-5" aria-hidden="true" />
          </Link>
        )
      }
      preview={
        found ? (
          <>
            <div className="room-preview-heading">
              <Badge variant="secondary">
                <Check className="size-3" aria-hidden="true" />
                参加可能
              </Badge>
              <span className="room-preview-code">CASE-042</span>
            </div>
            <h2>深夜の読書会</h2>
            <div className="room-preview-tags">
              {['場所', '食べ物', '趣味'].map((genre) => (
                <span key={genre}>{genre}</span>
              ))}
            </div>
            <div className="room-preview-players">
              {players.slice(0, 4).map((player) => (
                <PlayerAvatar
                  key={player.id}
                  src={player.avatarSrc}
                  name={player.name}
                  initials={player.initials}
                />
              ))}
              <span>6 / 8人</span>
            </div>
            <dl className="room-preview-details">
              <div>
                <dt>議論時間</dt>
                <dd>5:00</dd>
              </div>
              <div>
                <dt>観戦</dt>
                <dd>1人</dd>
              </div>
            </dl>
          </>
        ) : (
          <div className="room-preview-empty">
            <DoorOpen aria-hidden="true" />
            <h2>招待された部屋</h2>
            <p>
              URLを確認すると
              <br />
              部屋の情報が表示されます。
            </p>
          </div>
        )
      }
    >
      <div className="room-menu-field">
        <Label htmlFor="invite-url">招待URL</Label>
        <div className="invite-url-row">
          <Input
            id="invite-url"
            className="invite-url-input"
            type="url"
            inputMode="url"
            autoCapitalize="none"
            autoCorrect="off"
            placeholder="https://example.com/join/CASE-042"
            value={inviteUrl}
            aria-invalid={Boolean(error)}
            aria-describedby={error ? 'invite-url-error' : 'invite-url-help'}
            onChange={(event) => {
              setInviteUrl(event.target.value)
              setFound(false)
              if (error) setError('')
            }}
          />
          <InviteCopyButton value={inviteUrl} />
        </div>
        <div className="room-field-feedback">
          {error ? (
            <p id="invite-url-error" className="text-destructive" role="alert">
              {error}
            </p>
          ) : (
            <p id="invite-url-help">
              ホストから受け取ったURLを貼り付けてください。
            </p>
          )}
        </div>
      </div>
    </RoomMenuLayout>
  )
}

export function CreateRoomPage() {
  const [, navigate] = useLocation()
  const [roomName, setRoomName] = useState('深夜の読書会')
  const [selectedGenres, setSelectedGenres] = useState([
    '場所',
    '食べ物',
    '趣味',
  ])
  const [roomNameError, setRoomNameError] = useState('')
  const [genreError, setGenreError] = useState('')

  function handleSubmit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    if (roomName.trim().length < 2) {
      setRoomNameError('部屋名は2文字以上で入力してください。')
      return
    }
    if (selectedGenres.length === 0) {
      setGenreError('お題ジャンルを1つ以上選択してください。')
      return
    }
    navigate('/lobby')
  }

  return (
    <RoomMenuLayout
      mode="create"
      onSubmit={handleSubmit}
      actionLabel="作成"
      preview={
        <>
          <p className="room-preview-label">部屋情報</p>
          <h2>{roomName.trim() || '新しい部屋'}</h2>
          <div className="room-preview-tags">
            {selectedGenres.length ? (
              selectedGenres.map((genre) => <span key={genre}>{genre}</span>)
            ) : (
              <span>ジャンル未選択</span>
            )}
          </div>
          <dl className="room-preview-details">
            <div>
              <dt>参加方法</dt>
              <dd>招待URL</dd>
            </div>
            <div>
              <dt>公開範囲</dt>
              <dd>招待のみ</dd>
            </div>
          </dl>
        </>
      }
    >
      <div className="room-menu-field">
        <Label htmlFor="room-name">部屋名</Label>
        <Input
          id="room-name"
          maxLength={30}
          value={roomName}
          onChange={(event) => {
            setRoomName(event.target.value)
            if (roomNameError) setRoomNameError('')
          }}
          aria-invalid={Boolean(roomNameError)}
          aria-describedby={
            roomNameError ? 'room-name-error' : 'room-name-help'
          }
        />
        <div className="room-field-feedback">
          {roomNameError ? (
            <p id="room-name-error" className="text-destructive" role="alert">
              {roomNameError}
            </p>
          ) : (
            <p id="room-name-help">2〜30文字</p>
          )}
          <span>{roomName.length}/30</span>
        </div>
      </div>

      <fieldset
        className="room-genre-picker"
        aria-describedby={genreError ? 'genre-error' : undefined}
      >
        <legend>お題ジャンル</legend>
        <div className="room-genre-grid">
          {genreOptions.map((genre) => {
            const selected = selectedGenres.includes(genre.name)
            const Icon = genre.icon
            return (
              <label key={genre.name}>
                <Checkbox
                  checked={selected}
                  onCheckedChange={(checked) => {
                    setSelectedGenres((current) =>
                      checked
                        ? [...current, genre.name]
                        : current.filter((item) => item !== genre.name),
                    )
                    if (genreError) setGenreError('')
                  }}
                  aria-label={`${genre.name}ジャンル`}
                />
                <Icon className="size-5" aria-hidden="true" />
                <span>{genre.name}</span>
              </label>
            )
          })}
        </div>
        <div className="room-field-feedback">
          {genreError && (
            <p id="genre-error" className="text-destructive" role="alert">
              {genreError}
            </p>
          )}
        </div>
      </fieldset>
    </RoomMenuLayout>
  )
}

type LobbyMode = 'host' | 'player' | 'spectator'

export function LobbyPage({
  mode = 'host',
  viewpoint = 'citizen',
}: {
  mode?: LobbyMode
  viewpoint?: GameViewpoint
}) {
  const [, navigate] = useLocation()
  const [ready, setReady] = useState(true)
  const inviteUrl = `${window.location.origin}/join/CASE-042`
  const countdownPath =
    viewpoint === 'spectator' ? '/game/countdown/spectator' : '/game/countdown'
  const currentPlayerId =
    mode === 'player' ? 'minato' : mode === 'host' ? 'self' : null
  const lobbyPlayers = players.map((player) => ({
    ...player,
    isCurrentUser: player.id === currentPlayerId,
    ready: player.id === currentPlayerId ? ready : player.ready,
  }))

  useEffect(() => {
    if (mode === 'host') return
    return subscribeToGameStart((signal) => {
      navigate(`${countdownPath}?startsAt=${signal.startsAt}`)
    })
  }, [countdownPath, mode, navigate])

  function startGame() {
    sessionStorage.removeItem('mock-discussion-deadline')
    const signal = {
      type: 'game-started' as const,
      id: crypto.randomUUID(),
      startsAt: Date.now() + 300,
    }
    publishGameStart(signal)
    navigate(`/mock/game/countdown?startsAt=${signal.startsAt}`)
  }

  return (
    <MobileShell title="深夜の読書会" backHref="/rooms">
      <video
        hidden
        aria-hidden="true"
        preload="auto"
        muted
        playsInline
        src="/cinematics/entrance.mp4"
      />
      <main className="game-lobby hunt-lobby space-y-6 px-5 py-5">
        <p className="lobby-summary">
          参加 6 / 8<span aria-hidden="true"> · </span>観戦 1
          <span aria-hidden="true"> · </span>3ジャンル
        </p>

        <aside className="lobby-details" aria-label="部屋情報">
          <div className="lobby-invite">
            <Input aria-label="招待URL" readOnly value={inviteUrl} />
            <InviteCopyButton value={inviteUrl} />
          </div>
          <dl className="lobby-settings">
            <div>
              <dt>ジャンル</dt>
              <dd>場所・食べ物・趣味</dd>
            </div>
            <div>
              <dt>近さ</dt>
              <dd>標準</dd>
            </div>
            <div>
              <dt>議論</dt>
              <dd>5:00</dd>
            </div>
            <div>
              <dt>定員</dt>
              <dd>4〜8人</dd>
            </div>
          </dl>
          {mode === 'host' && (
            <Link
              className={buttonVariants({ variant: 'ghost', size: 'sm' })}
              href="/rooms/new"
            >
              設定
            </Link>
          )}
        </aside>

        <section className="lobby-players">
          <SectionHeader
            title="参加プレイヤー"
            meta={<Badge variant="outline">4〜8人</Badge>}
          />
          <div className="mt-3">
            <PlayerRoster
              players={lobbyPlayers}
              showReady
              onReadyChange={
                mode === 'spectator'
                  ? undefined
                  : () => setReady((current) => !current)
              }
            />
          </div>
        </section>

        {mode === 'host' ? (
          <div className="lobby-start">
            <Button
              className="h-11 w-full"
              disabled={!ready}
              onClick={startGame}
            >
              START
            </Button>
          </div>
        ) : (
          <div
            className="bg-night rounded-xl border px-4 py-5 text-center text-white"
            role="status"
          >
            <p className="font-semibold">ホストの開始を待っています</p>
          </div>
        )}
      </main>
    </MobileShell>
  )
}

function InviteCopyButton({ value }: { value: string }) {
  const [status, setStatus] = useState<'idle' | 'copied' | 'failed'>('idle')
  useEffect(() => {
    setStatus('idle')
  }, [value])
  useEffect(() => {
    if (status === 'idle') return
    const timeout = window.setTimeout(() => setStatus('idle'), 2500)
    return () => window.clearTimeout(timeout)
  }, [status])
  async function copy() {
    try {
      await navigator.clipboard.writeText(value)
      setStatus('copied')
    } catch {
      setStatus('failed')
    }
  }
  return (
    <span className="invite-copy-control">
      <Button
        type="button"
        className="invite-copy-button"
        variant="outline"
        disabled={!value.trim()}
        onClick={() => void copy()}
      >
        {status === 'copied' ? (
          <Check className="size-4" aria-hidden="true" />
        ) : (
          <Copy className="size-4" aria-hidden="true" />
        )}
        コピー
      </Button>
      <span className="invite-copy-status" role="status">
        {status === 'copied'
          ? 'コピーしました'
          : status === 'failed'
            ? 'URLを選択してコピーしてください'
            : ''}
      </span>
    </span>
  )
}
