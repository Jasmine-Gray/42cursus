# AGENTS.md — AIエージェント向けの作業ルール（正本）

探偵テーマのワードウルフ（42Tokyo チーム課題）。React + Phoenix、4人でレーン分担して開発する。
このファイルは Claude Code / GitHub Copilot / Codex 等すべてのAIツール共通の正本。
人間向けの詳細は `_docs/dev/`（実装フロー・規約・ツール）にあり、矛盾したらそちらが正。

## 絶対ルール（契約ファースト）

1. **契約が唯一の正本**: `_docs/spec/api/openapi.yaml`（REST）と `asyncapi.yaml`（Channel）。
   エンドポイント・イベント・スキーマの追加変更は必ず契約の編集から始める。実装から先に書かない。
2. **型は手書きしない**: `frontend/src/types/` と `backend/lib/word_wolf/contract/` は `make gen` の生成物。
   手編集禁止（CI の `make gen-check` で落ちる）。
3. **生の fetch・手書き interface 禁止**: FEは `openapi-fetch` + 生成型、BEは `@spec` に
   `WordWolf.Contract.Types` を使う。
4. **名前は契約の語彙をそのまま使う**: 略さない・訳さない（`roomState` を `rs` にしない）。

## コマンド

```sh
make             # 本番構成を1コマンド起動（Phoenix prod + 静的 FE）
make setup       # .env.production を生成（秘密鍵と DB パスワードは乱数）
make up          # 本番構成をビルド・起動 → https://localhost:8443
make urls        # 本番構成のアクセス先を表示
make dev         # 開発構成を起動（Vite + 開発用シード）
make dev-urls    # 開発構成のアクセス先とシードのログイン情報を表示
make gen         # 契約から型を生成（契約を変えたら必ず実行してコミット）
make spec-lint   # 契約の検証
make mock        # 契約からモックAPI（:4010）— BE未実装でもFE開発できる
make proxy       # 開発構成の実BEの契約適合検証（:4011 → :4000。make dev が必要）
cd frontend && npm run build   # tsc -b + vite build（FEの検証）
docker compose run --rm -e MIX_ENV=test backend mix test   # BEテスト
```

## 構成と所有（詳細: _docs/dev/conventions.html）

- `frontend/` React 19 + Vite + TS（strict）。ファイルは kebab-case、コンポーネントは named export の function 宣言
- `backend/` Phoenix 1.8。ファイルは snake_case、controller/channel は薄く保ち判断はコンテキストへ
- レーン: A=進行BE(GameServer/Channel) / B=リアルタイムFE / C=永続BE(REST/Ecto) / D=RESTFE+基盤
- ブランチ: `develop` から `<レーン>/<内容>`（例 `d/auth-pages`）を切り develop へPR。main は安定版のみ
- コミット接頭辞: `feat: / fix: / refactor: / docs: / test: / chore:`

## 画面の作り方

UIの正は `_docs/mock/`（25画面の実装済みモック）。画面を作るときは `_docs/mock/src/pages/` から移植し、
データ境界（fixture → prism mock → 実BE）だけを差し替える。詳細: `_docs/dev/data-first.html`。

## エンドポイント追加の手順

契約編集 → `make spec-lint` → `make gen` → `make spec-docs` → 契約PR → 実装PR → `make gen-check`/`make proxy`。
リファレンス実装は `GET /health`（`_docs/dev/index.html` §11）。
