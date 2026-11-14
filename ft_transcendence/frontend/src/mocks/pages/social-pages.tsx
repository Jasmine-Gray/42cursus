import { useState } from 'react'
import { Check, X } from 'lucide-react'

import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { MobileShell } from '@/components/layout/mobile-shell'
import { Badge } from '@/components/ui/badge'
import { Button } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { Separator } from '@/components/ui/separator'
import { cn } from '@/lib/utils'

const friends = [
  {
    name: 'アオイ',
    initials: 'AO',
    avatarSrc: '/avatars/aoi.webp',
    status: 'オンライン',
    online: true,
  },
  {
    name: 'レン',
    initials: 'RE',
    avatarSrc: '/avatars/ren.webp',
    status: 'ゲーム中',
    online: true,
  },
  {
    name: 'ユナ',
    initials: 'YU',
    avatarSrc: '/avatars/yuna.webp',
    status: '2時間前',
    online: false,
  },
  {
    name: 'ケイ',
    initials: 'KE',
    avatarSrc: '/avatars/kei.webp',
    status: '昨日',
    online: false,
  },
]

export function FriendsPage() {
  const [request, setRequest] = useState<'pending' | 'accepted' | 'declined'>(
    'pending',
  )
  const [query, setQuery] = useState('')
  const [invited, setInvited] = useState<string[]>([])
  const list =
    request === 'accepted'
      ? [
          ...friends,
          {
            name: 'ソラ',
            initials: 'SO',
            avatarSrc: '/avatars/minato.webp',
            status: 'オンライン',
            online: true,
          },
        ]
      : friends
  return (
    <MobileShell title="フレンド" bottomNav>
      <main className="space-y-6 px-5 py-5">
        <section>
          <div className="flex items-center justify-between">
            <h2 className="text-base font-semibold text-balance">
              フレンド申請
            </h2>
            <Badge variant="secondary">
              {request === 'pending' ? '1件' : '0件'}
            </Badge>
          </div>
          {request === 'pending' ? (
            <Card className="mt-3 shadow-none">
              <CardContent className="flex items-center gap-3 p-4">
                <PlayerAvatar
                  className="size-11"
                  src="/avatars/minato.webp"
                  initials="SO"
                  name="ソラ"
                />
                <div className="min-w-0 flex-1">
                  <p className="truncate text-sm font-medium">ソラ</p>
                  <p className="text-muted-foreground text-xs">3分前に申請</p>
                </div>
                <Button
                  onClick={() => setRequest('accepted')}
                  aria-label="ソラさんの申請を承認"
                  size="icon"
                  variant="secondary"
                >
                  <Check className="size-4" aria-hidden="true" />
                </Button>
                <Button
                  onClick={() => setRequest('declined')}
                  aria-label="ソラさんの申請を拒否"
                  size="icon"
                  variant="ghost"
                >
                  <X className="size-4" aria-hidden="true" />
                </Button>
              </CardContent>
            </Card>
          ) : (
            <p className="text-muted-foreground mt-4 text-sm" role="status">
              {request === 'accepted'
                ? 'ソラさんをフレンドに追加しました。'
                : '申請を拒否しました。'}
            </p>
          )}
        </section>

        <section>
          <div className="flex items-center justify-between">
            <h2 className="text-base font-semibold text-balance">
              フレンド一覧
            </h2>
            <span className="text-muted-foreground text-xs">
              {list.length}人
            </span>
          </div>
          <div className="mt-3 overflow-hidden rounded-xl border bg-white">
            {list
              .filter((friend) => friend.name.includes(query.trim()))
              .map((friend, index) => (
                <div key={friend.name}>
                  {index > 0 && <Separator />}
                  <div className="flex items-center gap-3 p-4">
                    <div className="relative">
                      <PlayerAvatar
                        className="size-11"
                        src={friend.avatarSrc}
                        initials={friend.initials}
                        name={friend.name}
                      />
                      <span
                        className={cn(
                          'absolute right-0 bottom-0 size-2.5 rounded-full border-2 border-white',
                          friend.online ? 'bg-emerald-500' : 'bg-stone-300',
                        )}
                        aria-hidden="true"
                      />
                    </div>
                    <div className="min-w-0 flex-1">
                      <p className="truncate text-sm font-medium">
                        {friend.name}
                      </p>
                      <p className="text-muted-foreground text-xs">
                        {friend.status}
                      </p>
                    </div>
                    <Button
                      onClick={() =>
                        setInvited((current) => [...current, friend.name])
                      }
                      disabled={
                        invited.includes(friend.name) ||
                        !friend.online ||
                        friend.status === 'ゲーム中'
                      }
                      size="sm"
                      variant="outline"
                    >
                      {invited.includes(friend.name) ? '招待済み' : '招待'}
                    </Button>
                  </div>
                </div>
              ))}
          </div>
        </section>

        <label className="block text-sm">
          フレンドを表示名で絞り込む
          <input
            className="mt-2 h-11 w-full rounded-lg bg-white px-3"
            value={query}
            onChange={(event) => setQuery(event.target.value)}
            placeholder="表示名を入力"
          />
        </label>
      </main>
    </MobileShell>
  )
}
