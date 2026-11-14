import { BookOpen, Fingerprint, HelpCircle } from 'lucide-react'

export function MysteryHand() {
  return (
    <div
      className="mystery-hand"
      aria-label="お題カードのイメージ。会話に違う言葉がひとつ混ざる"
    >
      <div className="mystery-hand-card mystery-hand-back" aria-hidden="true">
        <Fingerprint />
        
      </div>
      <div className="mystery-hand-card mystery-hand-word" aria-hidden="true">
        <span>お題の例</span>
        <BookOpen />
        <strong>図書館</strong>
        <small>その言葉は、あなただけの秘密。</small>
      </div>
      <div className="mystery-hand-card mystery-hand-secret" aria-hidden="true">
        <span>もうひとつの言葉</span>
        <HelpCircle />
        <strong>？</strong>
        <small>誰が違う言葉を持っている？</small>
      </div>
    </div>
  )
}
