import { UserMinus } from 'lucide-react'
import { useState } from 'react'
import { Button } from '@/components/ui/button'
import {
  AlertDialog,
  AlertDialogTrigger,
  AlertDialogContent,
  AlertDialogHeader,
  AlertDialogTitle,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogCancel,
  AlertDialogAction,
} from '@/components/ui/alert-dialog'
import { api } from '@/lib/api-client'
import type { components } from '@/types/api'

export function RemoveRoomMemberButton({
  roomId,
  member,
  disabled,
  onRemoved,
}: {
  roomId: string
  member: components['schemas']['UserPublic']
  disabled: boolean
  onRemoved: (room: components['schemas']['RoomState']) => void
}) {
  const [open, setOpen] = useState(false)
  const [removing, setRemoving] = useState(false)
  const [error, setError] = useState('')

  async function remove() {
    if (removing || disabled) return
    setRemoving(true)
    setError('')
    try {
      const { data, error: failure } = await api.DELETE(
        '/rooms/{roomId}/members/{userId}',
        {
          params: { path: { roomId, userId: member.userId } },
        },
      )
      if (failure || !data) {
        setError(
          failure?.error.code === 'INVALID_PHASE'
            ? 'ゲームが始まったため、退出させることはできません。'
            : failure?.error.code === 'NOT_HOST'
              ? '退出させる操作は現在のホストだけが行えます。'
              : failure?.error.code === 'NOT_FOUND'
                ? 'この参加者はすでに退出しています。'
                : '退出させられませんでした。もう一度お試しください。',
        )
        return
      }
      setOpen(false)
      onRemoved(data.data)
    } catch {
      setError('通信エラーが発生しました。もう一度お試しください。')
    } finally {
      setRemoving(false)
    }
  }

  return (
    <AlertDialog
      open={open}
      onOpenChange={(next) => {
        if (!removing) {
          setOpen(next)
          setError('')
        }
      }}
    >
      <AlertDialogTrigger
        render={<Button type="button" variant="ghost" size="icon" />}
        disabled={disabled}
        className="text-destructive"
        title="キックする"
        aria-label={`${member.displayName}さんを退出させる`}
      >
        <UserMinus className="size-4" aria-hidden="true" />
      </AlertDialogTrigger>
      <AlertDialogContent>
        <AlertDialogHeader>
          <AlertDialogTitle className="text-balance">
            {member.displayName}さんを退出させますか？
          </AlertDialogTitle>
          <AlertDialogDescription>
            この部屋から退出させます。招待リンクから再び参加することはできます。
          </AlertDialogDescription>
        </AlertDialogHeader>
        {error && (
          <p role="alert" className="text-destructive text-sm">
            {error}
          </p>
        )}
        <AlertDialogFooter>
          <AlertDialogCancel disabled={removing}>キャンセル</AlertDialogCancel>
          <AlertDialogAction
            variant="destructive"
            disabled={removing || disabled}
            onClick={() => void remove()}
          >
            {removing ? '退出させています…' : '退出させる'}
          </AlertDialogAction>
        </AlertDialogFooter>
      </AlertDialogContent>
    </AlertDialog>
  )
}
