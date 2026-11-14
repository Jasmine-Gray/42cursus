import { Channel, Socket } from 'phoenix'
import { useEffect, useRef } from 'react'

import { useMe } from '@/hooks/use-me'
import { getAuthToken } from '@/lib/api-client'
import { createSocket } from '@/lib/channel-client'
import type { UserEventName, UserEventPayloads } from '@/types/ws/events'

let sharedSocket: Socket | null = null
let sharedChannel: Channel | null = null
let channelUserId: string | null = null
let channelToken: string | null = null
const eventListeners = new Map<string, Set<(payload: any) => void>>()

function getOrCreateUserChannel(userId: string): Channel {
  const currentToken = getAuthToken()
  if (
    sharedChannel &&
    channelUserId === userId &&
    channelToken === currentToken
  ) {
    return sharedChannel
  }

  if (sharedSocket) {
    sharedSocket.disconnect()
    sharedSocket = null
    sharedChannel = null
  }

  const socket = createSocket()
  socket.connect()
  sharedSocket = socket

  const channel = socket.channel(`user:${userId}`, {})
  channel
    .join()
    .receive('ok', () => {
      // Joined successfully
    })
    .receive('error', (resp) => {
      if (import.meta.env.DEV) {
        console.warn(`[UserChannel] Join failed for user:${userId}`, resp)
      }
    })

  // サーバーからのイベントをリッスンして登録されたリスナーへ配信
  const userEvents: UserEventName[] = [
    'room.invited',
    'room.removed',
    'friend.presence_changed',
    'private.state',
    'connection.replaced',
  ]

  userEvents.forEach((eventName) => {
    channel.on(eventName, (payload: unknown) => {
      const listeners = eventListeners.get(eventName)
      if (listeners) {
        listeners.forEach((listener) => listener(payload))
      }
    })
  })

  sharedChannel = channel
  channelUserId = userId
  channelToken = currentToken
  return channel
}

export function useUserChannel() {
  const { user } = useMe()
  const token = getAuthToken()

  // ユーザーID: useMe の user.userId、またはシードトークン dev-token-<name> から即時推論
  const derivedUserId = token?.startsWith('dev-token-')
    ? `user_${token.slice('dev-token-'.length)}`
    : undefined
  const userId = user?.userId ?? derivedUserId

  useEffect(() => {
    if (!userId) return

    getOrCreateUserChannel(userId)
  }, [userId, token])

  const on = useRef(
    <E extends UserEventName>(
      event: E,
      callback: (payload: UserEventPayloads[E]) => void,
    ) => {
      let listeners = eventListeners.get(event)
      if (!listeners) {
        listeners = new Set()
        eventListeners.set(event, listeners)
      }
      listeners.add(callback as (payload: any) => void)

      return () => {
        listeners?.delete(callback as (payload: any) => void)
      }
    },
  ).current

  return {
    on,
  }
}
