import { Loader2 } from 'lucide-react'
import {
  RoomTopicFields,
  genreOptions,
} from '@/components/game-ui/room-topic-fields'
import { useState, type FormEvent } from 'react'
import { useLocation } from 'wouter'

import {
  RoomTimingFields,
  validRoomTiming,
  formatRoomDuration,
} from '@/components/game-ui/room-timing-fields'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { Separator } from '@/components/ui/separator'
import { api } from '@/lib/api-client'
import type { components } from '@/types/api'

type Genre = components['schemas']['RoomGenre']

export function RoomCreatePage() {
  const [, navigate] = useLocation()
  const [roomName, setRoomName] = useState('深夜の読書会')
  const [selectedGenres, setSelectedGenres] = useState<Genre[]>(() =>
    genreOptions.map((genre) => genre.id),
  )
  const [comebackEnabled, setComebackEnabled] = useState(false)
  const [timing, setTiming] = useState({ discussionMs: 300000, voteMs: 30000 })
  const [roomNameError, setRoomNameError] = useState('')
  const [genreError, setGenreError] = useState('')
  const [apiError, setApiError] = useState('')
  const [isSubmitting, setIsSubmitting] = useState(false)

  async function handleSubmit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    setRoomNameError('')
    setGenreError('')
    setApiError('')

    if (roomName.trim().length < 2) {
      setRoomNameError('部屋名は2文字以上で入力してください。')
      return
    }
    if (selectedGenres.length === 0) {
      setGenreError('お題ジャンルを1つ以上選択してください。')
      return
    }

    if (!validRoomTiming(timing)) {
      setApiError(
        '議論時間は30〜900秒、投票時間は10〜180秒で入力してください。',
      )
      return
    }

    setIsSubmitting(true)

    try {
      const res = await api.POST('/rooms', {
        body: {
          roomName: roomName.trim(),
          genres: selectedGenres,
          comebackEnabled,
          ...timing,
        },
      })

      if (res.error || !res.data) {
        if (res.response?.status === 401) {
          navigate('/login')
        } else {
          setApiError('部屋の作成に失敗しました。')
        }
        return
      }

      const createdRoom = res.data.data
      navigate(`/lobby?roomId=${createdRoom.roomId}`)
    } catch {
      setApiError('通信エラーが発生しました。')
    } finally {
      setIsSubmitting(false)
    }
  }

  return (
    <MobileShell title="部屋を作る" backHref="/home">
      <main className="space-y-6 px-5 py-6">
        {apiError && (
          <div
            role="alert"
            className="text-destructive rounded-lg border border-red-200 bg-red-50 p-3 text-xs"
          >
            {apiError}
          </div>
        )}

        <form className="space-y-5" onSubmit={handleSubmit} noValidate>
          <div>
            <Label htmlFor="room-name">部屋名</Label>
            <Input
              id="room-name"
              className="mt-2 h-11 bg-white"
              maxLength={30}
              value={roomName}
              onChange={(event) => {
                setRoomName(event.target.value)
                if (roomNameError) setRoomNameError('')
              }}
              aria-invalid={Boolean(roomNameError)}
              aria-describedby={roomNameError ? 'room-name-error' : undefined}
            />
            <div className="mt-2 flex justify-between gap-3 text-xs">
              {roomNameError ? (
                <p
                  id="room-name-error"
                  className="text-destructive"
                  role="alert"
                >
                  {roomNameError}
                </p>
              ) : (
                <p className="text-muted-foreground">2〜30文字</p>
              )}
              <span className="text-muted-foreground font-mono tabular-nums">
                {roomName.length}/30
              </span>
            </div>
          </div>

          <RoomTopicFields
            selectedGenres={selectedGenres}
            onGenresChange={setSelectedGenres}
            comebackEnabled={comebackEnabled}
            onComebackChange={setComebackEnabled}
            genreError={genreError}
            onEdited={() => setGenreError('')}
            disabled={isSubmitting}
            idPrefix="create-topics"
          />

          <RoomTimingFields
            value={timing}
            onChange={setTiming}
            disabled={isSubmitting}
            idPrefix="create-timing"
          />

          <div className="rounded-xl border bg-white p-4 text-sm">
            <div className="flex items-center justify-between">
              <span className="text-muted-foreground">参加方法</span>
              <span className="font-medium">招待URL / 招待コード</span>
            </div>
            <Separator className="my-3" />
            <div className="flex items-center justify-between">
              <span className="text-muted-foreground">議論時間</span>
              <span className="font-mono font-medium tabular-nums">
                {Number.isFinite(timing.discussionMs)
                  ? formatRoomDuration(timing.discussionMs)
                  : '—'}
              </span>
            </div>
            <Separator className="my-3" />
            <div className="flex items-center justify-between">
              <span className="text-muted-foreground">定員</span>
              <span className="font-mono font-medium">3〜8人</span>
            </div>
            <Separator className="my-3" />
            <div className="flex items-center justify-between">
              <span className="text-muted-foreground">逆転モード</span>
              <span className="font-medium">
                {comebackEnabled ? 'あり' : 'なし'}
              </span>
            </div>
          </div>

          <Button className="h-11 w-full" type="submit" disabled={isSubmitting}>
            {isSubmitting ? (
              <Loader2
                className="mr-1 size-4 animate-spin"
                aria-hidden="true"
              />
            ) : null}
            部屋を作成してロビーへ
          </Button>
        </form>
      </main>
    </MobileShell>
  )
}
