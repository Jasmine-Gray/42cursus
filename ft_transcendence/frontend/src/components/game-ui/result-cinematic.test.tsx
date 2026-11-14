import { act, cleanup, fireEvent, render, screen } from '@testing-library/react'
import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'
import { players, revealedVotes } from '@/mocks/data/mock-data'
import { ResultCinematic } from './result-cinematic'

beforeEach(() => {
  vi.useFakeTimers()
  vi.spyOn(HTMLMediaElement.prototype, 'play').mockResolvedValue()
  vi.spyOn(HTMLMediaElement.prototype, 'pause').mockImplementation(() => {})
})
afterEach(() => {
  cleanup()
  vi.useRealTimers()
  vi.restoreAllMocks()
})

function advance(ms: number) {
  act(() => vi.advanceTimersByTime(ms))
}
function setup(won: boolean) {
  const onComplete = vi.fn()
  render(
    <ResultCinematic
      players={players}
      votes={revealedVotes}
      selectedPlayer={players[1]}
      voteCount={4}
      currentPlayer={players[0]}
      won={won}
      onComplete={onComplete}
    />,
  )
  return onComplete
}

describe('ResultCinematic', () => {
  it.each([true, false])(
    '投票→指名→退場→専用演出→操作待ちの順に進む (won=%s)',
    (won) => {
      const complete = setup(won)
      expect(
        screen.getByRole('heading', { name: 'それぞれの選択' }),
      ).toBeInTheDocument()
      expect(screen.queryAllByRole('listitem')).toHaveLength(0)
      advance(850)
      expect(screen.getAllByRole('listitem')).toHaveLength(1)
      advance(5500)
      expect(screen.getAllByRole('listitem')).toHaveLength(6)
      expect(screen.queryByText('選ばれたのは、')).not.toBeInTheDocument()
      advance(3100)
      expect(screen.getByText('選ばれたのは、')).toBeInTheDocument()
      expect(
        screen.getByRole('heading', { name: players[1].name }),
      ).toBeInTheDocument()
      advance(3200)
      expect(
        screen.getByText(`${players[1].name}が退場します。`),
      ).toBeInTheDocument()
      advance(3400)
      expect(
        screen.queryByLabelText(won ? '勝利の演出' : '敗北の演出'),
      ).not.toBeInTheDocument()
      fireEvent.ended(document.querySelector('video')!)
      expect(
        screen.getByLabelText(won ? '勝利の演出' : '敗北の演出'),
      ).toBeInTheDocument()
      expect(
        screen.queryByRole('heading', { name: /VICTORY|DEFEAT/ }),
      ).not.toBeInTheDocument()
      const backdrop = document.querySelector('.outcome-city')
      const atmosphereFilm = document.querySelector('.outcome-film')
      expect(atmosphereFilm).toHaveAttribute(
        'src',
        `/cinematics/${won ? 'victory' : 'defeat'}.mp4`,
      )
      expect(backdrop).toHaveAttribute(
        'src',
        `/cinematics/${won ? 'victory' : 'defeat'}-still.jpg`,
      )
      advance(4000)
      expect(document.querySelector('.outcome-city')).toBe(backdrop)
      expect(document.querySelector('.outcome-film')).toBe(atmosphereFilm)
      expect(
        screen.getByRole('heading', { name: won ? 'VICTORY' : 'DEFEAT' }),
      ).toBeInTheDocument()
      expect(
        screen.queryByText('人狼', { exact: true }),
      ).not.toBeInTheDocument()
      expect(
        screen.queryByText('市民', { exact: true }),
      ).not.toBeInTheDocument()
      expect(screen.queryByLabelText('人狼')).not.toBeInTheDocument()
      advance(60000)
      expect(complete).not.toHaveBeenCalled()
      fireEvent.click(screen.getByRole('button', { name: '結果を見る' }))
      expect(complete).toHaveBeenCalledTimes(1)
    },
  )

  it('スキップでも結果画面へ自動遷移しない', () => {
    const complete = setup(true)
    fireEvent.click(screen.getByRole('button', { name: '演出をスキップ' }))
    expect(screen.getByRole('heading', { name: 'VICTORY' })).toBeInTheDocument()
    expect(complete).not.toHaveBeenCalled()
    expect(screen.queryByRole('listitem')).not.toBeInTheDocument()
    expect(document.querySelector('.outcome-film')).toBeNull()
  })

  it('勝敗映像が失敗しても静止画で表示し、結果の操作へ進む', () => {
    const complete = setup(false)
    advance(9450)
    advance(3200)
    fireEvent.ended(document.querySelector('video')!)
    fireEvent.error(document.querySelector('.outcome-film')!)
    expect(document.querySelector('.outcome-film')).toBeNull()
    expect(document.querySelector('.outcome-city')).toHaveAttribute(
      'src',
      '/cinematics/defeat-still.jpg',
    )
    advance(4000)
    expect(screen.getByRole('heading', { name: 'DEFEAT' })).toBeInTheDocument()
    expect(complete).not.toHaveBeenCalled()
    fireEvent.click(screen.getByRole('button', { name: '結果を見る' }))
    expect(complete).toHaveBeenCalledTimes(1)
  })

  it('動画の読み込みに失敗しても静止表示を経て進行できる', () => {
    setup(true)
    advance(9450)
    advance(3200)
    fireEvent.error(document.querySelector('video')!)
    expect(screen.getByText('退場', { exact: true })).toBeInTheDocument()
    expect(document.querySelector('video')).toBeNull()
    advance(2200)
    expect(screen.getByLabelText('勝利の演出')).toBeInTheDocument()
  })

  it('再生が止まっても待機上限を超えて画面を固めない', () => {
    setup(false)
    advance(9450)
    advance(3200)
    advance(16000)
    expect(screen.getByLabelText('敗北の演出')).toBeInTheDocument()
  })
})

it('指名なしでは退場を挟まず勝敗演出へ進む', () => {
  render(
    <ResultCinematic
      players={players}
      votes={[]}
      selectedPlayer={null}
      voteCount={0}
      currentPlayer={players[0]}
      won={false}
      onComplete={vi.fn()}
    />,
  )
  advance(2850)
  expect(screen.getByRole('heading', { name: '指名なし' })).toBeInTheDocument()
  advance(3200)
  expect(screen.queryByLabelText('退場の演出')).not.toBeInTheDocument()
  expect(screen.getByLabelText('敗北の演出')).toBeInTheDocument()
})
