import { useId, useState, type FormEvent } from 'react'
import { RoomTopicFields } from '@/components/game-ui/room-topic-fields'
import { Label } from '@/components/ui/label'
import {
  RoomDurationSelect,
  settingsSelectClassName,
} from '@/components/game-ui/room-duration-select'
import { Button } from '@/components/ui/button'
import { validRoomTiming } from '@/components/game-ui/room-timing-fields'
import { api } from '@/lib/api-client'
import type { components } from '@/types/api'

type RoomState = components['schemas']['RoomState']

export function RoomSettingsEditor({
  room,
  onSaved,
}: {
  room: RoomState
  onSaved: (room: RoomState) => void
}) {
  const idPrefix = useId()
  const [selectedGenres, setSelectedGenres] = useState(room.settings.genres)
  const [comebackEnabled, setComebackEnabled] = useState(
    room.settings.comebackEnabled,
  )
  const [genreError, setGenreError] = useState('')
  const [saved, setSaved] = useState(false)
  const [timing, setTiming] = useState({
    discussionMs: room.settings.discussionMs,
    voteMs: room.settings.voteMs,
  })
  const [saving, setSaving] = useState(false)
  const [error, setError] = useState('')
  const changed =
    selectedGenres.length !== room.settings.genres.length ||
    selectedGenres.some((genre) => !room.settings.genres.includes(genre)) ||
    comebackEnabled !== room.settings.comebackEnabled ||
    timing.discussionMs !== room.settings.discussionMs ||
    timing.voteMs !== room.settings.voteMs

  async function save(event: FormEvent) {
    event.preventDefault()
    if (saving || room.status !== 'WAITING') return
    setSaved(false)
    if (selectedGenres.length === 0) {
      setGenreError('お題ジャンルを1つ以上選択してください。')
      return
    }
    if (!validRoomTiming(timing)) {
      setError('議論時間は30〜900秒、投票時間は10〜180秒で入力してください。')
      return
    }
    setSaving(true)
    setError('')
    try {
      const { data, error: failure } = await api.PUT(
        '/rooms/{roomId}/settings',
        {
          params: { path: { roomId: room.roomId } },
          body: {
            roomName: room.settings.roomName,
            genres: selectedGenres,
            comebackEnabled,
            ...timing,
          },
        },
      )
      if (failure || !data) {
        setError(
          failure?.error.code === 'INVALID_PHASE'
            ? 'ゲームが始まったため、設定を変更できません。'
            : failure?.error.code === 'NOT_HOST'
              ? '設定を変更できるのは現在のホストだけです。'
              : '設定を保存できませんでした。もう一度お試しください。',
        )
        return
      }
      onSaved(data.data)
      setSaved(true)
    } catch {
      setError('通信エラーが発生しました。もう一度お試しください。')
    } finally {
      setSaving(false)
    }
  }

  return (
    <form onSubmit={save} noValidate className="space-y-4">
      <div className="divide-y text-sm">
        <div className="py-3">
          <RoomTopicFields
            selectedGenres={selectedGenres}
            onGenresChange={setSelectedGenres}
            comebackEnabled={comebackEnabled}
            onComebackChange={setComebackEnabled}
            genreError={genreError}
            onEdited={() => setGenreError('')}
            disabled={saving || room.status !== 'WAITING'}
            idPrefix={idPrefix}
            showComeback={false}
          />
        </div>
        <div className="py-3">
          <div className="flex items-center justify-between gap-3">
            <Label
              htmlFor={`${idPrefix}-comeback`}
              className="text-muted-foreground"
            >
              逆転モード
            </Label>
            <select
              id={`${idPrefix}-comeback`}
              className={settingsSelectClassName}
              value={comebackEnabled ? 'enabled' : 'disabled'}
              onChange={(event) =>
                setComebackEnabled(event.target.value === 'enabled')
              }
              disabled={saving || room.status !== 'WAITING'}
              aria-describedby={`${idPrefix}-comeback-help`}
            >
              <option value="disabled">なし</option>
              <option value="enabled">あり</option>
            </select>
          </div>
          <p
            id={`${idPrefix}-comeback-help`}
            className="text-muted-foreground mt-2 text-xs leading-5 text-pretty"
          >
            見つかったウルフが30秒以内に市民のお題を当てたら逆転勝利。回答は1回、入力途中の文字も全員に見えます。
          </p>
        </div>
        <div className="flex items-start justify-between gap-3 py-3">
          <Label
            htmlFor={`${idPrefix}-discussion`}
            className="text-muted-foreground pt-3"
          >
            議論時間
          </Label>
          <RoomDurationSelect
            id={`${idPrefix}-discussion`}
            label="議論時間"
            min={30}
            max={900}
            step={30}
            value={timing.discussionMs}
            disabled={saving || room.status !== 'WAITING'}
            onChange={(discussionMs) => setTiming({ ...timing, discussionMs })}
          />
        </div>
        <div className="flex items-start justify-between gap-3 py-3">
          <Label
            htmlFor={`${idPrefix}-vote`}
            className="text-muted-foreground pt-3"
          >
            投票時間
          </Label>
          <RoomDurationSelect
            id={`${idPrefix}-vote`}
            label="投票時間"
            min={10}
            max={180}
            step={5}
            value={timing.voteMs}
            disabled={saving || room.status !== 'WAITING'}
            onChange={(voteMs) => setTiming({ ...timing, voteMs })}
          />
        </div>
        <div className="flex items-center justify-between gap-3 py-3">
          <span className="text-muted-foreground">参加人数</span>
          <span className="font-mono font-medium tabular-nums">
            {room.settings.minPlayers}〜{room.settings.maxPlayers}人
          </span>
        </div>
      </div>
      <p className="text-muted-foreground text-xs">
        待機中にホストが変更できます。保存した設定は次のゲームから使われます。
      </p>
      {saved && !changed && (
        <p role="status" className="text-sm">
          設定を保存しました。
        </p>
      )}
      {error && (
        <p role="alert" className="text-destructive text-sm">
          {error}
        </p>
      )}
      <Button
        type="submit"
        variant="outline"
        className="w-full"
        disabled={!changed || saving || room.status !== 'WAITING'}
      >
        {saving ? '保存しています…' : '設定を保存'}
      </Button>
    </form>
  )
}
