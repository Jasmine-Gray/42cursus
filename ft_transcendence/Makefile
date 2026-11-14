# ==============================================================================
#  Makefile
# ------------------------------------------------------------------------------
#  Created : 2026-08-19  yayoi111                                   開発タスク（make gen 中心）を新規作成
#  Authors : 2026-09-02  yayoi111      (d/make-up-urls)             make up の最後にアクセス先とシードのログイン情報を表示する
#  Updated : 2026-09-02  yayoi111      (d/dev-multi-login)          urls に複数人デバッグ（/dev/as/<name>）の案内を追加
#            2026-09-02  yayoi111      (d/dev-tab-login-form)       登録済みユーザーのタブ限定ログインの案内を追加
#            2026-09-02  yayoi111      (d/socket-auth-lan)          urls に LAN の案内、ca-cert ターゲットを追加
#            2026-09-02  yayoi111      (develop)                    引数なしの make を start（setup → up）に。setup をツール欠如に耐える形に
#            2026-09-05  yayoi111      (d/creative-tooling)         Ubuntu の Node 確認、起動順序、clean / fclean / re を追加
#            2026-09-12  yayoi111      (d/character-ui-cleanup)     共通CSS同期・検証
#            2026-09-16  otnk666       (c/friends-invite)           make up で backend の起動待機（--wait）を追加
#            2026-09-28  yayoi111      (d/production-startup)       既定の起動を本番構成にし、開発構成を make dev に分離
# ------------------------------------------------------------------------------
#  役割    : 本番・開発起動と契約タスクの入口（make help で一覧）
#  契約    : _docs/spec/api/openapi.yaml, _docs/spec/api/asyncapi.yaml を正本として型を生成する
#  関連    : _tools/gen-contract-types.py, compose.yaml, _docs/dev/index.html（実装フロー）, _docs/dev/tools.html（ツール版の固定方針）
# ==============================================================================

# ワードウルフ — 起動・開発タスク
#
# 契約（_docs/spec/api/*.yaml）を正本とし、型はすべてここから生成する。
# 手順とツール選定は _docs/dev/index.html / _docs/dev/tools.html を参照。

OPENAPI      := _docs/spec/api/openapi.yaml
ASYNCAPI     := _docs/spec/api/asyncapi.yaml
FE_TYPES     := frontend/src/types
BE_CONTRACT  := backend/lib/word_wolf/contract

# ツール版は2026-08-19時点の最新に固定する。期間中は上げない（_docs/dev/tools.html §07）。
# openapi-typescript は peer が TS ^5.x のため frontend の依存に入れず npx 固定で呼ぶ
# （frontend は mock 由来の TS ~6.0.2。npx は独立ツリーなので衝突しない）。
OPENAPI_TS_V := 7.13.0
REDOCLY_V    := 2.46.1
ASYNCAPI_V   := 6.0.2
PRISM_V      := 5.16.0

NPX := npx --yes
PROD_ENV := .env.production
PROD_COMPOSE := docker compose --env-file $(PROD_ENV) -f compose.prod.yaml
DEV_COMPOSE := docker compose -f compose.yaml

# 引数なしの make は本番構成を一回で起動する。開発構成は make dev。
.DEFAULT_GOAL := start
.PHONY: start help gen gen-frontend gen-backend gen-check spec-lint spec-docs mock proxy
.PHONY: setup up urls ca-cert down clean fclean re logs db-shell
.PHONY: dev dev-setup dev-up dev-urls dev-down dev-logs db-seed dev-db-shell
.PHONY: ui-styles ui-styles-check

ui-styles: ## モックの共通CSSをフロントエンドへ同期する
	python3 _tools/sync-ui-styles.py

ui-styles-check: ## 共通CSSがモックの正本と一致するか検証する
	python3 _tools/sync-ui-styles.py --check

help: ## このヘルプを表示する
	@grep -E '^[a-zA-Z_-]+:.*?## ' $(MAKEFILE_LIST) \
	  | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-14s\033[0m %s\n", $$1, $$2}'

gen: gen-frontend gen-backend ## 契約から型を生成する（フロント + バック）

gen-frontend: ## REST型とChannel payload型をTypeScriptで生成する
	@mkdir -p $(FE_TYPES)
	$(NPX) openapi-typescript@$(OPENAPI_TS_V) $(OPENAPI) -o $(FE_TYPES)/api.d.ts
	python3 _tools/gen-contract-types.py --only frontend

gen-backend: ## REST型とChannelイベント型をElixirで生成する
	python3 _tools/gen-contract-types.py --only backend

gen-check: gen ## 生成物が最新か確認する（生成物の手編集と未コミットをCIで落とす）
	@if [ -n "$$(git status --porcelain -- $(FE_TYPES) $(BE_CONTRACT))" ]; then \
	  git status --short -- $(FE_TYPES) $(BE_CONTRACT); \
	  echo ""; \
	  echo "生成物が契約と一致していません（または未コミットです）。"; \
	  echo "make gen の結果をコミットしてください。"; \
	  exit 1; \
	fi
	@echo "生成物は契約と一致しています。"

spec-lint: ## 契約をlint / validateする
	$(NPX) @redocly/cli@$(REDOCLY_V) lint $(OPENAPI)
	@if command -v docker >/dev/null 2>&1; then \
	  docker run --rm -v "$$(pwd):/spec" asyncapi/cli:latest validate /spec/$(ASYNCAPI); \
	else \
	  $(NPX) @asyncapi/cli validate $(ASYNCAPI); \
	fi

spec-docs: ## 契約のHTMLドキュメントを再生成する
	_docs/spec/api/build-docs.sh

mock: ## 契約からモックAPIサーバーを起動する（FE先行開発用・:4010）
	$(NPX) @stoplight/prism-cli@$(PRISM_V) mock $(OPENAPI) -p 4010

proxy: ## 実BEのレスポンスが契約どおりか検証する（:4011 -> :4000）
	$(NPX) @stoplight/prism-cli@$(PRISM_V) proxy $(OPENAPI) http://localhost:4000 \
	  -p 4011 --errors

start: up ## （既定）本番構成をセットアップして起動する

setup: ## 本番用 .env.production を安全な乱数で作成する（既存ファイルは保持）
	@if [ ! -f $(PROD_ENV) ]; then \
	  command -v openssl >/dev/null 2>&1 || { echo "openssl が必要です"; exit 1; }; \
	  umask 077; \
	  { \
	    printf 'POSTGRES_USER=word_wolf\n'; \
	    printf 'POSTGRES_PASSWORD=%s\n' "$$(openssl rand -hex 24)"; \
	    printf 'POSTGRES_DB=word_wolf_prod\n'; \
	    printf 'SECRET_KEY_BASE=%s\n' "$$(openssl rand -hex 64)"; \
	    printf 'PHX_HOST=localhost\n'; \
	    printf 'APP_PORT=8443\n'; \
	    printf 'LAN_HOST=127.0.0.1\n'; \
	  } > $(PROD_ENV); \
	  echo "$(PROD_ENV) を作成しました（git 管理外）"; \
	fi
	@chmod 600 $(PROD_ENV)

up: setup ## 本番構成（Phoenix prod + 静的 FE）を起動する
	$(PROD_COMPOSE) up -d --wait --build
	@$(MAKE) --no-print-directory urls

# .env の値は sh で source せず grep で拾う（秘密値に記号が入っても安全）
prod_env_value = $$(grep -E '^$(1)=' $(PROD_ENV) 2>/dev/null | tail -1 | cut -d= -f2-)
dev_env_value = $$(grep -E '^$(1)=' .env 2>/dev/null | tail -1 | cut -d= -f2-)

urls: ## 本番構成のアクセス先を表示する
	@APP_PORT=$${APP_PORT:-$(call prod_env_value,APP_PORT)}; LAN_HOST=$${LAN_HOST:-$(call prod_env_value,LAN_HOST)}; \
	echo ""; \
	echo "  本番モード: https://localhost:$${APP_PORT:-8443}"; \
	if [ -n "$$LAN_HOST" ] && [ "$$LAN_HOST" != "127.0.0.1" ]; then \
	  echo "  LAN:       https://$$LAN_HOST:$${APP_PORT:-8443}"; \
	fi; \
	echo "  初回はローカル CA の証明書警告を許可してください。"; \
	echo "  ユーザーは /register から登録できます（開発用シードは投入しません）。"; \
	echo ""

dev: dev-up ## 開発構成をセットアップして起動する

dev-setup: ## 開発用 .env・FE依存・生成型を準備する
	@test -f .env || (cp .env.example .env && echo ".env を .env.example から作成しました（POSTGRES_PASSWORD 等は必要なら編集）")
	@if command -v npm >/dev/null 2>&1; then \
	  if ! node -e 'process.exit(Number(process.versions.node.split(".")[0]) === 24 ? 0 : 1)' >/dev/null 2>&1; then \
	    echo "ホスト側の Node.js 24 が必要です。README.md の「校舎の Ubuntu 環境」を実行してから make を再実行してください。"; \
	    exit 1; \
	  fi; \
	  (cd frontend && npm install --no-fund --no-audit); \
	else \
	  echo "npm が無いので frontend の依存インストールを省略（コンテナ内で npm ci するので起動には不要。lint/test を手元で回すなら Node を入れる）"; \
	fi
	@if command -v npx >/dev/null 2>&1 && python3 -c "import yaml" >/dev/null 2>&1; then \
	  $(MAKE) --no-print-directory gen; \
	else \
	  echo "npx か python3+PyYAML が無いので型生成を省略（生成物はコミット済みなので起動には不要。契約を変えるときは make gen が要る）"; \
	fi

dev-up: dev-setup ## 開発構成（Vite + 開発用シード）を起動する
	$(DEV_COMPOSE) up -d --wait --build
	@$(MAKE) --no-print-directory dev-urls

dev-urls: ## 開発構成のアクセス先とログイン情報を表示する
	@DB_PORT=$(call dev_env_value,POSTGRES_HOST_PORT); DB_NAME=$(call dev_env_value,POSTGRES_DB); DB_USER=$(call dev_env_value,POSTGRES_USER); \
	echo ""; \
	echo "  アクセス先（make up 後。backend の起動完了は docker compose ps の health で確認）"; \
	echo "  ------------------------------------------------------------------------"; \
	echo "  アプリ（Caddy 経由・本結合）   https://localhost:8443          ← ブラウザはこれ。自己署名証明書の警告は許可する"; \
	echo "  backend 直（契約適合・直叩き） http://localhost:4000/health"; \
	echo "  pgweb（DB ブラウザ）           http://localhost:8081"; \
	echo "  PostgreSQL                     localhost:$${DB_PORT:-5432}  db=$${DB_NAME:-word_wolf_dev}  user=$${DB_USER:-word_wolf}"; \
	echo ""; \
	echo "  開発ログイン（画面01）: 全シードユーザー共通パスワード password123（WordWolf.DevSeeds.dev_password/0）"; \
	echo "    例: player@example.com = 田中 ユウ（CASE-042 参加中・3戦2勝）"; \
	echo "  Bearer 直叩き: Authorization: Bearer dev-token-yu（田中 ユウ）"; \
	echo "  新規登録もできる: https://localhost:8443/register"; \
	echo ""; \
	echo ""; \
	LAN_HOST=$(call dev_env_value,LAN_HOST); LAN_IP=$$(ipconfig getifaddr en0 2>/dev/null || ipconfig getifaddr en1 2>/dev/null || hostname -I 2>/dev/null | cut -d' ' -f1); \
	if [ -n "$$LAN_HOST" ]; then \
	  echo "  同じ Wi-Fi の別端末から:        https://$$LAN_HOST:8443  （初回は証明書の警告を許可。確実に通すなら make ca-cert）"; \
	else \
	  echo "  同じ Wi-Fi の別端末から入るには: .env に LAN_HOST=$${LAN_IP:-<この PC の IP>} を書いて docker compose up -d proxy"; \
	fi; \
	echo ""

ca-cert: ## Caddy のローカル CA（root.crt）を書き出す（スマホ等に入れると証明書警告と WSS 失敗が消える）
	$(PROD_COMPOSE) cp proxy:/data/caddy/pki/authorities/local/root.crt _tools/caddy/root.crt
	@echo "書き出し: _tools/caddy/root.crt（git 管理外）。iPhone: AirDrop → 設定 > 一般 > VPNとデバイス管理 でインストール → 情報 > 証明書信頼設定 で有効化"

down: setup ## 本番構成を停止する（DB は保持）
	$(PROD_COMPOSE) down

dev-down: ## 開発構成を停止する（DB は保持）
	$(DEV_COMPOSE) down

clean: setup ## 本番コンテナ・ネットワークとビルド成果物を削除（DB は保持）
	$(PROD_COMPOSE) down --remove-orphans
	rm -rf frontend/dist frontend/dist-ssr frontend/coverage frontend/.vite \
	  frontend/node_modules/.tmp backend/_build backend/cover _tools/media/out

fclean: clean ## 本番 DB・証明書・ローカルイメージ・依存も削除（環境ファイルは保持）
	$(PROD_COMPOSE) down --volumes --rmi local --remove-orphans
	rm -rf frontend/node_modules backend/deps _tools/media/node_modules
	rm -f _tools/caddy/root.crt

re: clean ## 本番 DB を保持して再ビルド・起動
	@$(MAKE) --no-print-directory start

logs: setup ## 本番構成のログを追う
	$(PROD_COMPOSE) logs -f

dev-logs: ## 開発構成のログを追う
	$(DEV_COMPOSE) logs -f

db-seed: ## 開発 DB に画面確認用データを投入する（本番 DB は対象外）
	$(DEV_COMPOSE) run --rm backend sh -c "mix ecto.create --quiet && mix ecto.migrate --quiet && mix run priv/repo/seeds.exs"

db-shell: setup ## 本番 PostgreSQL へ接続する
	$(PROD_COMPOSE) exec db sh -c 'psql -U "$$POSTGRES_USER" -d "$$POSTGRES_DB"'

dev-db-shell: ## 開発 PostgreSQL へ接続する
	$(DEV_COMPOSE) exec db sh -c 'psql -U "$$POSTGRES_USER" -d "$$POSTGRES_DB"'
