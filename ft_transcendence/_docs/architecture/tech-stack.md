# 技術スタック案: React + Phoenix

> ステータス: 準確定。PoC完了後に正式決定する。

## 1. 選定方針

探偵・犯人モチーフのワードウルフ型ゲームでは、物理演算よりも次の要素が重要になる。

- ゲーム部屋ごとの状態分離
- チャットとフェーズ進行のリアルタイム同期
- ワードや役職など秘密情報の安全な個別配信
- 同時投票の直列化と重複防止
- 切断・再接続時の状態復元

PhoenixのChannel、Presence、プロセス分離をゲームサーバーに利用し、ReactはUIに専念させる。

## 2. 採用候補

### フロントエンド

| 用途 | 技術 | 理由 |
|---|---|---|
| UIフレームワーク | React | コンポーネント分割、情報量、チーム開発のしやすさ |
| ビルド | Vite | SSRを持ち込まず、Phoenixとの責務を分離できる |
| 言語 | TypeScript | UI、RESTレスポンス、Channelイベントの型安全性 |
| ルーティング | React Router | 認証、ロビー、ゲーム、プロフィール画面の遷移 |
| REST状態管理 | TanStack Query | キャッシュ、再取得、Mutation管理 |
| リアルタイム通信 | Phoenix JavaScript Client | Phoenix Socket / Channelとの公式クライアント通信 |
| フォーム | React Hook Form + Zod | フロントエンド側の入力検証 |
| スタイリング | Tailwind CSS | Subjectのスタイリングソリューション要件を満たす |
| テスト | Vitest + React Testing Library | UIとロジックの単体・結合テスト |

グローバル状態管理ライブラリは最初から必須にしない。ゲームの正しい状態はPhoenix側に置き、React側では受信した表示用スナップショットと一時的なUI状態だけを管理する。

### バックエンド

| 用途 | 技術 | 理由 |
|---|---|---|
| Webフレームワーク | Phoenix | HTTP、Channel、PubSub、監視ツリーを統合できる |
| 言語 | Elixir | 軽量プロセスとメッセージ処理がゲーム部屋に適する |
| リアルタイム通信 | Phoenix Channels | 部屋単位の双方向通信とBroadcast |
| オンライン状態 | Phoenix Presence | 接続、切断、複数タブを含むPresence管理 |
| 認証 | `phx.gen.auth`を基礎にしたCookie認証 | メール・パスワード認証を実装し、React向けAPIへ適応する |
| ORM | Ecto | Schema、Migration、Changeset、Transaction |
| テスト | ExUnit | Context、GameServer、Channelのテスト |

### データベース・インフラ

| 用途 | 技術 | 理由 |
|---|---|---|
| RDBMS | PostgreSQL | リレーション、制約、Transactionによる整合性保証 |
| HTTPS / Proxy | Caddy | TLS終端とReact/Phoenixの同一オリジン公開 |
| コンテナ | Docker Compose | Subjectの1コマンド起動要件を満たす |
| E2Eテスト | Playwright | 複数ブラウザコンテキストによる対戦・再接続テスト |

依存バージョンは、プロジェクトのscaffold時点の安定版に固定する。バージョン更新はチームでレビューし、lockfileを必ずコミットする。

## 3. フロントエンドとバックエンドの境界

### REST APIで扱うもの

- ユーザー登録、ログイン、ログアウト
- プロフィール、アバター、フレンド
- ゲーム一覧と参加可能なロビー
- ゲーム履歴、統計、設定
- プライバシーポリシー、利用規約に必要なデータ

### Phoenix Channelsで扱うもの

- ロビーへの参加・退出
- オンライン状態
- チャット
- ゲーム開始とフェーズ遷移
- ワードの個別配信
- 投票と投票締切
- 切断通知と再接続後のスナップショット取得

RESTとChannelの責務を重複させず、ゲーム中の操作は原則としてChannel経由に統一する。

## 4. 認証・セキュリティ方針

- ReactとPhoenixはCaddy経由で同一オリジンとして公開する。
- 認証CookieはHttpOnly、Secure、SameSiteを設定する。
- Socket接続時に署名済みの短時間トークンまたはサーバーSessionを検証する。
- JWTやSession TokenをLocal Storageへ保存しない。
- Socket接続時だけでなく、Channel参加時にも部屋への参加権限を確認する。
- クライアントから届いたユーザーID、役職、現在フェーズを信用しない。
- 全入力をReactとPhoenix/Ecto Changesetの両方で検証する。
- ワードや役職は、権限のないクライアントへ送信した上で非表示にする実装を禁止する。

## 5. API契約

ElixirとTypeScriptの間で型を直接共有できないため、通信契約をリポジトリ内で明文化する。

- REST API: OpenAPIを正とし、可能ならTypeScript型を生成する。
- Channel: Topic、送受信イベント、Payload、Errorを仕様書に記載する。
- ReactではZod、PhoenixではPattern MatchingとChangesetで境界を検証する。
- イベント追加・変更時は、フロントとバックのテストを同じPRで更新する。

## 6. モジュールとの対応

| Subjectモジュール | 対応技術 |
|---|---|
| フロント・バック両方のフレームワーク Major | React + Phoenix |
| WebSocketリアルタイム Major | Phoenix Channels |
| ORM Minor | Ecto |
| 標準ユーザー管理 Major | `phx.gen.auth`、プロフィール、Presence、フレンド |
| Webゲーム / リモート / 3人以上 | GameServer、Channels、再接続 |

## 7. 最終確定条件

次のPoCがすべて通過したら、この構成を正式決定として `docs/requirements/decisions.md` に反映する。

1. ReactからPhoenix Socketへ接続できる。
2. 5つ以上のブラウザセッションが同じ部屋へ参加できる。
3. ユーザーごとに異なるワードを送り、他ユーザーの通信に含まれないことを確認できる。
4. チャットと同時投票を処理し、1人1票を保証できる。
5. 切断後に再接続し、本人用の現在状態を復元できる。
6. 全メンバーが通信経路とゲーム状態遷移を説明できる。

