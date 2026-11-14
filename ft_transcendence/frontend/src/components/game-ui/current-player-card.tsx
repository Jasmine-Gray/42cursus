import { motion, useReducedMotion } from 'motion/react'
import { ChevronRight } from 'lucide-react'
import { Link } from 'wouter'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { DEFAULT_AVATAR_URL } from '@/data/avatar-options'

export function CurrentPlayerCard({
  displayName,
  avatarUrl,
}: {
  displayName: string
  avatarUrl?: string | null
}) {
  const reducedMotion = useReducedMotion()
  return (
    <motion.aside
      className="current-player-card"
      initial={reducedMotion ? false : { opacity: 0, y: 18 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ duration: .8, ease: 'easeOut' }}
      aria-label="あなたのプレイヤーカード"
    >
      <div className="current-player-portrait">
        <PlayerAvatar
          presentation="portrait"
          className="current-player-image"
          src={avatarUrl || DEFAULT_AVATAR_URL}
          name={displayName}
          initials={displayName.slice(0, 2)}
          decorative={false}
        />
      </div>
      <div className="current-player-details">
        <h1 className="text-balance">{displayName}</h1>
        <Link href="/profile" className="current-player-profile">
          プロフィール
          <ChevronRight className="size-4" aria-hidden="true" />
        </Link>
      </div>
    </motion.aside>
  )
}
