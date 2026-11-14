export type GameViewpoint = 'citizen' | 'wolf' | 'spectator'

export type GameStartSignal = {
  type: 'game-started'
  id: string
  startsAt: number
}

export const GAME_START_STORAGE_KEY = 'word-wolf:CASE-042:game-start'
const GAME_START_CHANNEL_NAME = 'word-wolf:CASE-042'

function isGameStartSignal(value: unknown): value is GameStartSignal {
  if (!value || typeof value !== 'object') return false
  const candidate = value as Partial<GameStartSignal>
  return candidate.type === 'game-started' && typeof candidate.id === 'string' && typeof candidate.startsAt === 'number'
}

export function publishGameStart(signal: GameStartSignal) {
  window.localStorage.setItem(GAME_START_STORAGE_KEY, JSON.stringify(signal))

  if ('BroadcastChannel' in window) {
    const channel = new BroadcastChannel(GAME_START_CHANNEL_NAME)
    channel.postMessage(signal)
    channel.close()
  }
}

export function subscribeToGameStart(onStart: (signal: GameStartSignal) => void) {
  const channel = 'BroadcastChannel' in window ? new BroadcastChannel(GAME_START_CHANNEL_NAME) : null

  const handleChannelMessage = (event: MessageEvent<unknown>) => {
    if (isGameStartSignal(event.data)) onStart(event.data)
  }

  const handleStorage = (event: StorageEvent) => {
    if (event.key !== GAME_START_STORAGE_KEY || !event.newValue) return
    try {
      const signal: unknown = JSON.parse(event.newValue)
      if (isGameStartSignal(signal)) onStart(signal)
    } catch {
      // Ignore malformed mock events from storage.
    }
  }

  channel?.addEventListener('message', handleChannelMessage)
  window.addEventListener('storage', handleStorage)

  return () => {
    channel?.removeEventListener('message', handleChannelMessage)
    channel?.close()
    window.removeEventListener('storage', handleStorage)
  }
}
