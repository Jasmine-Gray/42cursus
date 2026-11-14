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
import type { UserPublic } from '@/hooks/use-friends'

export function RemoveFriendButton({
  friend,
  onRemove,
}: {
  friend: UserPublic
  onRemove: () => Promise<{ success: boolean; error?: string }>
}) {
  const [open, setOpen] = useState(false)
  const [removing, setRemoving] = useState(false)
  const [error, setError] = useState('')

  async function remove() {
    if (removing) return
    setRemoving(true)
    setError('')
    try {
      const result = await onRemove()
      if (!result.success) {
        setError(
          result.error ||
            'フレンドを解除できませんでした。もう一度お試しください。',
        )
        return
      }
      setOpen(false)
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
        className="text-destructive"
        title="フレンドを解除"
        aria-label={`${friend.displayName}さんとのフレンドを解除する`}
      >
        <UserMinus className="size-4" aria-hidden="true" />
      </AlertDialogTrigger>
      <AlertDialogContent>
        <AlertDialogHeader>
          <AlertDialogTitle className="text-balance">
            {friend.displayName}さんとのフレンドを解除しますか？
          </AlertDialogTitle>
          <AlertDialogDescription>
            お互いのフレンド一覧から消えます。相手には通知されません。あとから改めてフレンド申請できます。
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
            disabled={removing}
            onClick={() => void remove()}
          >
            {removing ? '解除しています…' : '解除する'}
          </AlertDialogAction>
        </AlertDialogFooter>
      </AlertDialogContent>
    </AlertDialog>
  )
}
