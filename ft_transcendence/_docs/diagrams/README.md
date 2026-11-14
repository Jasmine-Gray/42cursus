# 図解

> 最終更新: 2026-09-02

現行仕様（[game-spec.md](../spec/game/game-spec.md) / [spec/api/](../spec/api/README.md)）のフロー図。
Mermaidで管理しているため、GitHub上でこのページを開けばそのまま表示され、テキスト差分でレビューできる。

## フェーズ遷移

```mermaid
stateDiagram-v2
    [*] --> LOBBY
    LOBBY --> DEALING : ホストが開始（4〜8人・全員準備完了）
    DEALING --> DISCUSSION : 配役とお題の配布完了
    DISCUSSION --> VOTING : 5分経過
    VOTING --> RESULT : 全員投票 または 30秒経過
    RESULT --> LOBBY : 30秒経過（結果確認後は必ずロビーへ）
    DEALING --> LOBBY : 再接続猶予（20秒）超過
    DISCUSSION --> LOBBY : 再接続猶予（20秒）超過
    VOTING --> LOBBY : 再接続猶予（20秒）超過
```

- 一時停止は独立フェーズにせず、進行フェーズに `paused: true` を付与する。
- 開始直後の3秒カウントダウンはクライアント演出であり、フェーズではない。
- `RESULT → DEALING` の直接遷移（再戦ショートカット）は存在しない。次のゲームは必ずロビーの
  開始フロー（設定確認・準備完了・ホスト開始）を経由する。

## 1ゲームの通信シーケンス（HTTPとChannelの分担）

```mermaid
sequenceDiagram
    participant C as クライアント
    participant H as HTTP API
    participant W as Channel（room / user）

    C->>H: POST /rooms/{roomId}/join
    C->>W: Join room:{roomId} と user:{userId}
    C->>W: room.ready（準備完了）
    C->>H: POST /rooms/{roomId}/start（ホストのみ）
    W-->>C: phase.changed（DEALING → DISCUSSION）
    W-->>C: private.state（自分のお題。user Topicのみ）
    Note over C,H: 再取得は GET /games/{gameId}/assignment
    C->>W: chat.send
    W-->>C: chat.published（全員へ）
    W-->>C: phase.changed（VOTING）
    C->>H: POST /games/{gameId}/votes
    W-->>C: vote.progress（投票済み人数のみ）
    W-->>C: phase.changed（RESULT）と result.published
    C->>H: GET /games/{gameId}/result
    Note over C,W: 30秒後に phase.changed（LOBBY）で全員ロビーへ戻る
```

要求はHTTP、配信はChannelが基本方針。イベントの完全な定義は
[openapi.yaml](../spec/api/openapi.yaml) と [asyncapi.yaml](../spec/api/asyncapi.yaml) を正とする。

## 切断・再接続の例外フロー

```mermaid
flowchart TD
    A[進行中にプレイヤーが切断] --> B[フェーズタイマーを停止し残り時間を保存]
    B --> C[game.paused を配信<br/>再接続期限 = 最初の切断から20秒<br/>追加切断でも期限は延長しない]
    C --> D{期限内に全切断者が再接続した?}
    D -- はい --> E[残り時間から締切を再設定<br/>game.resumed と private.state を再送]
    D -- いいえ --> F[ABORTED_DISCONNECT として中止<br/>勝敗・戦績は記録しない]
    F --> G[game.aborted を配信し全員 LOBBY へ]
    E --> H[同じ役職・お題・フェーズから再開]
```

## 技術構成図（SVG）

React + Phoenix の全体構成。詳細は [architecture/](../architecture/README.md) を参照。

![ReactとPhoenixの構成図](../architecture/react-phoenix-architecture.svg)

## 複数タブの接続置換

```mermaid
sequenceDiagram
    participant T1 as 古いタブ
    participant S as サーバー
    participant T2 as 新しいタブ

    T1->>S: Join（connectionId: A）
    T2->>S: Join（connectionId: B）
    S-->>T1: connection.replaced を直接送信しChannelを終了
    Note over T1: 以降の操作は STALE_CONNECTION で拒否
    S-->>T2: 公開状態と private.state を配信（有効な接続はBのみ）
```

## 認証と WebSocket のしくみ

REST と WebSocket が同じトークンで本人を決める経路、別タブが別プレイヤーになる理由、Socket / Channel / GameServer の層の重なりを
[auth-socket.md](auth-socket.md) にまとめた（Mermaid 4 枚 + 公式ドキュメントへのリンク）。
