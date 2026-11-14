# MTG #1 議事録（キックオフ / 要件定義）

- 日時: 2026-07-17
- アジェンダ: [mtg-01-agenda.md](mtg-01-agenda.md)

---

## 決定事項

> decisions.md にも反映済み → [decisions.md](../requirements/decisions.md)

### モジュール構成（当時17pt / 現在15pt）

- **ベース 14pt + 観戦（Minor 1pt）+ AI対戦（Major 2pt）= 17pt**
- ※2026-08-18: **AI対戦相手（Major 2pt）を構成から除外**し、ベース14pt + 観戦1pt = **15pt** に変更。
  最新の構成は [decisions.md](../requirements/decisions.md) §2 と [module-plan.md](../requirements/module-plan.md) を正とする
- 余裕があれば追加候補: OAuth / 2FA / ダッシュボード など

### 技術スタック

| レイヤ | 採用 | 理由 |
|---|---|---|
| フロント | TypeScript / React + Vite + Tailwind CSS | FW Major要件充足。情報量が多く詰まりにくい |
| バック | Elixir / Phoenix（リアルタイムは Phoenix Channels 想定） | 部屋ごとの状態管理・同時接続・再接続に強い。※まず試す方針で採用 |
| DB | PostgreSQL + Ecto (ORM) | スキーマ明確化の要件 + ORM Minor(1pt)。EctoはPhoenix標準（Prisma相当） |
| インフラ | Docker Compose + nginx（TLS終端） | 1コマンド起動 + HTTPS要件を満たす |
| 作図 | draw.io | ワークフロー・設計図の共有用 |

- バックエンドは当初提案の TypeScript 統一（NestJS + Socket.IO）から変更。メンバーから「Elixir は状況的にあり」との声があり、**試しにやってみる方針**で採用。

### 運用

- タスク管理: **Notion**（カレンダー機能など、使いやすい運用方法は引き続き模索する）

## 持ち越し（次回へ）

- ゲームの骨格（部屋人数 / フェーズ制 / MVP役職 / 切断処理）

## 来週の予定

- draw.io を使ったワークフローの作成
- ゲームデザインのフロー
- おおまかな分担
