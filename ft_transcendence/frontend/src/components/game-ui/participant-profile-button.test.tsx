import { fireEvent, render, screen, waitFor } from '@testing-library/react'
import { beforeEach, expect, it, vi } from 'vitest'
import type { components } from '@/types/api'
import { ParticipantProfileButton } from './participant-profile-button'

const mocks = vi.hoisted(() => ({ get: vi.fn(), post: vi.fn() }))
vi.mock('@/lib/api-client', () => ({
  api: { GET: mocks.get, POST: mocks.post },
  getAuthToken: () => 'profile-test',
}))
const profile = {
  userId: 'other',
  displayName: '参加者',
  avatarUrl: '',
  bio: '推理が好きです',
}
let friends: components['schemas']['Friend'][]
let incoming: components['schemas']['FriendRequest'][]
let outgoing: components['schemas']['FriendRequest'][]
beforeEach(() => {
  vi.clearAllMocks()
  localStorage.clear()
  friends = []
  incoming = []
  outgoing = []
  mocks.get.mockImplementation(async (path, options) => ({
    data: {
      status: 'ok',
      data:
        path === '/users/{userId}'
          ? profile
          : {
              items:
                path === '/friends'
                  ? friends
                  : options.params.query.direction === 'incoming'
                    ? incoming
                    : outgoing,
            },
    },
  }))
  mocks.post.mockResolvedValue({
    data: { status: 'ok', data: { requestId: 'request_1' } },
  })
})
async function openProfile(currentUserId = 'self') {
  render(
    <ParticipantProfileButton
      userId="other"
      displayName="参加者"
      currentUserId={currentUserId}
    />,
  )
  expect(mocks.get).not.toHaveBeenCalled()
  fireEvent.click(
    screen.getByRole('button', { name: '参加者さんのプロフィール' }),
  )
  expect(await screen.findByText('推理が好きです')).toBeInTheDocument()
}
it('プロフィールを開くだけでは送信せず、申請操作後は重複送信できない', async () => {
  await openProfile()
  expect(screen.getByRole('dialog')).toBeInTheDocument()
  expect(mocks.get).toHaveBeenCalledWith('/users/{userId}', {
    params: { path: { userId: 'other' } },
  })
  expect(mocks.post).not.toHaveBeenCalled()
  fireEvent.click(
    await screen.findByRole('button', { name: 'フレンド申請する' }),
  )
  expect(
    await screen.findByRole('button', { name: 'フレンド申請済み' }),
  ).toBeDisabled()
  expect(mocks.post).toHaveBeenCalledExactlyOnceWith('/friends/requests', {
    body: { targetUserId: 'other' },
  })
})
it('既存フレンドと自分には申請できない', async () => {
  friends = [{ user: profile, presence: { state: 'ONLINE', lastSeenAt: null } }]
  await openProfile()
  expect(
    await screen.findByRole('button', { name: 'フレンド登録済み' }),
  ).toBeDisabled()
})
it('自分のプロフィールには申請操作を出さない', async () => {
  await openProfile('other')
  expect(
    screen.queryByRole('button', { name: /フレンド/ }),
  ).not.toBeInTheDocument()
})
it('受信した申請はその場で承認できる', async () => {
  incoming = [
    { requestId: 'incoming_1', from: profile, sentAt: '2026-09-26T00:00:00Z' },
  ]
  mocks.post.mockResolvedValue({
    data: {
      status: 'ok',
      data: { user: profile, presence: { state: 'ONLINE' } },
    },
  })
  await openProfile()
  fireEvent.click(
    await screen.findByRole('button', { name: 'フレンド申請を承認' }),
  )
  expect(
    await screen.findByRole('button', { name: 'フレンド登録済み' }),
  ).toBeDisabled()
  expect(mocks.post).toHaveBeenCalledWith(
    '/friends/requests/{requestId}/accept',
    { params: { path: { requestId: 'incoming_1' } } },
  )
})
it('通信失敗時はエラーを表示し申請を再試行できる', async () => {
  mocks.post.mockRejectedValueOnce(new Error('offline'))
  await openProfile()
  fireEvent.click(
    await screen.findByRole('button', { name: 'フレンド申請する' }),
  )
  expect(await screen.findByRole('alert')).toHaveTextContent('通信エラー')
  await waitFor(() =>
    expect(
      screen.getByRole('button', { name: 'フレンド申請する' }),
    ).toBeEnabled(),
  )
  fireEvent.click(screen.getByRole('button', { name: 'フレンド申請する' }))
  expect(
    await screen.findByRole('button', { name: 'フレンド申請済み' }),
  ).toBeDisabled()
})
