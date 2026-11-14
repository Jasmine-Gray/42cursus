// プライバシーポリシー・利用規約用の簡易Markdownレンダラー。
//
// 対応するのは「## 見出し」「- 箇条書き」「段落」「[文字](URL)リンク」の4つだけ。
// この程度の構造しか使わないため、外部ライブラリ(react-markdown等)は追加せず自前で解釈する。
import type { ReactNode } from 'react'

type Block = { type: 'paragraph'; text: string } | { type: 'list'; items: string[] }

type Section = { title: string; blocks: Block[] }

function parseSections(markdown: string): Section[] {
  const sections: Section[] = []
  let current: Section | null = null

  const paragraphLines: string[] = []
  const listItems: string[] = []

  const flushParagraph = () => {
    if (paragraphLines.length === 0) return
    current?.blocks.push({ type: 'paragraph', text: paragraphLines.join(' ') })
    paragraphLines.length = 0
  }

  const flushList = () => {
    if (listItems.length === 0) return
    current?.blocks.push({ type: 'list', items: [...listItems] })
    listItems.length = 0
  }

  for (const rawLine of markdown.split('\n')) {
    const line = rawLine.trim()

    if (line.startsWith('## ')) {
      flushParagraph()
      flushList()
      current = { title: line.slice(3).trim(), blocks: [] }
      sections.push(current)
      continue
    }

    if (line.startsWith('- ')) {
      flushParagraph()
      listItems.push(line.slice(2).trim())
      continue
    }

    if (line === '') {
      flushParagraph()
      flushList()
      continue
    }

    // 通常の段落行（空行が来るまで1つの段落として連結する）
    flushList()
    paragraphLines.push(line)
  }

  flushParagraph()
  flushList()

  return sections
}

const LINK_PATTERN = /\[([^\]]+)\]\(([^)]+)\)/g

function renderInline(text: string): ReactNode[] {
  const nodes: ReactNode[] = []
  let lastIndex = 0
  let match: RegExpExecArray | null

  LINK_PATTERN.lastIndex = 0
  while ((match = LINK_PATTERN.exec(text)) !== null) {
    if (match.index > lastIndex) {
      nodes.push(text.slice(lastIndex, match.index))
    }
    nodes.push(
      <a key={match.index} className="font-medium text-primary underline underline-offset-4" href={match[2]}>
        {match[1]}
      </a>,
    )
    lastIndex = match.index + match[0].length
  }
  if (lastIndex < text.length) {
    nodes.push(text.slice(lastIndex))
  }
  return nodes
}

export function renderLegalMarkdown(markdown: string): ReactNode {
  return parseSections(markdown).map((section) => (
    <section key={section.title} className="space-y-2">
      <h2 className="font-heading text-base font-semibold text-balance text-foreground">{section.title}</h2>
      <div className="space-y-2 text-pretty [&_li]:marker:text-primary [&_ul]:list-disc [&_ul]:space-y-1 [&_ul]:pl-5">
        {section.blocks.map((block, index) =>
          block.type === 'paragraph' ? (
            <p key={index}>{renderInline(block.text)}</p>
          ) : (
            <ul key={index}>
              {block.items.map((item, itemIndex) => (
                <li key={itemIndex}>{renderInline(item)}</li>
              ))}
            </ul>
          ),
        )}
      </div>
    </section>
  ))
}
