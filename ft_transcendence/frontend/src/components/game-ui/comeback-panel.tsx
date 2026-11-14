import { useEffect, useRef, useState, type FormEvent } from 'react'
import { PencilLine } from 'lucide-react'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input'
import type { useRoomChannel } from '@/hooks/use-room-channel'
import type { ComebackState, RoomStateSnapshot } from '@/types/ws/events'

type ComebackPanelProps = Pick<
  ReturnType<typeof useRoomChannel>,
  'updateComebackDraft' | 'submitComeback' | 'sendError' | 'status'
> & {
  comeback: ComebackState
  players: RoomStateSnapshot['players']
  meId?: string
}

export function ComebackPanel({
  comeback,
  players,
  meId,
  updateComebackDraft,
  submitComeback,
  sendError,
  status,
}: ComebackPanelProps) {
  const [content, setContent] = useState(comeback.content)
  const [submitting, setSubmitting] = useState(false)
  const [submitted, setSubmitted] = useState(false)
  const draftTimer = useRef<ReturnType<typeof setTimeout> | null>(null)
  const composing = useRef(false)
  const wolf = players.find((player) => player.userId === comeback.wolfUserId)
  const canAnswer = meId === comeback.wolfUserId

  useEffect(
    () => () => {
      if (draftTimer.current) clearTimeout(draftTimer.current)
    },
    [],
  )

  function changeDraft(value: string) {
    setContent(value)
    if (draftTimer.current) clearTimeout(draftTimer.current)
    draftTimer.current = setTimeout(() => updateComebackDraft(value), 180)
  }

  async function submit(event: FormEvent) {
    event.preventDefault()
    if (
      composing.current ||
      !content.trim() ||
      submitting ||
      submitted ||
      !canAnswer
    )
      return
    if (draftTimer.current) clearTimeout(draftTimer.current)
    setSubmitting(true)
    try {
      setSubmitted(await submitComeback(content))
    } finally {
      setSubmitting(false)
    }
  }

  return (
    <section
      className="mx-auto w-full max-w-xl space-y-6"
      aria-labelledby="comeback-title"
    >
      <div className="text-center">
        {wolf && (
          <PlayerAvatar
            className="mx-auto size-20"
            src={wolf.avatarUrl}
            initials={wolf.displayName.slice(0, 2)}
            name={wolf.displayName}
          />
        )}
        <p className="text-primary mt-4 text-sm font-semibold">
          最後の逆転チャンス
        </p>
        <h2
          id="comeback-title"
          className="mt-2 text-2xl font-semibold text-balance"
        >
          {canAnswer
            ? '市民のお題は？'
            : `${wolf?.displayName ?? 'ウルフ'}が回答中`}
        </h2>
        <p className="text-muted-foreground mt-3 text-sm leading-6">
          30秒以内に、1回だけ回答できます。正解すればウルフの逆転勝利です。
        </p>
      </div>
      <div className="border-primary/30 bg-primary/5 rounded-xl border p-6">
        <p className="text-muted-foreground flex items-center gap-2 text-xs">
          <PencilLine className="size-4" aria-hidden="true" />
          全員に見えている回答
        </p>
        <p
          className="mt-4 min-h-12 text-center text-2xl leading-relaxed break-words"
          role="status"
          aria-live="polite"
          aria-label="入力中の回答"
        >
          {comeback.content || 'まだ文字は書かれていません…'}
        </p>
      </div>
      {canAnswer ? (
        <form className="space-y-3" onSubmit={submit}>
          <label htmlFor="comeback-answer" className="text-sm font-medium">
            市民側のお題を入力
          </label>
          <Input
            id="comeback-answer"
            autoComplete="off"
            maxLength={80}
            value={content}
            disabled={submitting || submitted || status !== 'ready'}
            onCompositionStart={() => {
              composing.current = true
            }}
            onCompositionEnd={() => {
              composing.current = false
            }}
            onChange={(event) => changeDraft(event.target.value)}
            aria-describedby="comeback-answer-help"
          />
          <p
            id="comeback-answer-help"
            className="text-muted-foreground text-xs leading-5"
          >
            入力途中の文字も全員に公開されます。全角・半角、ひらがな・カタカナ、空白の違いは同じ回答として扱います。
          </p>
          <Button
            className="h-11 w-full"
            type="submit"
            disabled={
              !content.trim() || submitting || submitted || status !== 'ready'
            }
          >
            {submitting
              ? '判定中…'
              : submitted
                ? '回答しました'
                : 'この回答で勝負する'}
          </Button>
          {sendError && (
            <p className="text-destructive text-sm" role="alert">
              回答を送信できませんでした。接続と残り時間を確認してください。
            </p>
          )}
        </form>
      ) : (
        <p className="text-muted-foreground text-center text-sm">
          入力の様子を見守りましょう。回答が確定すると結果が表示されます。
        </p>
      )}
    </section>
  )
}
