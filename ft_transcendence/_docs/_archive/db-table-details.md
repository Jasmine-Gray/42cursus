# データベーステーブル項目詳細解説

> ⚠️ **アーカイブ済み（2026-08-16）**: 初期案。ワードウルフ向けの更新版
> `docs/architecture/db-wwolf-updated.md`（PR #4でレビュー中）に置き換え。

---

## users（ユーザー情報）
| カラム名        | 型      | 説明 |
|----------------|---------|------|
| id             | int, PK | 各ユーザーを一意に識別する番号。自動採番。|
| email          | string  | ユーザーのメールアドレス。ログインIDや通知先。重複不可。|
| password_hash  | string  | 暗号化（ハッシュ化）したパスワード。元のパスワードは保存しない。|
| profile        | text    | 自己紹介やニックネームなどの自由記述欄。|

---

## friends（フレンド関係）
| カラム名   | 型      | 説明 |
|------------|---------|------|
| id         | int, PK | フレンド関係を一意に識別する番号。|
| user_id    | int, FK | フレンド申請をしたユーザーのID。users.idを参照。|
| friend_id  | int, FK | フレンド相手のユーザーID。users.idを参照。|
| status     | string  | 申請中・承認済みなどの状態。|

---

## games（ゲームルーム）
| カラム名    | 型      | 説明 |
|------------|---------|------|
| id         | int, PK | ゲームを一意に識別する番号。|
| status     | string  | ゲームの状態（待機中/進行中/終了など）。|
| started_at | datetime| ゲーム開始時刻。|
| ended_at   | datetime| ゲーム終了時刻。|

---

## game_players（ゲーム参加者）
| カラム名   | 型      | 説明 |
|-----------|---------|------|
| id        | int, PK | 参加記録を一意に識別する番号。|
| game_id   | int, FK | 参加したゲームのID。games.idを参照。|
| user_id   | int, FK | 参加したユーザーのID。users.idを参照。|
| role_id   | int, FK | 割り当てられた役職ID。roles.idを参照。|
| state     | string  | 生存/死亡などの状態。|

---

## game_phases（ゲーム進行フェーズ）
| カラム名    | 型      | 説明 |
|------------|---------|------|
| id         | int, PK | フェーズを一意に識別する番号。|
| game_id    | int, FK | 対象ゲームのID。games.idを参照。|
| phase      | string  | フェーズ名（昼/夜など）。|
| started_at | datetime| フェーズ開始時刻。|
| ended_at   | datetime| フェーズ終了時刻。|

---

## chat_messages（チャットメッセージ）
| カラム名   | 型      | 説明 |
|-----------|---------|------|
| id        | int, PK | メッセージを一意に識別する番号。|
| game_id   | int, FK | どのゲームでの発言か。games.idを参照。|
| user_id   | int, FK | 発言したユーザーのID。users.idを参照。|
| content   | text    | メッセージ内容。|
| sent_at   | datetime| 送信時刻。|

---

## votes（投票履歴）
| カラム名   | 型      | 説明 |
|-----------|---------|------|
| id        | int, PK | 投票を一意に識別する番号。|
| game_id   | int, FK | どのゲームでの投票か。games.idを参照。|
| round     | int     | 投票ラウンド番号。|
| voter_id  | int, FK | 投票したユーザーID。users.idを参照。|
| target_id | int, FK | 投票対象ユーザーID。users.idを参照。|
| voted_at  | datetime| 投票時刻。|

---

## roles（役職マスタ）
| カラム名   | 型      | 説明 |
|-----------|---------|------|
| id        | int, PK | 役職を一意に識別する番号。|
| name      | string  | 役職名（市民/犯人/探偵など）。|
| description| text   | 役職の説明文。|

---

## statistics（戦績）
| カラム名      | 型      | 説明 |
|--------------|---------|------|
| id           | int, PK | 戦績を一意に識別する番号。|
| user_id      | int, FK | 対象ユーザーID。users.idを参照。|
| games_played | int     | プレイ回数。|
| wins         | int     | 勝利数。|
| losses       | int     | 敗北数。|

---

## sessions（セッション管理）
| カラム名   | 型      | 説明 |
|-----------|---------|------|
| id        | int, PK | セッションを一意に識別する番号。|
| user_id   | int, FK | 対象ユーザーID。users.idを参照。|
| token     | string  | セッショントークン（ログイン状態を識別）。|
| expires_at| datetime| 有効期限。|
