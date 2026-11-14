# 5 人で並行して進めるタスク一覧

> 最終更新: 2026-09-02（develop。複数人ログイン・socket 認証・LAN アクセスあり）

前提: `make up` で 3 タブ（`/dev/as/yu` `/dev/as/minato` `/dev/as/aoi`）を開けば同じブラウザで 3 人になれる（自分で登録したプレイヤーは `/dev/as` のフォームから）。
同じ Wi-Fi の別端末からは `.env` に `LAN_HOST=<この PC の IP>` を書いて `https://<IP>:8443`。WebSocket の接続認証（`user_socket.ex`）と FE の `createSocket()` は済みで、タブごとに別ユーザーで繋がる。
各タスクの「確認」はその 3 タブで見る。実装前に見せられるデモ手順は [code-walkthrough.md §8.5](code-walkthrough.md#85-開発で見せる手順実装前でも動く範囲のデモ)。契約は `_docs/spec/api/openapi.yaml` / `asyncapi.yaml`、画面は `_docs/mock/src/pages/`。
進め方は [code-walkthrough.md](code-walkthrough.md) と [conventions.html](conventions.html)。

凡例: **今すぐ** = 依存なしで着手できる / **待ち** = 依存タスクの契約 PR がマージされたら着手

## 1. 土台（先に 1 週間で。5 本を 5 人で並走できる）

| # | タスク | 着手 | 契約 | 主なファイル | 完了条件 |
|---|---|---|---|---|---|
| T1 | 部屋 REST: `POST /rooms` `GET /rooms/{id}` `GET /invites/{code}` `POST /rooms/{id}/join` `leave` | 今すぐ | openapi `rooms` / `invites` | `backend/lib/word_wolf/rooms.ex`, `controllers/room_controller.ex`, `test/.../room_controller_test.exs` | yu が作った部屋に minato が `join` でき、`GET /users/me` の `activeRoom` に出る |
| T2 | Channel の骨: `room_channel.ex`（`join` → `room.state` を返す）。**`user_socket.ex` の接続認証（token → `socket.assigns.user_id`）と `ChannelCase` は済み**。`user_socket.ex` の `channel "room:*"` を有効化するところから | 今すぐ | asyncapi `room.state` | `backend/lib/word_wolf_web/channels/room_channel.ex`, `test/word_wolf_web/channels/room_channel_test.exs` | `ChannelCase` のテストで `connect` → `subscribe_and_join` → `room.state` を受け取れる |
| T3 | FE の Channel 購読: `use-room-channel.ts`（`createSocket()` にチャンネルを載せ、`room.state` を state に）。**`channel-client.ts` の `createSocket()`（接続 + token）は済み** | 今すぐ（T2 とは契約 `types/ws/events.ts` で対面） | asyncapi | `frontend/src/hooks/use-room-channel.ts`, テスト | モックの socket で `room.state` を受けて state が更新される |
| T4 | CI: GitHub Actions で `make gen-check` `make spec-lint` `npm run build && npm test` `mix test` | 今すぐ | — | `.github/workflows/ci.yml` | PR に check が付く。develop への PR で必須化 |
| T5 | 画面 05–07（招待URL検索 / 招待確認 / 部屋を作る）を prism mock（`make mock`）に向けて移植 | 今すぐ（T1 と契約で対面） | openapi `rooms` / `invites` | `frontend/src/pages/room-*.tsx`, `use-rooms.ts`（新規） | mock で 3 画面が動く。T1 が入ったら `VITE_API_BASE` を外して実 BE で確認 |

## 2. 画面群（土台の後。1 人 1 群。依存する BE の REST 部分も同じ人が書く）

| # | 画面群 | 着手 | 依存 | 契約 | 完了条件（3 タブ） |
|---|---|---|---|---|---|
| S1 | 08–10 ロビー（ホスト / プレイヤー / 観戦） | T2, T3 の後 | `room.state`, `room.player_joined/left`, `room.ready` | asyncapi room.* | yu がホスト、minato が入室、aoi が観戦で、3 タブの人数表示が同期する |
| S2 | 11 カウントダウン / 12–13 お題確認 | S1 の後 | `POST /rooms/{id}/start`, GameServer の DEALING、`private.state`（本人だけ） | openapi `start`, asyncapi `private.*` | ホストが開始すると 3 タブが同時にカウントダウンし、各タブに違うお題（1 人だけウルフ）が出る |
| S3 | 14–16 捜査会議（議論 / 観戦） | S2 の後 | `POST /games/{id}/messages`、`chat.message` 配信、タイマー | openapi `games/messages`, asyncapi chat.* | 1 タブの発言が他 2 タブに出る。観戦タブは発言できない |
| S4 | 17, 19 投票 / 20 結果 | S3 の後 | `POST /games/{id}/votes`、`vote.progress`、`game.result`、`GET /games/{id}/result` | openapi votes / result, asyncapi vote.* game.* | 3 タブが投票すると同時に結果画面へ。`GET /users/me` の戦績が増える |
| S5 | 21 フレンド | 今すぐ（独立） | friends REST 全部 | openapi `friends` | yu が minato に申請 → minato のタブで承認 → 両方の `friendCount` が増える |

## 3. BE 機能（画面と切り離して進められる）

| # | タスク | 着手 | 完了条件 |
|---|---|---|---|
| B1 | GameServer の状態遷移（LOBBY → DEALING → DISCUSSION → VOTING → RESULT）を GenServer + テストで。配信は後 | 今すぐ | `game_server_test.exs` で 5 状態の遷移とタイマー満了が通る。`_docs/diagrams/` の図と一致 |
| B2 | お題（word_pairs）の選択ロジック: ジャンル・近さでペアを引き、1 人をウルフに | 今すぐ | DevSeeds の word_pairs で決定的にテストできる |
| B3 | `POST /auth/logout`（現在セッションの失効）+ FE の `logout()` から呼ぶ | 今すぐ | logout 後に同じトークンで `GET /users/me` が 401 |
| B4 | `GET /users/me/games`（対戦履歴）→ プロフィールの fixture を差し替え | 今すぐ | プロフィールの「最近の対戦」が DB の値になる |
| B5 | `DELETE /users/me`（アカウント削除）→ プロフィールの確認ダイアログを接続 | 今すぐ | 削除後にログインできない。関連行（sessions, friendships）が消える |
| B6 | 切断・再接続（`reconnect_grace_ms`、`connection.replaced`、`user_channel.ex`） | S1 の後 | タブを閉じて開き直しても同じ席に戻れる |

## 4. 基盤・品質（E が持つ。他と並走可能）

| # | タスク | 着手 | 完了条件 |
|---|---|---|---|
| E1 | Playwright E2E を `frontend/e2e/` に追加。3 コンテキストで「登録 → 部屋作成 → 2 人 join → ロビー同期」 | T1, S1 の後 | CI で動く。失敗時に screenshot が artifact に残る |
| E2 | `mix format --check-formatted` と `credo` を CI に。生成物 `contract/` は生成側で整形して出す | 今すぐ | `make gen` 直後に format check が通る |
| E3 | `use-me.ts` と `home-page.tsx` の取得ロジック統合、401 の共通処理（middleware の `onResponse` で `/login` へ） | 今すぐ | 全画面で 401 → /login。`home-page.tsx` から `api.GET` 直呼びが消える |
| E4 | `backend/priv/static/avatars/` の複製削除、`frontend/src/data/` を所有マップに追記、conventions.html にレーン E とローテーション表 | 今すぐ | ドキュメントと実体の差が無い |
| E5 | スライスごとの作業報告（`_docs/_archive/YYYY-MM-DD-*.md`）と、触ったファイルのヘッダー更新レビュー | 常時 | 各 PR でヘッダーの Authors / Updated が更新されている |

## 5. 役割の分解（5 本）と 1 週目の成果物

「誰と誰が話す必要があるか」で切る。契約（openapi / asyncapi）を挟んで会話が成立する境界が分割線。

| # | 役割 | 触るもの | 1 週目の成果物 | 契約を挟む相手 |
|---|---|---|---|---|
| ① 部屋（REST + 画面） | `backend/lib/word_wolf/rooms.ex`, `controllers/room_controller.ex`, 画面 05–07 | `POST /rooms` `GET /invites/{code}` `join` と 3 画面。1 人で縦に通す（T1 + T5） | ③（部屋 ID をロビーに渡す） |
| ② リアルタイム BE | `games/game_server.ex`, `channels/room_channel.ex`, `channels/user_channel.ex` | `user_socket.ex` の `channel "room:*"` を有効化し join → `room.state`。GameServer は LOBBY 状態だけ（T2 + B1） | ③④（asyncapi） |
| ③ リアルタイム FE 基盤 + ロビー | socket Provider, `hooks/use-room-channel.ts`, 画面 08–10 | `createSocket()` を 1 本保持する Provider、`room.state` を受けるロビー。3 タブで人数が同期（T3 + S1） | ②（asyncapi）, ①（部屋 ID） |
| ④ ゲーム中画面 + games REST | 画面 11–20, `controllers/game_controller.ex`（messages / votes / result） | 画面 11–13 を移植。②の配信が来る前は `types/ws/events.ts` の型の固定データで動かす（S2–S4 の FE 側） | ②（asyncapi）, ③（Provider を使う側） |
| ⑤ 基盤・統合・QA | `.github/workflows/`, `frontend/e2e/`, 画面 21, 小さな穴（logout / `/users/me/games` / 削除） | CI、Playwright 3 コンテキストで「登録 → 部屋作成 → 2 人 join」。全員の PR レビュー（T4 + E1–E5 + S5） | 全員 |

境界の約束:

1. ①②④が BE を触るとき、他人のコンテキストは呼ぶだけで変えない。変えたいときは契約 PR か、その人への PR
2. socket は③の Provider 経由で 1 本だけ。④が自分で `createSocket()` を呼ばない
3. 契約 PR は 2 人 approve（書いた人 + 受け手）。`make gen` の生成物を含めてマージ

人の当て方（実績ベースの初期案）: ② = Elixir を最も書いている人、④ = 新規参加者（モック移植が定型化している）、⑤ = 統合をしてきた人。
ローテーションするなら、スライス 1（部屋 + ロビー同期、2 週間）が `make up` で見えた時点で ①→②→③→④→⑤ に 1 つずらす。②に入る人は 1 週間前から②の PR レビューに入る。

1 週目の終わりのゴール: `make` → 3 タブで「部屋を作る → 招待URLで 2 人が入る → ロビーに 3 人」が見える。
