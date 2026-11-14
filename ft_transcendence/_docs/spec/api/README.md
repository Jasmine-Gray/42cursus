# API仕様書

ワードウルフのAPI契約を置くディレクトリ。HTTP（REST）とWebSocket（Phoenix Channels）の2本立て。
**プロジェクト内で最新の情報源はこのAPI契約**とし、他の文書はこれに整合させる。

> ステータス: Draft 0.1
>
> 最終更新: 2026-08-16

## ファイル構成

| ファイル | 役割 |
|---|---|
| [openapi.yaml](openapi.yaml) | **REST API契約の正本**（OpenAPI 3.1.1） |
| [asyncapi.yaml](asyncapi.yaml) | **WebSocket契約の正本**（AsyncAPI 3.1.0、Phoenix Channels） |
| [index.html](index.html) | REST APIのHTMLドキュメント（Scalarで生成） |
| [realtime.html](realtime.html) | WebSocketのHTMLドキュメント（AsyncAPIビューアで生成） |
| [build-docs.sh](build-docs.sh) | HTML 2つの再生成スクリプト |

## 仕様書の見方

HTMLをブラウザで開くだけ（ビューア本体をCDNから読むためネット接続が必要）。

```sh
open _docs/spec/api/index.html      # REST API
open _docs/spec/api/realtime.html   # WebSocket
```

画面（モック25枚）とAPIの対応表は `openapi.yaml` の冒頭（HTMLでは最上部）に記載。

## 編集の流れ

1. `openapi.yaml` / `asyncapi.yaml` を編集する（正本はYAML。HTMLは生成物なので直接編集しない）
2. 検証する

   ```sh
   npx @redocly/cli lint _docs/spec/api/openapi.yaml
   npx @asyncapi/cli validate _docs/spec/api/asyncapi.yaml
   ```

3. HTMLを再生成してYAMLと一緒にコミットする（依存: python3 + PyYAML）

   ```sh
   _docs/spec/api/build-docs.sh
   ```

## HTTPとWebSocketの境界

**要求はHTTP、配信はWebSocket**が基本方針。

| 操作 | 通信 |
|---|---|
| 認証、プロフィール、フレンド | HTTP |
| 部屋の作成・参加・退出・設定変更 | HTTP |
| ゲーム開始（ホスト）、投票、お題・結果の取得 | HTTP |
| 準備状態、チャット送信、状態再送要求 | WS（クライアント→サーバー） |
| フェーズ遷移、チャット、投票進捗、一時停止・再開、結果、接続置換 | WS（サーバー→クライアント配信） |

## 共通規約（両仕様書で共通）

- キー名はJSONの `camelCase`
- 日時はUTCのISO 8601文字列、時間の長さはミリ秒単位の整数
- IDは推測不能な文字列（例: `user_...`, `room_...`, `game_...`）
- クライアントは `userId`・役職・現在フェーズ・締切を送信しない（サーバー権威）
- 成功は `{"status": "ok", "data": ...}`、失敗は `{"status": "error", "error": {"code", "message"}}`
- エラーコードはHTTPとWSで同じ語彙（`openapi.yaml` の `ErrorCode` 参照）
- 秘密情報（役職・自分以外のお題）は本人向けエンドポイントと `user:{userId}` チャンネル以外に含めない

相談中・保留の項目は [\_archive/open-questions.md](../../_archive/open-questions.md) に記録する。

関連文書: [ゲーム仕様](../game/game-spec.md) / [バックエンド仕様](../backend/backend-spec.md) / [DB設計](../../architecture/db-wwolf-updated.md)
