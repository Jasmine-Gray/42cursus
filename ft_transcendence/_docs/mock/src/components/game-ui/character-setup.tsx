import { useEffect, useRef, useState, type ChangeEvent } from 'react'
import { motion, useReducedMotion } from 'motion/react'
import { Upload } from 'lucide-react'
import { Link } from 'wouter'
import { AvatarPicker } from '@/components/game-ui/avatar-picker'
import { PlayerAvatar } from '@/components/game-ui/player-avatar'
import { Button } from '@/components/ui/button'
import { avatarOptions } from '@/data/avatar-options'

type CharacterSetupProps = {
  initialSrc: string
  displayName: string
  onSave: (src: string, file: File | null) => Promise<void>
}

export function CharacterSetup({ initialSrc, displayName, onSave }: CharacterSetupProps) {
  const [selection, setSelected] = useState(initialSrc)
  const [file, setFile] = useState<File | null>(null)
  const [dirty, setDirty] = useState(false)
  const [pending, setPending] = useState(false)
  const [checking, setChecking] = useState(false)
  const [error, setError] = useState('')
  const request = useRef(0)
  const reducedMotion = useReducedMotion()
  const selected = dirty ? selection : initialSrc
  const option = avatarOptions.find(avatar => avatar.src === selected)

  useEffect(() => () => {
    if (selected.startsWith('blob:')) URL.revokeObjectURL(selected)
  }, [selected])
  useEffect(() => () => { request.current += 1 }, [])

  async function chooseFile(event: ChangeEvent<HTMLInputElement>) {
    const candidate = event.target.files?.[0]
    event.target.value = ''
    if (!candidate) return
    setError('')
    if (!['image/png', 'image/jpeg', 'image/webp'].includes(candidate.type) || candidate.size === 0 || candidate.size > 5 * 1024 * 1024) {
      setError('PNG・JPEG・WebP、5MB以下の画像を選択してください。')
      return
    }
    const version = ++request.current
    const src = URL.createObjectURL(candidate)
    setChecking(true)
    try {
      const image = new Image()
      image.src = src
      await image.decode()
      if (image.naturalWidth > 4096 || image.naturalHeight > 4096) throw new Error('画像は縦・横それぞれ4096px以下にしてください。')
      if (version !== request.current) { URL.revokeObjectURL(src); return }
      setSelected(src)
      setFile(candidate)
      setDirty(true)
    } catch (cause) {
      URL.revokeObjectURL(src)
      if (version === request.current) setError(cause instanceof Error && cause.message.includes('4096') ? cause.message : '画像を読み込めませんでした。別の画像を選択してください。')
    } finally {
      if (version === request.current) setChecking(false)
    }
  }

  async function save() {
    setPending(true)
    setError('')
    try { await onSave(selected, file) }
    catch (cause) { setError(cause instanceof Error ? cause.message : 'キャラクターの保存に失敗しました。') }
    finally { setPending(false) }
  }

  return (
    <main className="character-select">
      <section className="character-select-stage" aria-label="選択中のキャラクター">
        <motion.div className="character-standee" key={selected}
          initial={reducedMotion ? false : { opacity: 0, x: -18 }}
          animate={{ opacity: 1, x: 0 }} transition={{ duration: .4, ease: 'easeOut' }}>
          <PlayerAvatar presentation="portrait" src={selected} name={displayName} initials={displayName.slice(0, 2)} decorative={false} />
        </motion.div>
        <div className="character-nameplate" aria-live="polite">
          <span>CHARACTER</span>
          <h1>{option?.label || 'オリジナル'}</h1>
          <p>{option?.description || 'お気に入りの姿で、ゲームへ。'}</p>
        </div>
      </section>
      <section className="character-select-controls">
        <header><p>SELECT YOUR CHARACTER</p><h2>キャラクター選択</h2></header>
        <fieldset disabled={pending || checking} className="character-choices">
          <legend className="sr-only">キャラクターを選ぶ</legend>
          <AvatarPicker value={selected} onChange={src => {
            request.current += 1
            setSelected(src); setFile(null); setDirty(true); setError('')
          }} />
          <label className="character-upload">
            <Upload aria-hidden="true" size={18} /><span>画像を追加</span>
            <input type="file" accept="image/png,image/jpeg,image/webp" aria-label="キャラクター画像を追加" onChange={chooseFile} />
          </label>
          <p className="character-file-hint">透過PNG・WebP対応 / JPEG / 5MBまで</p>
        </fieldset>
        <p className="character-save-feedback" role={error ? 'alert' : 'status'}>{error || (checking ? '画像を確認中…' : '')}</p>
        <footer className="character-select-actions">
          <Button disabled={pending || checking} onClick={save}>{pending ? '保存中…' : '決定'}</Button>
          <Link href="/home" aria-disabled={pending || checking} onClick={event => { if (pending || checking) event.preventDefault() }}>あとで</Link>
        </footer>
      </section>
    </main>
  )
}
