/* ========================================================================
 * 自動生成ファイル — 手で編集しない
 *
 *   生成元: _docs/spec/api/asyncapi.yaml
 *   生成:   make gen  （_tools/gen-contract-types.py）
 *
 * 契約を変えるときは契約PRでYAMLを直し、make gen を実行してコミットする。
 * ======================================================================== */

export interface RoomRemoved {
  roomId: string;
  /** 本人用チャンネルへの通知には部屋名を含める */
  roomName?: string;
  reason: "REMOVED_BY_HOST";
}

/** 一時停止は独立フェーズにせず、進行フェーズに `paused: true` を付与する */
export type Phase = "LOBBY" | "DEALING" | "DISCUSSION" | "VOTING" | "COMEBACK" | "RESULT";

/** 選択可能なお題ジャンル。恋愛・本音と価値観は廃止し、新規作成・設定変更では指定できない。 */
export type RoomGenre = "PLACE" | "FOOD" | "HOBBY" | "DAILY_ITEM" | "WORK_SCHOOL" | "ENTERTAINMENT";

/** お題ジャンル。openapi.yaml の Genre と同一語彙 */
export type Genre = "PLACE" | "FOOD" | "HOBBY" | "DAILY_ITEM" | "WORK_SCHOOL" | "ENTERTAINMENT" | "ROMANCE" | "VALUES";

/** 議論スキップに賛成した人数。全員（skippedCount == playerCount）で VOTING へ移る */
export interface SkipProgress {
  skippedCount: number;
  playerCount: number;
}

export interface VoteProgress {
  votedCount: number;
  playerCount: number;
}

/** ロビーを含む公開状態のスナップショット。閲覧者によらず同一（秘密情報を含めない） */
export interface RoomStateSnapshot {
  /** COMEBACK中の公開入力。それ以外は null */
  comeback: ComebackState | null;
  roomId: string;
  /** 進行中ゲームがない場合 null */
  gameId: string | null;
  phase: Phase;
  phaseVersion: number;
  /** 一時停止中は null にして reconnectDeadline を設定する */
  deadline: string | null;
  paused: boolean;
  reconnectDeadline: string | null;
  hostUserId: string;
  /** 現在の部屋設定。LOBBY中のジャンル・逆転モード・時間の変更を全員へ配信する */
  settings?: {
    genres?: RoomGenre[];
    /** ウルフが市民のお題を当てて逆転できるか */
    comebackEnabled?: boolean;
    discussionMs: number;
    voteMs: number;
  };
  players: ({
    userId: string;
    displayName: string;
    avatarUrl: string;
    /** ホストは常に true */
    ready: boolean;
    connected: boolean;
  })[];
  spectators: ({
    userId: string;
    displayName: string;
    avatarUrl: string;
  })[];
  /** DISCUSSIONフェーズ以外は null。再接続時に賛成状況を復元するために載せる */
  skipProgress: SkipProgress | null;
  /** VOTINGフェーズ以外は null */
  voteProgress: VoteProgress | null;
}

/** openapi.yaml の GameResult と同じ公開内容（`myVote` はHTTP側のみ。個別の投票先は配信しない） */
export interface GameResult {
  /** 逆転回答の確定内容。逆転回答なしの場合は null */
  comeback: ComebackResult | null;
  gameId: string;
  winner: "CITIZEN_SIDE" | "WOLF_SIDE";
  /** WOLF_IDENTIFIED=ウルフが単独最多, WOLF_ESCAPED=ウルフが単独最多でない（同数最多・投票0件を含む）, WOLF_GUESSED=逆転回答に正解 */
  reason: "WOLF_IDENTIFIED" | "WOLF_ESCAPED" | "WOLF_GUESSED";
  wolfUserId: string;
  words: {
    majority: ResultWord;
    minority: ResultWord;
  };
  /** openapi.yaml の ResultPlayer と同じ形（UserPublic + 役職・お題・得票数） */
  players: ({
    userId: string;
    displayName: string;
    avatarUrl: string;
    role: "CITIZEN" | "WORD_WOLF";
    word: string;
    votesReceived: number;
  })[];
}

/** 逆転回答の公開状態。正解のお題は含めず、入力内容だけを全員へ公開する */
export interface ComebackState {
  gameId: string;
  wolfUserId: string;
  content: string;
}

export interface ComebackResult {
  answer: string;
  correct: boolean;
  timedOut: boolean;
}

export interface ResultWord {
  word: string;
  /** このお題を配られた人数 */
  count: number;
}

export interface RoomInvite {
  inviteId: string;
  roomId: string;
  inviteCode: string;
  roomName: string;
  from: {
    userId: string;
    displayName: string;
    avatarUrl: string;
  };
  sentAt: string;
}

/* --- イベントpayload --- */

/** comeback.draft */
export interface ComebackDraft {
  content: string;
}

/** comeback.submit */
export interface ComebackSubmit {
  content: string;
}

/** room.ready（クライアント→サーバー） */
export interface RoomReady {
  ready: boolean;
}

/** chat.send（クライアント→サーバー） */
export interface ChatSend {
  /** 送信元タブ内で一意なUUID。重複排除に使う */
  clientMessageId: string;
  /** trim後1〜300文字。プレーンテキストとして扱う */
  content: string;
}

/** discussion.skip（クライアント→サーバー） */
export interface DiscussionSkip {
  /** 賛成するなら true、取り消すなら false */
  skip: boolean;
}

/** state.request（クライアント→サーバー） */
export type StateRequest = Record<string, never>;

/** phase.changed（サーバー→全員） */
export interface PhaseChanged {
  gameId: string;
  phase: Phase;
  /** 古い配信の破棄に使う単調増加値 */
  phaseVersion: number;
  /** 締切のないフェーズは null */
  deadline: string | null;
}

/** chat.published（サーバー→全員） */
export interface ChatPublished {
  /** サーバーが発行 */
  messageId: string;
  clientMessageId: string;
  userId: string;
  displayName: string;
  content: string;
  /** サーバーが発行 */
  sentAt: string;
}

/** player.connection_changed（サーバー→全員） */
export interface PlayerConnectionChanged {
  userId: string;
  connected: boolean;
}

/** game.paused（サーバー→全員） */
export interface GamePaused {
  reason: "PLAYER_DISCONNECTED";
  disconnectedUserIds: string[];
  reconnectDeadline: string;
}

/** game.resumed（サーバー→全員） */
export interface GameResumed {
  phase: Phase;
  phaseVersion: number;
  deadline: string;
}

/** game.aborted（サーバー→全員） */
export interface GameAborted {
  gameId: string;
  reason: "RECONNECT_TIMEOUT" | "INTERNAL_ERROR";
  nextPhase: "LOBBY";
}

/** private.state（サーバー→本人） */
export interface PrivateState {
  gameId: string;
  word: string;
  category: Genre;
}

/** connection.replaced（サーバー→古い接続の本人） */
export interface ConnectionReplaced {
  reason: "NEWER_CONNECTION";
}

/** friend.presence_changed（サーバー→本人） */
export interface FriendPresenceChanged {
  /** 状態が変化したフレンドのID */
  userId: string;
  state: "ONLINE" | "IN_GAME" | "OFFLINE";
}

/** クライアント→サーバー */
export const CLIENT_EVENTS = [
  "comeback.draft",
  "comeback.submit",
  "room.ready",
  "chat.send",
  "discussion.skip",
  "state.request",
] as const;

/** サーバー→部屋の全接続（room:{roomId}） */
export const SERVER_ROOM_EVENTS = [
  "room.removed",
  "comeback.updated",
  "room.state",
  "phase.changed",
  "chat.published",
  "player.connection_changed",
  "game.paused",
  "game.resumed",
  "skip.progress",
  "vote.progress",
  "result.published",
  "game.aborted",
] as const;

/** サーバー→本人のみ（user:{userId}） */
export const SERVER_USER_EVENTS = [
  "room.removed",
  "private.state",
  "connection.replaced",
  "friend.presence_changed",
  "room.invited",
] as const;

/** 送信イベント名 → payload */
export interface ClientEventPayloads {
  "comeback.draft": ComebackDraft;
  "comeback.submit": ComebackSubmit;
  "room.ready": RoomReady;
  "chat.send": ChatSend;
  "discussion.skip": DiscussionSkip;
  "state.request": StateRequest;
}

/** room チャンネルの受信イベント名 → payload */
export interface RoomEventPayloads {
  "room.removed": RoomRemoved;
  "comeback.updated": ComebackState;
  "room.state": RoomStateSnapshot;
  "phase.changed": PhaseChanged;
  "chat.published": ChatPublished;
  "player.connection_changed": PlayerConnectionChanged;
  "game.paused": GamePaused;
  "game.resumed": GameResumed;
  "skip.progress": SkipProgress;
  "vote.progress": VoteProgress;
  "result.published": GameResult;
  "game.aborted": GameAborted;
}

/** user チャンネルの受信イベント名 → payload */
export interface UserEventPayloads {
  "room.removed": RoomRemoved;
  "private.state": PrivateState;
  "connection.replaced": ConnectionReplaced;
  "friend.presence_changed": FriendPresenceChanged;
  "room.invited": RoomInvite;
}

export type ClientEventName = keyof ClientEventPayloads;
export type RoomEventName = keyof RoomEventPayloads;
export type UserEventName = keyof UserEventPayloads;
