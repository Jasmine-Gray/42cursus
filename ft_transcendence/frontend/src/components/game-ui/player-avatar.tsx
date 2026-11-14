import { Avatar, AvatarFallback, AvatarImage } from '@/components/ui/avatar'
import { avatarOptions } from '@/data/avatar-options'
import { cn } from '@/lib/utils'

type PlayerAvatarProps = {
  src?: string
  initials: string
  name: string
  className?: string
  imageClassName?: string
  decorative?: boolean
  presentation?: 'icon' | 'portrait'
}

export function PlayerAvatar({ src, initials, name, className, imageClassName, decorative = true, presentation = 'icon' }: PlayerAvatarProps) {
  return (
    <Avatar data-presentation={presentation} data-character-kind={avatarOptions.find(avatar => avatar.src === src)?.category || 'custom'} className={cn('player-avatar', className)}>
      {src && <AvatarImage className={imageClassName} src={src} alt={decorative ? '' : `${name}のアバター`} />}
      <AvatarFallback>{initials}</AvatarFallback>
    </Avatar>
  )
}
