# ドキュメント索引

> 最終更新: 2026-09-02

探偵テーマのワードウルフ（ft_transcendence）のドキュメント置き場。
**最新の正はAPI契約（[spec/api/](spec/api/)）**とし、他の文書はこれに整合させる。

## 現行ドキュメント

| ディレクトリ | 内容 |
|---|---|
| [requirements/](requirements/) | Subject要件の整理、確定事項、技術スタック比較、モジュール構成案 |
| [spec/api/](spec/api/) | **API契約**。REST（openapi.yaml）とWebSocket（asyncapi.yaml）、HTMLドキュメント付き |
| [spec/game/](spec/game/) | ゲーム外部仕様（フェーズ、役職、勝利条件、切断・再接続ルール） |
| [spec/backend/](spec/backend/) | バックエンド仕様（GameServer、状態遷移、公開範囲、永続化境界） |
| [dev/](dev/) | **実装フロー**（型生成ベース）・**使用ツール**・**作業分担と実装規約**。ブラウザで [dev/index.html](dev/index.html) / [dev/tools.html](dev/tools.html) / [dev/conventions.html](dev/conventions.html) を開く。コードを読む人は [dev/code-walkthrough.md](dev/code-walkthrough.md) から |
| [architecture/](architecture/) | 技術構成（React + Phoenix）、ディレクトリ構成案、DB設計 |
| [diagrams/](diagrams/) | フェーズ遷移・通信シーケンス・例外フロー（Mermaid。READMEを開くとそのまま表示） |
| [agenda/](agenda/) | ミーティングのアジェンダと議事録 |

42 Subject本文（原文PDF）は [_archive/en.subject.pdf](_archive/en.subject.pdf) に置く。
要約は [requirements/requirements.md](requirements/requirements.md) を参照し、疑義がある場合は原文を正とする。

UI画面のモックは [mock/](mock/)（スクリーンショット25枚は [mock/screenshots/](mock/screenshots/)）。

## はじめて読む人向けの順番

1. [requirements/requirements.md](requirements/requirements.md) — 何を作るか
2. [spec/game/game-spec.md](spec/game/game-spec.md) — ゲームのルール
3. [mock/screenshots/](mock/screenshots/) — 画面イメージ
4. [spec/api/README.md](spec/api/README.md) — HTTPとWebSocketの契約
5. [spec/backend/backend-spec.md](spec/backend/backend-spec.md) — サーバー内部の方針
6. [dev/index.html](dev/index.html) — どう作るか（型生成ベースの実装フロー）

## アーカイブ

古くなったドキュメントは [_archive/](_archive/) へ移動する（削除しない）。
移動時は元の場所から参照している箇所のリンクを更新し、アーカイブ側の冒頭に置き換え先を明記する。

## 注意（更新中の項目）

- `spec/game/` と `spec/backend/` はAPI契約に合わせてワードウルフ構成（お題＋全員投票）へ
  更新済み（2026-08-16）。競合する場合は [spec/api/](spec/api/) の契約を優先する。
- DB設計は [architecture/db-wwolf-updated.md](architecture/db-wwolf-updated.md) とEcto migrationを参照する。
