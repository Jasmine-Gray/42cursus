import { LockKeyhole, SendHorizontal } from 'lucide-react'
import {
  useEffect,
  useRef,
  useState,
  type FormEvent,
  type KeyboardEvent,
} from 'react'

import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { Button } from '@/components/ui/button'
import { Textarea } from '@/components/ui/textarea'
import { useMe } from '@/hooks/use-me'
import { useRoomChannel, type SendErrorCode } from '@/hooks/use-room-channel'
import type { RoomStateSnapshot } from '@/types/ws/events'
import { cn } from '@/lib/utils'

type ChatThreadProps = {
  roomId: string
  gameId: string
  /** 観戦者は閲覧のみ（入力欄を出さない）。 */
  readOnly?: boolean
}

const MAX_LENGTH = 300
const SHORTCUT_HINT_ID = 'chat-shortcut-hint'
// Mac / iOS は ⌘、それ以外は Ctrl を案内する（判定はどちらのキーでも送信できる）。
const SEND_MODIFIER_LABEL = isApplePlatform() ? '⌘' : 'Ctrl'

export function ChatThread({ roomId, gameId, readOnly }: ChatThreadProps) {
  const { user } = useMe()
  const channel = useRoomChannel(roomId, gameId)
  return <ChatThreadView {...channel} user={user} readOnly={readOnly} />
}

export function ChatThreadView({
  messages,
  status,
  sendError,
  send,
  user,
  readOnly,
  players = [],
}: Pick<
  ReturnType<typeof useRoomChannel>,
  'messages' | 'status' | 'sendError' | 'send'
> & {
  user: ReturnType<typeof useMe>['user']
  readOnly?: boolean
  players?: RoomStateSnapshot['players']
}) {
  const [content, setContent] = useState('')
  const [formError, setFormError] = useState('')
  const logRef = useRef<HTMLDivElement>(null)

  useEffect(() => {
    const log = logRef.current
    if (log) log.scrollTop = log.scrollHeight
  }, [messages])

  function handleSubmit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    const trimmed = content.trim()

    if (!trimmed) {
      setFormError('メッセージを入力してください。')
      return
    }
    if (trimmed.length > MAX_LENGTH) {
      setFormError(`メッセージは${MAX_LENGTH}文字以内で入力してください。`)
      return
    }

    send(trimmed)
    setContent('')
    setFormError('')
  }

  const canSend = content.trim() !== '' && status === 'ready'

  // Enter 単体・Shift + Enter は textarea の既定動作（改行）に任せ、
  // ⌘ + Enter / Ctrl + Enter のときだけ送信する。
  // IME 変換中の Enter（isComposing）は変換の確定なので送信しない。
  function handleKeyDown(event: KeyboardEvent<HTMLTextAreaElement>) {
    if (event.key !== 'Enter' || !(event.metaKey || event.ctrlKey)) return
    if (event.nativeEvent.isComposing) return

    event.preventDefault()
    // 送信ボタンが押せない状態（空・未接続）ではショートカットも効かせない
    if (canSend) event.currentTarget.form?.requestSubmit()
  }

  const errorText = formError || (sendError ? sendErrorText(sendError) : '')

  return (
    <section className="discussion-chat" aria-labelledby="chat-title">
      <div>
        <h2 id="chat-title" className="text-base font-semibold text-balance">
          チャット
        </h2>
        <p className="text-muted-foreground text-xs">
          全員に公開されるチャット
        </p>
      </div>

      {status === 'error' && (
        <p
          className="bg-muted text-muted-foreground mt-3 rounded-lg border px-3 py-2 text-xs"
          role="status"
        >
          接続が不安定です。再接続を試みています。
        </p>
      )}

      <div
        ref={logRef}
        className="mt-4 max-h-96 space-y-4 overflow-y-auto"
        role="log"
        aria-live="polite"
        aria-relevant="additions"
        aria-label="チャットメッセージ"
      >
        {messages.map((message) => {
          const ownMessage =
            message.pending === true ||
            (!!user && message.userId === user.userId)
          const name =
            ownMessage && user ? user.displayName : message.displayName

          return (
            <article
              key={message.clientMessageId ?? message.messageId}
              className={cn(
                'chat-message flex items-end gap-2.5',
                ownMessage && 'flex-row-reverse',
              )}
            >
              <PlayerAvatar
                className="size-9"
                src={
                  players.find(
                    (player) =>
                      player.userId ===
                      (ownMessage ? user?.userId : message.userId),
                  )?.avatarUrl
                }
                initials={initialsOf(name)}
                name={name || '不明'}
              />
              <div className="max-w-[78%] min-w-0">
                <div
                  className={cn(
                    'mb-1 flex items-baseline gap-2 px-1',
                    ownMessage && 'justify-end',
                  )}
                >
                  <p className="truncate text-xs font-medium">
                    {name || '（不明）'}
                  </p>
                  <time className="text-muted-foreground shrink-0 font-mono text-xs tabular-nums">
                    {formatTime(message.sentAt)}
                  </time>
                </div>
                <p
                  className={cn(
                    'rounded-2xl px-3.5 py-2.5 text-sm leading-6 text-pretty break-words shadow-sm',
                    ownMessage
                      ? 'bg-primary text-primary-foreground rounded-br-sm'
                      : 'text-foreground rounded-bl-sm border bg-white',
                    message.pending && 'opacity-60',
                  )}
                >
                  {message.content}
                </p>
              </div>
            </article>
          )
        })}
      </div>

      {readOnly ? (
        <div className="bg-muted text-muted-foreground mt-5 flex items-center gap-2 rounded-lg border px-3 py-3 text-sm">
          <LockKeyhole className="size-4" aria-hidden="true" />
          観戦中は閲覧のみです
        </div>
      ) : (
        <form
          className="chat-composer mt-5 border-t pt-4"
          onSubmit={handleSubmit}
        >
          <label className="sr-only" htmlFor="chat-content">
            メッセージ
          </label>
          <Textarea
            id="chat-content"
            className="min-h-20 resize-none bg-white"
            maxLength={MAX_LENGTH}
            placeholder="メッセージを入力"
            value={content}
            aria-describedby={SHORTCUT_HINT_ID}
            aria-keyshortcuts="Meta+Enter Control+Enter"
            onKeyDown={handleKeyDown}
            onChange={(event) => {
              setContent(event.target.value)
              if (formError) setFormError('')
            }}
          />
          <p
            id={SHORTCUT_HINT_ID}
            className="text-muted-foreground mt-1.5 text-xs"
          >
            <kbd className="font-mono">{SEND_MODIFIER_LABEL} + Enter</kbd>{' '}
            で送信・<kbd className="font-mono">Enter</kbd> で改行
          </p>
          <div className="mt-2 flex items-start gap-3">
            {errorText && (
              <p className="text-destructive text-xs" role="alert">
                {errorText}
              </p>
            )}
            <div className="ml-auto flex items-center gap-2">
              <span className="text-muted-foreground font-mono text-xs tabular-nums">
                {content.length}/{MAX_LENGTH}
              </span>
              <Button
                className="chat-send"
                disabled={!canSend}
                type="submit"
              >
                送信
                <SendHorizontal className="size-4" aria-hidden="true" />
              </Button>
            </div>
          </div>
        </form>
      )}
    </section>
  )
}

function isApplePlatform(): boolean {
  if (typeof navigator === 'undefined') return false
  return /Mac|iPhone|iPad|iPod/.test(navigator.userAgent)
}

function initialsOf(name: string): string {
  const trimmed = name.trim()
  return trimmed ? trimmed.slice(0, 2) : '？'
}

function formatTime(iso: string): string {
  const date = new Date(iso)
  if (Number.isNaN(date.getTime())) return ''
  const hh = String(date.getHours()).padStart(2, '0')
  const mm = String(date.getMinutes()).padStart(2, '0')
  return `${hh}:${mm}`
}

function sendErrorText(code: SendErrorCode): string {
  switch (code) {
    case 'RATE_LIMITED':
      return '送信が早すぎます。少し待ってからもう一度送ってください。'
    case 'INVALID_PHASE':
      return 'いまは発言できません（議論フェーズのみ）。'
    case 'GAME_PAUSED':
      return 'ゲームが一時停止中です。'
    case 'INVALID_PAYLOAD':
      return 'メッセージを送信できませんでした。'
    default:
      return '送信に失敗しました。通信状況を確認してください。'
  }
}
