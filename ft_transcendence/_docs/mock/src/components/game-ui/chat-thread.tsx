import { LockKeyhole, SendHorizontal } from 'lucide-react'
import { useState, type FormEvent } from 'react'

import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { Button } from '@/components/ui/button'
import { Textarea } from '@/components/ui/textarea'
import type { ChatMessage } from '@/data/mock-data'
import { cn } from '@/lib/utils'

type ChatThreadProps = {
  initialMessages: ChatMessage[]
  readOnly?: boolean
}

export function ChatThread({ initialMessages, readOnly }: ChatThreadProps) {
  const [messages, setMessages] = useState(initialMessages)
  const [content, setContent] = useState('')
  const [error, setError] = useState('')

  function handleSubmit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    const trimmed = content.trim()

    if (!trimmed) {
      setError('メッセージを入力してください。')
      return
    }
    if (trimmed.length > 300) {
      setError('メッセージは300文字以内で入力してください。')
      return
    }

    setMessages((current) => [
      ...current,
      {
        id: crypto.randomUUID(),
        userId: 'self',
        name: '田中 ユウ',
        initials: 'YU',
        avatarSrc: '/avatars/yu.webp',
        content: trimmed,
        sentAt: '21:06',
      },
    ])
    setContent('')
    setError('')
  }

  return (
    <section className="discussion-chat" aria-labelledby="chat-title">
      <div>
        <h2 id="chat-title" className="text-balance text-base font-semibold">チャット</h2>
        <p className="text-xs text-muted-foreground">全員に公開されるチャット</p>
      </div>

      <div className="mt-4 space-y-4" role="log" aria-live="polite" aria-relevant="additions" aria-label="チャットメッセージ">
        {messages.map((message) => {
          const ownMessage = message.userId === 'self'

          return (
            <article key={message.id} className={cn('chat-message flex items-end gap-2.5', ownMessage && 'flex-row-reverse')}>
              <PlayerAvatar className="size-9" src={message.avatarSrc} initials={message.initials} name={message.name} />
              <div className="min-w-0 max-w-[78%]">
                <div className={cn('mb-1 flex items-baseline gap-2 px-1', ownMessage && 'justify-end')}>
                  <p className="truncate text-xs font-medium">{message.name}</p>
                  <time className="shrink-0 font-mono text-xs tabular-nums text-muted-foreground">{message.sentAt}</time>
                </div>
                <p
                  className={cn(
                    'break-words text-pretty rounded-2xl px-3.5 py-2.5 text-sm leading-6 shadow-sm',
                    ownMessage
                      ? 'rounded-br-sm bg-primary text-primary-foreground'
                      : 'rounded-bl-sm border bg-white text-foreground',
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
        <div className="mt-5 flex items-center gap-2 rounded-lg border bg-muted px-3 py-3 text-sm text-muted-foreground">
          <LockKeyhole className="size-4" aria-hidden="true" />観戦中は閲覧のみです
        </div>
      ) : (
        <form className="chat-composer mt-5 border-t pt-4" onSubmit={handleSubmit}>
          <label className="sr-only" htmlFor="chat-content">メッセージ</label>
          <Textarea
            id="chat-content"
            className="min-h-20 resize-none bg-white"
            maxLength={300}
            placeholder="メッセージを入力"
            value={content}
            onChange={(event) => {
              setContent(event.target.value)
              if (error) setError('')
            }}
          />
          <div className="mt-2 flex items-start gap-3">
            {error && <p className="text-xs text-destructive" role="alert">{error}</p>}
            <div className="ml-auto flex items-center gap-2">
              <span className="font-mono text-xs tabular-nums text-muted-foreground">{content.length}/300</span>
              <Button disabled={!content.trim()} className="chat-send" type="submit">
                送信<SendHorizontal className="size-4" aria-hidden="true" />
              </Button>
            </div>
          </div>
        </form>
      )}
    </section>
  )
}
