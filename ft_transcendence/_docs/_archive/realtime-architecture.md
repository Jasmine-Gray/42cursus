# リアルタイム構成

> ⚠️ **アーカイブ済み（2026-08-16）**: 旧ゲーム案を含む参考資料。現行の方針は
> [バックエンド仕様](../spec/backend/backend-spec.md) と [asyncapi.yaml](../spec/api/asyncapi.yaml) を参照。

## 1. 基本方針

ゲームはサーバー権威型とする。Reactは操作要求を送り、フェーズ、ワード、投票、勝敗などの正しい状態はPhoenixが決定する。

```text
React
  │ Socket / Channel
  ▼
Phoenix Channel
  │ 認証・Payload検証
  ▼
GameServer（1ゲーム = 1プロセス）
  ├── 状態遷移
  ├── タイマー
  ├── 投票受付
  └── 公開範囲の決定
  │
  ├── Phoenix PubSub / Channel ──> 各Reactクライアント
  └── Ecto ──> PostgreSQL
```

Channelプロセスは接続クライアントごとに存在するため、ゲームの正本をChannelの状態に置かない。

## 2. GameServer

GameServerは `DynamicSupervisor` 配下でゲームごとに起動し、RegistryからゲームIDで参照する。

```text
GameSupervisor
├── GameServer: game-001
├── GameServer: game-002
└── GameServer: game-003
```

想定するフェーズは次のとおり。

```text
waiting
  -> role_assignment
  -> word_confirmation
  -> discussion
  -> voting
  -> result
  -> finished
```

各イベントは現在フェーズ、送信者、参加状態、締切時刻を検証してから受理する。クライアントから送られた現在フェーズや権限情報は使用しない。

## 3. Topic案

| Topic | 対象 | 内容 |
|---|---|---|
| `lobby:<lobby_id>` | ロビー参加者 | 参加・退出、設定、準備状態 |
| `game:<game_id>` | ゲーム参加者 | 公開状態、チャット、フェーズ、投票進捗 |
| `user:<user_id>` | 認証済み本人のみ | ワード、役職、個別通知 |
| `spectate:<game_id>` | 許可された観戦者 | 観戦用に制限した公開状態 |

`user:<user_id>` は、Socketに保存された認証済みユーザーIDとTopicのIDが一致するときだけ参加を許可する。

## 4. 公開情報と秘密情報

### 全参加者へ送信できる情報

- ゲームID、現在フェーズ、締切時刻
- プレイヤーの公開プロフィールと生存・接続状態
- 公開チャット
- 誰が投票済みか（ルール上公開する場合）
- フェーズ終了後に公開される結果

### 本人だけに送信する情報

- 自分のワード
- 自分の役職・陣営
- 本人だけが知る調査結果
- 個別アクションの受付結果

秘密情報を全員向けPayloadに含めてフロント側で非表示にする実装は禁止する。

## 5. 投票と同時実行

投票要求はGameServerが1件ずつ処理し、DBにも次の一意制約を設定する。

```text
UNIQUE(game_id, round_id, voter_id)
```

これにより、連打、再送、複数タブ、再接続後の重複投票を拒否する。投票の永続化とゲーム状態更新が複数DB操作になる場合は、EctoのTransaction内で処理する。

## 6. タイマー

- Phoenixがフェーズの終了時刻を決定する。
- Reactへ残り秒数を毎秒Broadcastせず、`ends_at` を送る。
- Reactは表示用カウントダウンをローカル計算する。
- 実際の締切判定はPhoenixの時刻だけを正とする。

## 7. 再接続

1. ReactがSocket切断を検知する。
2. Presence上では一定時間、切断中として扱う。
3. 再接続時にSocketとChannelの認証を再実行する。
4. GameServerへ本人用スナップショットを要求する。
5. 公開状態と本人だけの秘密情報を分けて再送する。
6. 再接続猶予を超えた場合は、ゲームルールに従って退出またはAI代打へ移行する。

Supervisorによるプロセス再起動だけではゲーム状態は復元されない。重要な状態遷移をイベントログまたはスナップショットとしてPostgreSQLへ保存し、必要に応じてGameServerを再構築できる設計にする。

## 8. 最低限のテスト

- 未認証ユーザーがChannelへ参加できない。
- 他人の `user:<user_id>` Topicへ参加できない。
- 現在フェーズで許可されていない操作を拒否する。
- 同一ユーザーの重複投票を拒否する。
- 同時投票後も票数と結果が一致する。
- 切断・再接続後に本人用状態を復元できる。
- 観戦者へ秘密情報が送られない。
- GameServer停止後の復旧方針どおりに動作する。

