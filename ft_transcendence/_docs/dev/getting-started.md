# 起動と開発コマンド（日本語）

> ルートの README.md（英語・課題提出用）から移した、チーム向けの起動・開発手順です。

## クイックスタート

Docker Engine と Docker Compose v2、`make`、`openssl` が必要です。
`make` は `.env.production` にランダムな DB パスワードと秘密鍵を生成し、
Phoenix の `prod` 環境とビルド済みのフロントエンドを起動します。
`.env.production` は Git 管理外です。既存ファイルがあれば値を変更しません。
開発用シードユーザーは投入せず、ゲームのお題だけを同期します。

```sh
make
```

起動後は `https://localhost:8443` にアクセスし、`/register` からユーザーを登録します。
ローカル CA の証明書を使うため、初回はブラウザの警告を許可してください。
開発環境の `make dev` と同時起動する場合は `.env.production` の `APP_PORT` を別の値にします。
同じ Wi-Fi の端末から接続する場合は `.env.production` の `LAN_HOST` をこの PC の IP に変更します。

### 開発モード

校舎の Ubuntu で `make dev` を使う場合は、先に次の Node.js 導入手順を実行してください。

### 校舎の Ubuntu 環境

Ubuntu のターミナル（Bash）で以下を実行してください。
[Node.js 公式の導入案内](https://nodejs.org/en/download)と
[nvm の公式手順](https://github.com/nvm-sh/nvm/tree/v0.40.7#installing-and-updating)に基づきます。
すでに nvm があれば、最初のインストールは省略できます。

```bash
# nvm をインストールして読み込む
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.7/install.sh | bash
source "$HOME/.nvm/nvm.sh"

# Node 24 をインストールして使用する
nvm install 24
nvm use 24
nvm alias default 24

# node が v24.x.x になっていることを確認
node -v
npm -v
```

nvm の保存先を変更している場合は、その場所の `nvm.sh` を読み込んでください。
その後、プロジェクトのルート（`Makefile` があるディレクトリ）で実行します。

```sh
make dev
```

ルートに `.nvmrc` があるので、次回以降はここで `nvm use` と実行しても Node 24 に切り替えられます。
`make dev` はホスト側でも `npm install` を実行します。
Docker 内が Node 24 でも、Ubuntu 側が古い Node のままだと `EBADENGINE` などの警告の原因になります。
`make dev-setup` は npm がある場合にホストの Node 24 を確認し、異なる場合は導入手順を案内して停止します。
npm がない場合は従来どおりホスト側の依存インストールを省略し、Docker 内でインストールします。

これでも停止する場合は、最後の `npm ERR!` または `npm error` を含むログを共有してください。
警告だけの抜粋では停止原因を特定できず、実際のエラーが続きに出ている可能性があります。

### 起動・再構築・片付け

| コマンド | 動作 |
| --- | --- |
| `make` | 本番構成をセットアップしてビルド・起動 |
| `make dev` | 開発構成をセットアップしてビルド・起動 |
| `make clean` | 本番コンテナ・ネットワークとローカルのビルド成果物を削除。DB・証明書ボリューム・依存・環境ファイルは保持 |
| `make fclean` | `clean` に加え、本番 DB・Caddy 証明書ボリューム、ローカルイメージ・依存、書き出した `root.crt` を削除 |
| `make re` | 本番 DB を保持して再ビルド・起動 |

**`make fclean` は登録ユーザーや対戦履歴などのDBデータも削除します。**
`.env.production`・ソース・ロックファイル・契約からの生成型は保持します。
完全に初期化して起動する場合は `make fclean` の完了後に `make` を実行してください。
Caddy の証明書も作り直されるため、信頼済み証明書が必要なら `make ca-cert` で再取得します。
任意の制作ツールの依存は `make` の対象外なので、必要に応じて `cd _tools/media && npm ci` で再導入してください。

### その他のコマンド

```sh
make                         # 本番構成を起動 → https://localhost:8443
make setup                   # 本番用 .env.production の初回作成（再実行可）
make up                      # 本番構成のビルド・起動
make urls                    # 本番構成のアクセス先を表示
make down                    # 本番構成を停止（DB は保持）
make dev                     # 開発構成を起動 → https://localhost:8443
make dev-urls                # 開発用シードのログイン情報を表示
make dev-down                # 開発構成を停止（DB は保持）
make help                    # タスク一覧
make gen                     # API契約から型を生成（FE + BE）
make mock                    # 契約からモックAPIサーバー（:4010、BE未実装でもFE開発できる）
make db-seed                 # 開発 DB に画面確認用データを再投入
make db-shell                # 本番 PostgreSQL へ接続
open _docs/dev/index.html    # 実装フロー（どう作るか）
```

開発用 Seed には`CASE-042`（ロビー）、`CASE-099`（投票中）、プロフィール用の対戦履歴・フレンド・結果データが入る。
全シードユーザーのパスワードは `password123`。通常のログイン画面を利用してください。
開発環境で同じ Wi-Fi の別端末（スマホ等）から入るには `.env` に `LAN_HOST=<この PC の IP>` を書いて `make dev-up` を再実行します。証明書の警告を消すなら本番構成の `make ca-cert` で書き出した root.crt を端末に入れます。

## 開発ルール

- 分担・命名・分割の規約: [_docs/dev/conventions.html](conventions.html)
- ブランチ: `develop` から `<レーン>/<内容>` を切り、`develop` へPR。`main` は安定版のみ
- エンドポイントの追加・変更は契約から（`.claude/skills/add-endpoint` に手順を定義済み）
