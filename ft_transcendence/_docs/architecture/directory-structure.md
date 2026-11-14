# 予定ディレクトリ構成

> アプリケーション実装開始時の目標構成。現在この文書はディレクトリ設計のみを定義し、空ディレクトリやscaffold済みコードは追加しない。

```text
ft_transcendence/
├── frontend/                    # React + Vite + TypeScript
│   ├── public/
│   ├── src/
│   │   ├── app/                 # Router、Provider、アプリ初期化
│   │   ├── components/          # 複数機能から使うUI
│   │   ├── features/            # 機能単位の実装
│   │   │   ├── auth/
│   │   │   ├── friends/
│   │   │   ├── game/
│   │   │   ├── lobby/
│   │   │   ├── profile/
│   │   │   └── chat/
│   │   ├── lib/                 # REST Client、Phoenix Socket、共通処理
│   │   ├── pages/               # Route単位のページ
│   │   ├── styles/
│   │   └── types/
│   ├── tests/
│   ├── Dockerfile
│   └── package.json
├── backend/                     # Elixir + Phoenix
│   ├── config/
│   ├── lib/
│   │   ├── transcendence/       # ドメインとContext
│   │   │   ├── accounts/
│   │   │   ├── friendships/
│   │   │   ├── games/
│   │   │   │   ├── game_server.ex
│   │   │   │   ├── game_supervisor.ex
│   │   │   │   ├── registry.ex
│   │   │   │   └── rules.ex
│   │   │   └── chat/
│   │   └── transcendence_web/   # HTTP / WebSocket境界
│   │       ├── channels/
│   │       ├── controllers/
│   │       ├── plugs/
│   │       └── router.ex
│   ├── priv/
│   │   └── repo/migrations/
│   ├── test/
│   ├── Dockerfile
│   └── mix.exs
├── contracts/                   # 言語間の通信契約
│   ├── openapi.yaml             # REST API
│   └── realtime-events.md       # Channel Topic / Event / Payload
├── infra/
│   └── caddy/
│       └── Caddyfile
├── docs/
│   ├── agenda/
│   ├── architecture/
│   ├── diagrams/
│   └── requirements/
├── e2e/                         # Playwrightによる複数ユーザーE2E
├── .env.example
├── compose.yaml
├── Makefile
└── README.md
```

## 配置ルール

### frontend

- `features/` は業務機能ごとに分ける。
- 汎用化が確認できるまで、機能固有コンポーネントを `components/` へ移動しない。
- ゲームルールと勝敗判定を配置しない。UI表示用の変換だけを許可する。
- `lib/phoenix/` にSocket生成とChannel購読処理を集約する。

### backend

- `transcendence_web` はHTTPとWebSocketの境界処理に限定する。
- ゲームルールは `transcendence/games` に置き、Channelから直接実装しない。
- GameServerを1ゲームにつき1プロセス起動する。
- DBアクセスはContext経由にし、ChannelやControllerからRepoを直接呼び出さない。

### contracts

- ElixirとTypeScriptの通信仕様の正本とする。
- REST変更時は `openapi.yaml` を同じPRで更新する。
- Channel変更時は `realtime-events.md` を同じPRで更新する。
- フロントとバックで同じイベント名を別々に推測して実装しない。

### infra

- 本番相当の外部通信はCaddy経由でHTTPS/WSSに統一する。
- コンテナ内部のPostgreSQL接続にはDocker Networkを使用する。
- `compose.yaml` からfrontend、backend、database、proxyをまとめて起動する。

## 初期scaffoldの順番

1. `backend/` にPhoenixプロジェクトを生成する。
2. `frontend/` にReact + Vite + TypeScriptプロジェクトを生成する。
3. PostgreSQLとCaddyを含む `compose.yaml` を追加する。
4. ReactからPhoenix REST APIとSocketへ接続する。
5. `contracts/realtime-events.md` を作成してPoCイベントを固定する。
6. 複数ブラウザのPoCが通ってから機能実装を開始する。

