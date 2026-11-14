# ワードウルフ MVPバックエンド仕様

> ステータス: Draft 0.2
>
> 最終更新: 2026-08-16
>
> 外部挙動は [game-spec.md](../game/game-spec.md)、通信形式は
> [openapi.yaml](../api/openapi.yaml)（HTTP）と [asyncapi.yaml](../api/asyncapi.yaml)（Channel）を参照する。

## 1. 基本方針

- Phoenixをサーバー権威とする。
- クライアントが送る役職、フェーズ、締切、ユーザーIDは判定に使用しない。
- 1部屋につき1つの `GameServer` プロセスを起動し、進行中状態の正本とする。
- 要求はHTTP（部屋の作成・参加・開始・投票・結果取得）とChannel（準備状態・チャット）で受け、
  配信はすべてChannelで行う。
- GameServerは状態遷移、タイマー、権限、勝敗、公開範囲を決定する。
- 秘密情報を公開Payloadへ含め、フロントエンドで隠す実装を禁止する。

## 2. 構成

```text
React
  -> HTTP（Phoenix Controller） ─┐
  -> Phoenix Socket / Channel ──┼-> GameServer（1 room = 1 process）
                                │      -> Phoenix PubSub
                                │      -> Ecto / PostgreSQL（完了結果のみ）
```

`GameServer` は `DynamicSupervisor` 配下で起動し、`Registry` から `room_id` で参照する。
HTTPで受けた操作要求（開始・投票）もControllerからGameServerへ転送し、結果の配信はPubSub経由でChannelが行う。

## 3. GameServer状態

概念上、次の状態を保持する。実装時の構造体名や型は変更してよいが、意味は維持する。

```text
room_id
phase
phase_version
host_user_id
players[]
spectators[]
active_connection_by_user
game_id
roles_by_user            # CITIZEN / WORD_WOLF
theme_pair               # 多数派用と少数派用のお題
assigned_word_by_user
votes_by_user
winner
result_reason
phase_deadline
paused
paused_remaining_ms
reconnect_deadline
disconnected_player_ids[]
chat_rate_state_by_user
```

### プレイヤー状態

各プレイヤーについて少なくとも次を保持する。

```text
user_id
joined_at
ready
connected
connection_id
role
```

秘密情報はGameServer内では保持してよいが、スナップショット生成時に閲覧者ごとにフィルタする。

## 4. 状態遷移

| 現在 | 遷移条件 | 次 |
|---|---|---|
| `LOBBY` | ホストの開始要求（HTTP）を受理 | `DEALING` |
| `DEALING` | 配役とお題の生成・配信完了 | `DISCUSSION` |
| `DISCUSSION` | 5分経過 | `VOTING` |
| `VOTING` | 全プレイヤーの投票を受理 | `RESULT` |
| `VOTING` | 30秒経過 | `RESULT` |
| `RESULT` | 30秒経過 | `LOBBY` |
| 進行中 | 再接続猶予超過 | `LOBBY` |

遷移はGameServerだけが実行する。クライアントから任意の遷移先を指定させない。
遷移はすべて `phase.changed` で全接続へ配信する。
`RESULT` から `DEALING` への直接遷移（再戦ショートカット）は存在しない。
次のゲームは必ず `LOBBY` の開始フロー（設定確認・準備完了・ホスト開始）を経由する。

## 5. 開始処理

`POST /rooms/{roomId}/start` 受理時に、次を順番に検証する。

1. 現在フェーズが `LOBBY` である。
2. 送信者が現在のホストである。
3. プレイヤー数が3〜8人である。
4. 全プレイヤーが接続中かつ準備完了である。
5. 同じ部屋で進行中ゲームがない。

受理後は、以降の入室者を観戦者として扱い、プレイヤー一覧をゲーム終了まで固定する。

## 6. 配役とお題

- 暗号学的に安全な乱数を使い、ワードウルフ1人を選ぶ。残りを市民とする。
- 部屋設定のジャンルとお題の近さに合うお題ペアを1組選び、どちらを少数派用とするかランダムに決定する。
- ワードウルフには少数派のお題、市民には多数派のお題を割り当てる。
- 本人へ配るのはお題とジャンルだけとし、役職（多数派か少数派か）は結果確定まで本人にも配信しない。
- 配役処理の途中失敗ではゲームを開始せず、状態を `LOBBY` へ戻してエラーを通知する。

## 7. タイマー

- GameServerが絶対時刻の `phase_deadline` を決定する。
- クライアントにはISO 8601 UTCの締切時刻を送る。
- クライアントのカウントダウンは表示専用とし、受理可否はサーバー時刻で判断する。
- タイマーコールバックには `phase_version` を持たせ、古いコールバックによる二重遷移を無視する。
- 締切時刻と要求受信時刻が同じ場合、GameServerが要求を処理した時点で締切以上なら期限切れとする。

## 8. 投票と勝敗

`POST /games/{gameId}/votes` は次をすべて満たす場合だけ受理する。

- フェーズが `VOTING` である。
- 送信者が同じゲームのプレイヤーである（観戦者は不可）。
- 未投票である（変更・再投票は不可）。
- 締切前である。
- `targetUserId` が自分以外の同じゲームのプレイヤーである。

受理のたびに投票済み人数だけを `vote.progress` で配信する。投票先と得票数は `RESULT` まで公開しない。

集計は `RESULT` への遷移時にGameServerが行う。

- ワードウルフが単独で最多得票なら `CITIZEN_SIDE`（`WOLF_IDENTIFIED`）。
- それ以外（同数最多、投票0件を含む）は `WOLF_SIDE`（`WOLF_ESCAPED`）。
- 期限切れ時は受理済みの投票だけで集計する。
- 切断による中止は勝者なしとする。

## 9. チャット

- `DISCUSSION` 中のプレイヤーだけを受理する。
- サーバーで文字列型、trim後1〜300文字、送信頻度を検証する。
- 1秒に1件かつローリング10秒で5件までとする。
- クライアント指定の送信者名、送信時刻は使用しない。
- `message_id` と `sent_at` はサーバーが発行する。
- チャットはDBの `chat_messages` へ保存し、`GET /games/{gameId}/messages` で履歴を返す。
- 進行中の配信はGameServer経由の `chat.published` で行う。

## 10. 切断・再接続

### 切断検知

- 有効なゲーム接続が失われたときだけプレイヤー切断として扱う。
- `DEALING`、`DISCUSSION`、`VOTING` 中なら、最初の切断時にタイマーを停止して残り時間を保存する。
- 最初の切断から20秒後を共通の再接続期限とする。
- 待機中に別のプレイヤーが切断しても期限を延長しない。

### 再開

- 期限内に全切断者が再接続した場合だけ再開する。
- 保存した残り時間から新しい締切とタイマーを設定する。
- 公開スナップショットと本人用のお題を再送する。

### 猶予超過

- ゲームを `ABORTED_DISCONNECT` として終了する。
- 勝敗と戦績を記録しない。
- 秘密情報を結果として公開せず、全接続者を `LOBBY` へ戻す。

## 11. 複数接続

- Channel参加ごとに推測不能な `connection_id` を発行する。
- 同じユーザーの新しい接続を登録した時点で、以前の `connection_id` を無効化する。
- 古いRoom Channelへ `connection.replaced` を直接送信した後、そのChannelを終了して公開・秘密イベントの購読を解除する。
- 操作要求では、Channelが保持する `connection_id` とGameServer上の有効IDを照合する。
- 古い接続からの要求は `STALE_CONNECTION` として拒否する。

## 12. スナップショットと公開範囲

公開スナップショットに含めてよいもの:

- 部屋ID、ゲームID、フェーズ、締切
- プレイヤーの公開プロフィール、準備状態、接続状態
- ホスト、観戦者一覧
- 公開チャット
- 投票済み人数（`VOTING` 中）
- 一時停止状態と再接続期限
- `RESULT` の公開結果（全役職、両方のお題、得票数）

本人用スナップショットにのみ含めるもの:

- 自分に割り当てられたお題とジャンル
- 自分の投票受付結果と投票先

## 13. 永続化境界

MVPでは、進行中状態の正本をGameServerのメモリとする。
テーブル定義はDB設計（`docs/architecture/`、PR #4でレビュー中）に従う。

DBへ保存するもの:

- 正常終了したゲーム（開始・終了日時、終了理由、勝者）
- 各参加者の役職と勝敗
- 受理した投票
- 使用したお題ペアと少数派側の割り当て
- 公開チャット

DBへ保存しないもの:

- 現在フェーズと残り時間
- 準備状態
- 再接続待機状態
- レート制限状態

GameServerプロセス自体が停止した場合、MVPではゲームを復元せず勝敗なしで中止する。将来、スナップショットまたはイベントログによる復旧を追加できる。

## 14. 最低限のテスト

- ホスト以外、人数不足、未準備状態から開始できない。
- 各ユーザーへ自分のお題だけが配信され、役職は配信されない。
- 観戦者へ秘密情報が配信されない。
- フェーズ外のチャットと投票が拒否される。
- 観戦者の投票、自己投票、二重投票が拒否される。
- 締切と同時の要求がサーバー時刻で一意に判定される。
- 全員投票で締切前に `RESULT` へ進む。
- 連続送信がレート制限される。
- 再接続後に同じ状態と残り時間から再開する。
- 再接続猶予超過で勝敗なしとなる。
- 古いタブからの操作が拒否される。
- 同時要求を処理しても結果確定が一度だけ行われる。
