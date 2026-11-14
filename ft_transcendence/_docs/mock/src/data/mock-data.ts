import type { LucideIcon } from 'lucide-react'
import {
  CircleUserRound,
  Eye,
  FileText,
  Gavel,
  House,
  LogIn,
  Link2,
  MessageCircle,
  PauseCircle,
  Search,
  Settings2,
  ShieldQuestion,
  Trophy,
  Timer,
  UserPlus,
  Users,
  UserRound,
} from 'lucide-react'

export type Player = {
  id: string
  name: string
  initials: string
  connected: boolean
  ready: boolean
  isHost?: boolean
  isCurrentUser?: boolean
  avatarSrc: string
}

export type ChatMessage = {
  id: string
  userId: string
  name: string
  initials: string
  content: string
  sentAt: string
  avatarSrc: string
}

export type MockScreen = {
  title: string
  description: string
  href: string
  icon: LucideIcon
}

export const players: Player[] = [
  { id: 'self', name: '田中 ユウ', initials: 'YU', avatarSrc: '/avatars/yu.webp', connected: true, ready: true, isCurrentUser: true, isHost: true },
  { id: 'minato', name: 'ミナト', initials: 'MI', avatarSrc: '/avatars/minato.webp', connected: true, ready: true },
  { id: 'aoi', name: 'アオイ', initials: 'AO', avatarSrc: '/avatars/aoi.webp', connected: true, ready: true },
  { id: 'ren', name: 'レン', initials: 'RE', avatarSrc: '/avatars/ren.webp', connected: true, ready: true },
  { id: 'yuna', name: 'ユナ', initials: 'YU', avatarSrc: '/avatars/yuna.webp', connected: true, ready: true },
  { id: 'kei', name: 'ケイ', initials: 'KE', avatarSrc: '/avatars/kei.webp', connected: true, ready: true },
]

export const spectatorPlayers: Player[] = players.map((player) => ({ ...player, isCurrentUser: false }))

export const chatMessages: ChatMessage[] = [
  {
    id: 'msg_1',
    userId: 'aoi',
    name: 'アオイ',
    initials: 'AO',
    content: '静かに過ごすことが多い場所ですよね。',
    sentAt: '21:04',
    avatarSrc: '/avatars/aoi.webp',
  },
  {
    id: 'msg_2',
    userId: 'minato',
    name: 'ミナト',
    initials: 'MI',
    content: '私は休日にもよく行きます。長居することもあります。',
    sentAt: '21:04',
    avatarSrc: '/avatars/minato.webp',
  },
  {
    id: 'msg_3',
    userId: 'self',
    name: '田中 ユウ',
    initials: 'YU',
    content: 'そこで何かを借りることはありますか？',
    sentAt: '21:05',
    avatarSrc: '/avatars/yu.webp',
  },
  {
    id: 'msg_4',
    userId: 'ren',
    name: 'レン',
    initials: 'RE',
    content: '借りることもありますが、読むだけの時もあります。',
    sentAt: '21:05',
    avatarSrc: '/avatars/ren.webp',
  },
]

export const resultRoles = [
  { playerId: 'self', role: '市民', word: '図書館' },
  { playerId: 'minato', role: '人狼', word: '書店' },
  { playerId: 'aoi', role: '市民', word: '図書館' },
  { playerId: 'ren', role: '市民', word: '図書館' },
  { playerId: 'yuna', role: '市民', word: '図書館' },
  { playerId: 'kei', role: '市民', word: '図書館' },
]

// Result-only fixture: derive totals from the same ballots shown in the UI.
export const revealedVotes = [
  { voterId: 'self', targetId: 'minato' },
  { voterId: 'minato', targetId: 'aoi' },
  { voterId: 'aoi', targetId: 'minato' },
  { voterId: 'ren', targetId: 'minato' },
  { voterId: 'yuna', targetId: 'ren' },
  { voterId: 'kei', targetId: 'minato' },
]

export const voteResults: Record<string, number> = Object.fromEntries(
  players.map(player => [player.id, revealedVotes.filter(vote => vote.targetId === player.id).length]),
)

export const mockScreenGroups: Array<{ label: string; screens: MockScreen[] }> = [
  {
    label: '認証',
    screens: [
      { title: 'ログイン', description: 'メールとパスワードでログイン', href: '/login', icon: LogIn },
      { title: '新規登録', description: '表示名を含むアカウント作成', href: '/register', icon: UserPlus },
      { title: 'アバター作成', description: 'キャラクターを選んでゲームへ', href: '/avatar/setup', icon: UserRound },
    ],
  },
  {
    label: 'ゲーム開始前',
    screens: [
      { title: 'ホーム', description: '進行中の部屋と戦績', href: '/home', icon: House },
      { title: 'URLで部屋を探す', description: '招待URLから参加・観戦', href: '/rooms', icon: Search },
      { title: '招待URLから入室', description: '共有リンクを直接開いた状態', href: '/join/CASE-042', icon: Link2 },
      { title: '部屋を作る', description: '部屋名とカテゴリを設定', href: '/rooms/new', icon: Settings2 },
      { title: 'ロビー（ホスト）', description: '開始イベントを全タブへ配信', href: '/lobby', icon: Users },
      { title: 'ロビー（別プレイヤー）', description: 'ホストの開始を別タブで受信', href: '/lobby/player', icon: UserRound },
      { title: 'ロビー（観戦者）', description: '開始後に観戦画面へ自動遷移', href: '/lobby/spectator', icon: Eye },
    ],
  },
  {
    label: 'ゲーム中',
    screens: [
      { title: 'ゲーム開始', description: '3秒カウントダウン後に自動遷移', href: '/game/countdown', icon: Timer },
      { title: '市民のお題確認', description: '秘密のお題を安全に確認', href: '/game/reveal', icon: ShieldQuestion },
      { title: '人狼のお題確認', description: '陣営を明かさずお題を確認', href: '/game/reveal', icon: ShieldQuestion },
      { title: '市民視点', description: '市民のお題で議論', href: '/game', icon: MessageCircle },
      { title: '人狼視点', description: '人狼のお題で議論', href: '/game', icon: ShieldQuestion },
      { title: '観戦者視点', description: '秘密情報なし・閲覧専用', href: '/game/spectator', icon: Eye },
      { title: '観戦中の投票', description: '投票状況のみを閲覧', href: '/game/spectator/vote', icon: Eye },
      { title: '切断待機', description: '20秒の再接続猶予', href: '/game/paused', icon: PauseCircle },
      { title: '投票', description: '全員が怪しい人物へ投票', href: '/game/vote', icon: Gavel },
      { title: '結果発表', description: '役割、お題、投票結果を公開', href: '/game/result', icon: Trophy },
    ],
  },
  {
    label: 'ユーザーと必須ページ',
    screens: [
      { title: 'フレンド', description: 'オンライン状態とゲーム招待', href: '/friends', icon: Users },
      { title: 'プロフィール', description: 'アバター、戦績、対戦履歴', href: '/profile', icon: CircleUserRound },
      { title: 'プライバシー', description: '課題必須のプライバシーポリシー', href: '/privacy', icon: FileText },
      { title: '利用規約', description: '課題必須の利用規約', href: '/terms', icon: FileText },
    ],
  },
]
