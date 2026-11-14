import { render, screen, within } from '@testing-library/react'
import { describe, expect, it } from 'vitest'
import { players, revealedVotes } from '@/mocks/data/mock-data'
import { BallotStacks } from './ballot-stacks'

describe('BallotStacks', () => {
  it('まだ開いていない票を数えず、暫定最多の全員を光らせる', () => {
    const { container, rerender } = render(
      <BallotStacks players={players} votes={[]} />,
    )
    expect(container.querySelectorAll('[data-leading]')).toHaveLength(0)
    expect(screen.queryAllByRole('listitem')).toHaveLength(0)
    rerender(
      <BallotStacks players={players} votes={revealedVotes.slice(0, 1)} />,
    )
    expect(container.querySelectorAll('[data-leading]')).toHaveLength(1)
    expect(screen.getByLabelText('ミナト、1票、最多票')).toHaveAttribute(
      'data-leading',
    )
    rerender(
      <BallotStacks players={players} votes={revealedVotes.slice(0, 2)} />,
    )
    expect(container.querySelectorAll('[data-leading]')).toHaveLength(2)
    expect(screen.getByLabelText('アオイ、1票、最多票')).toHaveAttribute(
      'data-leading',
    )
    rerender(
      <BallotStacks players={players} votes={revealedVotes.slice(0, 3)} />,
    )
    expect(container.querySelectorAll('[data-leading]')).toHaveLength(1)
    expect(screen.getByLabelText('ミナト、2票、最多票')).toHaveAttribute(
      'data-leading',
    )
    expect(screen.getByLabelText('アオイ、1票')).not.toHaveAttribute(
      'data-leading',
    )
  })

  it('投票した本人のカードを投票先の上に積み、古い票は下に保つ', () => {
    render(<BallotStacks players={players} votes={revealedVotes} />)
    const stack = screen.getByRole('list', { name: 'ミナトへの投票' })
    expect(
      within(stack)
        .getAllByRole('listitem')
        .map((item) => item.getAttribute('data-voter-id')),
    ).toEqual(['kei', 'ren', 'aoi', 'self'])
    expect(screen.getAllByRole('listitem')).toHaveLength(6)
    expect(screen.getByLabelText('ミナト、4票、最多票')).toHaveAttribute(
      'data-leading',
    )
    expect(screen.getByLabelText('田中 ユウ、0票')).not.toHaveAttribute(
      'data-leading',
    )
  })
})

it('公開得票では投票者を捏造せず匿名の票を積む', () => {
  const { container } = render(
    <BallotStacks
      players={players}
      votes={[{ targetId: 'minato' }, { targetId: 'minato' }]}
    />,
  )
  const tickets = within(
    screen.getByRole('list', { name: 'ミナトへの投票' }),
  ).getAllByRole('listitem')
  expect(tickets).toHaveLength(2)
  expect(tickets.every((ticket) => ticket.textContent === '1票')).toBe(true)
  expect(container.querySelector('[data-voter-id]')).toBeNull()
  expect(screen.getByLabelText('ミナト、2票、最多票')).toBeInTheDocument()
})
