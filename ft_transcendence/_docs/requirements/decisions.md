# 確定事項まとめ

> 最終更新: 2026-08-18
>
> 「もう決まっていること」だけを載せる。議論の余地があるものは載せない。確定事項が出来次第、追記していきます。
> 相談中の項目は [\_archive/open-questions.md](../_archive/open-questions.md) に記録する。
> Subject要件の詳細は [requirements.md](requirements.md)、モジュール構成は [module-plan.md](module-plan.md) を参照。

---

## 1. Subjectにより確定（変更不可）

### 作るものの条件
- フロントエンド + バックエンド + データベースを持つWebアプリ
- コンテナ（Docker等）で **1コマンド起動**
- 最新版Chromeで動作、**ブラウザコンソールに警告・エラーゼロ**
- バックエンドへの接続はすべて **HTTPS**（コンテナ内部同士は平文可）
- 認証情報は `.env`（Git管理外）+ `.env.example` を提供
- **メール + パスワード認証**（ハッシュ化+ソルト）を最低限実装
- 全フォーム・入力は **フロント/バック両方でバリデーション**
- **プライバシーポリシー・利用規約ページ**（中身必須。空・プレースホルダは却下）
- **マルチユーザー同時利用**（同時ログイン、競合処理、リアルタイム反映、レースコンディションなし）
- CSSフレームワーク/スタイリングソリューションを使用

### 進め方の条件
- Git必須: 全員のコミット + 明確なコミットメッセージ + 作業分担が見えること
- **README.mdは英語**、必須セクションあり（AI利用の記載含む）→ 詳細は [requirements.md](requirements.md) §5
- モジュール **Major=2pt / Minor=1pt、合計14pt** で完成。動かないモジュールは0pt
- ボーナスは14pt充足後の超過分、最大5pt
- 評価時: 各モジュールのデモ、全員が自分の貢献を説明、軽微なコード修正を求められる場合あり
- AI生成コードは理解して説明できるものだけ使う（説明できないと失格）

## 2. チームで確定済み

- **テーマ**: 探偵テーマのワードウルフ（Web）。ルールは [game-spec.md](../spec/game/game-spec.md) を正とする
- **ゲームの骨格**（2026-08-16）: 4〜8人、ウルフ1人、`LOBBY → DEALING → DISCUSSION(5分) → VOTING(30秒) → RESULT(結果表示30秒)`。RESULT後は必ず全員LOBBYへ戻り、設定確認・準備完了を経て次のゲームを開始する（再戦ショートカットなし）。切断は20秒の再接続猶予（詳細は [game-spec.md](../spec/game/game-spec.md)）
- **API契約**（2026-08-16）: HTTPは [openapi.yaml](../spec/api/openapi.yaml)、WebSocketは [asyncapi.yaml](../spec/api/asyncapi.yaml) を最新の情報源とする
- **ロール**
  プロダクトオーナー（PO）/dev: hakobori
  プロジェクトマネージャー（PM）/dev:tkusama
  テックリード（TL）/dev:skomatsu
  サービスデザイナー（SD）/dev:mishimod

- **モジュール構成**
（MTG #1, 2026-07-17 / 2026-08-18更新）: ベース14pt + 観戦(Minor 1pt) = **15pt**。
  **AI対戦相手（Major 2pt）は構成から除外**し、AIプレイヤー・NPCは実装しない。
  上積み候補（ORM / ゲーム統計を最優先）は [module-plan.md](module-plan.md) §2 を参照する

- **技術スタック**（MTG #1, 2026-07-17）

| レイヤ | 採用 | 理由 |
|---|---|---|
| フロント | TypeScript / React + Vite + Tailwind CSS | FW Major要件充足。情報量が多く詰まりにくい |
| バック | Elixir / Phoenix（リアルタイムは Phoenix Channels 想定） | 部屋ごとの状態管理・同時接続・再接続に強い。※まず試す方針で採用 |
| DB | PostgreSQL + Ecto (ORM) | スキーマ明確化の要件 + ORM Minor(1pt)。EctoはPhoenix標準（Prisma相当） |
| インフラ | Docker Compose + nginx（TLS終端） | 1コマンド起動 + HTTPS要件を満たす |
| 作図 | draw.io | ワークフロー・設計図の共有用 |
- **タスク管理**: Notion

## 3. テーマ選定により実質確定（ワードウルフを作る以上ほぼ不可避）

- ベースモジュール構成 14pt
- テキストチャットがゲームの中核機能
- WebSocket等によるリアルタイム同期（フェーズ進行・議論・投票進捗）
- 切断対応（20秒の再接続猶予。復帰しなければ勝敗なしで中止。AI・NPCによる代打は行わない）
