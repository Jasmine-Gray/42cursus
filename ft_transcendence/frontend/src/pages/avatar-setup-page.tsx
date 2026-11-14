import { useLocation, useSearch } from 'wouter'
import { CharacterSetup } from '@/components/game-ui/character-setup'
import { MobileShell } from '@/components/layout/mobile-shell'
import { avatarOptions, DEFAULT_AVATAR_URL } from '@/data/avatar-options'
import { inviteReturnPath, withInviteReturn } from '@/lib/room-invite'
import { useMe } from '@/hooks/use-me'

export function AvatarSetupPage() {
  const [, navigate] = useLocation()
  const returnTo = inviteReturnPath(useSearch())
  const { user, updateAvatarPreset, uploadAvatarFile } = useMe()

  async function save(src: string, file: File | null) {
    const preset = avatarOptions.find((avatar) => avatar.src === src)
    if (!file && !preset && src === user?.avatarUrl) {
      navigate(returnTo ?? '/home')
      return
    }
    if (!file && !preset) throw new Error('キャラクターを選択してください。')
    const result = file
      ? await uploadAvatarFile(file)
      : await updateAvatarPreset(preset!.id)
    if (!result.success) throw new Error(result.error)
    navigate(returnTo ?? '/home')
  }

  return (
    <MobileShell
      title="キャラクター選択"
      backHref={withInviteReturn('/register', returnTo)}
    >
      <CharacterSetup
        initialSrc={user?.avatarUrl || DEFAULT_AVATAR_URL}
        displayName={user?.displayName || 'プレイヤー'}
        onSave={save}
      />
    </MobileShell>
  )
}
