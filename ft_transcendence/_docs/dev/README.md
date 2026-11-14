# 開発ガイド

> 最終更新: 2026-09-02

型生成ベースの実装フローと使用ツールをまとめたディレクトリ。**本文はHTMLが正本**（生成物ではないので直接編集する）。

```sh
open _docs/dev/index.html         # 実装フロー
open _docs/dev/data-first.html    # データ前提の画面設計（概念・図中心）
open _docs/dev/tools.html         # 使用ツール
open _docs/dev/conventions.html   # 作業分担と実装規約
```

| ファイル | 内容 |
|---|---|
| [getting-started.md](getting-started.md) | 起動・再構築・片付けと開発コマンド（旧 README の日本語部分）。校舎 Ubuntu での Node.js 導入手順を含む |
| [index.html](index.html) | 型生成ベースの実装フロー。原則・全体像・手で書く境界・セットアップ・日々のサイクル・レーン別の使い方・**§11 実例: /health で一周する**（実用の死活監視とexampleの2役。エンドポイント追加はこの差分を真似る） |
| [data-first.html](data-first.html) | 「データがある前提」で画面を作る概念（図中心）。画面=f(データ)・並走できる理由・3段階の差し替え・層のルール・4状態・画面01〜04への適用 |
| [tools.html](tools.html) | 使用ツールの判断。契約系6 + lint/整形/静的解析/テスト11・導入コマンド・入力と出力の対応・**採用しないツールと理由**・バージョン運用 |
| [conventions.html](conventions.html) | **作業分担と実装規約**。4分割の原則（レーンA〜D）・ディレクトリ所有マップ・ファイル命名・関数定義・ファイル分割・競合を避ける運用（main/developブランチ戦略）・Claude Code skills（`.claude/skills/`） |
| [code-walkthrough.md](code-walkthrough.md) | **コードの流れ（読書会ガイド）**。動いている縦串（登録 / ログイン → ホーム）をブラウザから DB までファイル順に追う。§10 は Elixir / Phoenix の標準形（pipeline / with / action_fallback / changeset）を認証前・認証後のエンドポイントで対比 |
| [parallel-tasks.md](parallel-tasks.md) | **5 人で並行して進めるタスク一覧**。土台 5 本（部屋 REST / Channel の骨 / channel-client / CI / 画面 05–07）、画面群 S1–S5、BE 機能 B1–B6、基盤 E1–E5。各タスクの完了条件は `/dev/as/<name>` の 3 タブで確認する形で書いてある |
| [style.css](style.css) | 上記4ファイル共通のスタイル |

## 型生成

契約から型を生成するコマンドはリポジトリ直下の `Makefile` にある。

```sh
make help          # タスク一覧
make gen           # 契約から型を生成（フロント + バック）
make gen-check     # 生成物が契約と一致しているか確認（CI用）
make spec-lint     # 契約のlint / validate
make mock          # 契約からモックAPIサーバー（:4010）
```

生成スクリプトは `_tools/gen-contract-types.py`（python3 + PyYAML）。
出力は `frontend/src/types/`（TypeScript）と `backend/lib/word_wolf/contract/`（Elixir）。

契約の正本は [../spec/api/](../spec/api/README.md)（`openapi.yaml` / `asyncapi.yaml`）。
このガイドは「契約から型を生成し、その上に実装を載せる」ための手順書であり、契約そのものではない。
