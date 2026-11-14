# 認証と WebSocket のしくみ（図解）

> 最終更新: 2026-09-02。GitHub で開くと Mermaid がそのまま描画される。

3 行でまとめると:

1. **本人 = トークン**。サーバーはブラウザもタブも知らない。リクエストに付いたトークンを `sessions` テーブルで照合するだけ
2. **トークンの置き場が「誰になるか」を決める**。`localStorage` はブラウザ全体、`sessionStorage` はタブ単位。だから別タブ = 別プレイヤーにできる
3. **中継（Caddy / vite）は中身を見ない**。REST も WebSocket も同じトークンで、判定するのは Phoenix

## 1. 経路: ブラウザから DB まで

```mermaid
flowchart LR
  subgraph tab["ブラウザの 1 タブ（= 1 プレイヤー）"]
    T["token<br/>sessionStorage（タブ）→ localStorage（共通）の順に読む<br/>lib/api-client.ts getAuthToken()"]
    R["REST<br/>Authorization: Bearer token<br/>lib/api-client.ts"]
    W["WebSocket<br/>/socket/websocket?token=…<br/>lib/channel-client.ts"]
    T --> R
    T --> W
  end

  P["中継<br/>Caddy :8443（本結合）<br/>vite proxy :5173（npm run dev）"]
  R -- "/auth/* /users/* /rooms/* …" --> P
  W -- "/socket/*" --> P

  subgraph be["backend = Phoenix on Bandit :4000"]
    A["Plugs.Authenticate<br/>REST の本人確定<br/>→ conn.assigns.current_user"]
    S["UserSocket.connect/3<br/>WebSocket の本人確定<br/>→ socket.assigns.user_id"]
    X["Accounts.get_session_by_token/1<br/>sha256(token) で検索・期限チェック"]
    C["Controller<br/>（実装済み）"]
    K["Channel room:* / user:*<br/>（T2: これから）"]
    A --> X
    S --> X
    A --> C
    S --> K
  end
  P --> A
  P --> S
  X --> DB[("PostgreSQL<br/>sessions ⋈ users")]
```

REST と WebSocket で違うのは入口の関数（`Plugs.Authenticate.call/2` か `UserSocket.connect/3`）だけ。どちらも `Accounts.get_session_by_token/1` を呼び、以降のコード（Controller / Channel）は「本人はもう分かっている」前提で書く。

## 2. WebSocket が繋がるまで

```mermaid
sequenceDiagram
  participant B as ブラウザ（タブ）
  participant M as Caddy / vite proxy
  participant P as Phoenix UserSocket
  participant A as Accounts
  participant D as PostgreSQL

  B->>M: GET /socket/websocket?token=dev-token-yu&vsn=2.0.0<br/>Upgrade: websocket
  M->>P: そのまま転送（中身は見ない）
  P->>A: get_session_by_token("dev-token-yu")
  A->>D: SELECT sessions JOIN users<br/>WHERE token_hash = sha256(token) AND expires_at > now
  D-->>A: session + user
  A-->>P: {:ok, session}
  P-->>B: 101 Switching Protocols<br/>socket.assigns.user_id = "user_yu"
  Note over B,P: 以降は同じ接続の上で Channel の join / push / broadcast（T2 以降）
  Note over P: 無効なら connect/3 が :error → 403。接続は作られない<br/>（ログ: REFUSED CONNECTION TO WordWolfWeb.UserSocket）
```

REST の場合はこの図の `UserSocket.connect/3` が `Plugs.Authenticate.call/2` に、`101` が `200 + JSON` に、`403` が `401 UNAUTHENTICATED` に置き換わるだけ。

## 3. なぜ別タブが別プレイヤーになるのか

```mermaid
flowchart LR
  subgraph browser["同じブラウザ"]
    L["localStorage（全タブ共通）<br/>画面01 の通常ログインが書く"]
    subgraph t1["タブ 1"]
      S1["sessionStorage: dev-token-yu"]
    end
    subgraph t2["タブ 2"]
      S2["sessionStorage: dev-token-minato"]
    end
    subgraph t3["タブ 3"]
      S3["sessionStorage: なし → localStorage を使う"]
    end
  end
  S1 -- "Bearer dev-token-yu" --> P["Phoenix"]
  S2 -- "Bearer dev-token-minato" --> P
  L -. 共通 .-> S3
  S3 -- "Bearer （ログインした人）" --> P
  P --> U1["user_yu"]
  P --> U2["user_minato"]
  P --> U3["ログインした人"]
```

`sessionStorage` はブラウザの標準機能で、タブ（browsing context）ごとに独立している。開発用ページ `/dev/as/<name>` はここに `dev-token-<name>` を書くだけ。自分で登録したプレイヤーは `/dev/as` のフォームで `POST /auth/login` → 返ったトークンを sessionStorage に書く（`useAuth({ scope: 'tab' })`）。本番ビルドにはこのページは含まれない。

## 4. 層の重なり（どこまで出来ていて、次は何か）

```mermaid
flowchart TB
  L1["トランスポート: WebSocket 1 本 / タブ（RFC 6455）<br/>Caddy・vite は Upgrade をそのまま通す"]
  L2["Phoenix.Socket: 接続と本人確認<br/>backend/lib/word_wolf_web/channels/user_socket.ex ✅"]
  L3["Phoenix.Channel: トピック room:{roomId} / user:{userId}<br/>join → room.state、push / broadcast<br/>room_channel.ex ⏳ T2、FE use-room-channel.ts ⏳ T3"]
  L4["GameServer: 部屋ごとの状態機械（LOBBY → … → RESULT）<br/>games/game_server.ex ⏳ B1"]
  L1 --> L2 --> L3 --> L4
```

いま `/dev/as` の「WebSocket 接続テスト」で見せられるのは L2 まで（本人確認済みの回線が開く）。L3 からがゲームの実装。

## 4.5 同じ Wi-Fi の別端末から繋がるしくみ

`localhost` は「その端末自身」を指すので、スマホから `https://localhost:8443` を開いてもスマホ自身に繋ごうとして失敗する。
代わりに **この PC が Wi-Fi ルーターからもらっている IP**（例 `192.168.1.23`。`make urls` が候補を出す）を使う。

```mermaid
flowchart LR
  Phone["スマホ / 別 PC<br/>https://192.168.1.23:8443"]
  Router["Wi-Fi ルーター<br/>同じサブネットなので<br/>直接届く"]
  subgraph mac["この PC（192.168.1.23）"]
    Port["Docker が公開したポート<br/>0.0.0.0:8443 → proxy:443<br/>（compose.yaml の ports）"]
    subgraph proxy["proxy コンテナ（Caddy）"]
      Site["site address<br/>localhost, {$LAN_HOST}<br/>→ 192.168.1.23 宛ても受ける"]
      Cert["tls internal<br/>IP 192.168.1.23 用の証明書を発行<br/>default_sni で SNI 無しにも使う"]
    end
    BE["backend:4000（Phoenix）"]
    FE["frontend:5173（vite）"]
  end
  Phone --> Router --> Port --> Site --> Cert
  Cert -- "/auth/* /users/* /socket/*" --> BE
  Cert -- "それ以外" --> FE
```

各段で何が起きているか:

| 段 | 何が必要か | どこで設定しているか |
|---|---|---|
| 1. 届く | スマホと PC が同じ Wi-Fi（同じサブネット）にいる。PC の IP を知っている。macOS のファイアウォールで Docker が塞がれていない | `make urls` が `ipconfig getifaddr en0` で IP 候補を表示 |
| 2. Docker に入る | ポート 8443 が `0.0.0.0`（全インターフェース）で公開されている | `compose.yaml` の `proxy.ports: "8443:443"`（既定で全インターフェース） |
| 3. Caddy が受ける | Caddy は **設定に書いた host 宛て**しか応答しない。IP 宛てを受けるには site address に IP が要る。さらに TLS では「その名前の証明書」が要る | `.env` の `LAN_HOST=192.168.1.23` → `compose.yaml` が proxy に渡す → Caddyfile の `localhost, {$LAN_HOST}` と `default_sni {$LAN_HOST}` |
| 4. あとは同じ | パスで backend / frontend に振り分け。FE は同一オリジンで API を呼ぶので、`https://192.168.1.23:8443/users/me` も Caddy が backend へ | `_tools/caddy/Caddyfile`（localhost と同じ経路） |

つまずきやすい点:

- **証明書の警告**。Caddy の `tls internal` は自前の CA で署名するので、スマホは「信頼できない」と言う。HTTP は警告を許可すれば進めるが、**iOS Safari は自己署名だと WebSocket（wss）が失敗する**ことがある。`make ca-cert` で書き出した `_tools/caddy/root.crt` をスマホに入れて信頼させると、警告も wss の失敗も消える
- **`default_sni` が要る理由**。ブラウザは IP 直打ちのとき TLS の SNI（どの名前の証明書が欲しいか）を送らない。Caddy は普通「接続先 IP に一致する証明書」で代替するが、Docker の NAT を通ると Caddy から見た接続先はコンテナの IP（172.x）になり一致しない。そこで「SNI が無ければ LAN_HOST の証明書を使う」と固定している
- **IP が変わる**。Wi-Fi を変えると PC の IP も変わるので、`LAN_HOST` を書き直して `docker compose up -d proxy`
- **PC の localhost からも今までどおり**。site address は `localhost, {$LAN_HOST}` の両方なので、開発者本人の `https://localhost:8443` は影響を受けない

nginx でも考え方は同じで、`server_name` に IP を足し、その IP を SAN に含む証明書を置く、に対応する。

## 5. 対応するファイル

| 役目 | ファイル |
|---|---|
| トークンの読み書き（唯一の場所） | `frontend/src/lib/api-client.ts` |
| REST に Bearer を付ける | 同上（middleware） |
| WebSocket に token を渡す | `frontend/src/lib/channel-client.ts` `createSocket()` |
| タブ単位ログイン（開発） | `frontend/src/pages/dev-login-page.tsx`, `frontend/src/hooks/use-auth.ts`（`scope: 'tab'`） |
| REST の本人確定 | `backend/lib/word_wolf_web/plugs/authenticate.ex` |
| WebSocket の本人確定 | `backend/lib/word_wolf_web/channels/user_socket.ex` |
| 照合の実体 | `backend/lib/word_wolf/accounts.ex` `get_session_by_token/1`, `hash_token/1` |
| 接続の入口 | `backend/lib/word_wolf_web/endpoint.ex` `socket "/socket"` |
| 中継の設定 | `_tools/caddy/Caddyfile`, `frontend/vite.config.ts` |
| 別端末からのアクセス | `.env` の `LAN_HOST`, `compose.yaml`（proxy.environment / ports）, `Makefile`（`urls` / `ca-cert`） |
| 契約 | `_docs/spec/api/openapi.yaml`（REST）, `_docs/spec/api/asyncapi.yaml`（`servers.development` の `token`、channels） |

## 6. 文献

Phoenix / Plug（公式）

- Phoenix.Socket — 接続と `connect/3`, `id/1`: https://hexdocs.pm/phoenix/Phoenix.Socket.html
- Phoenix.Channel — トピック、join / push / broadcast: https://hexdocs.pm/phoenix/Phoenix.Channel.html
- Channels ガイド（全体像と JS クライアント）: https://hexdocs.pm/phoenix/channels.html
- phoenix.js（`new Socket(url, { params })`, `channel()`）: https://hexdocs.pm/phoenix/js/
- Phoenix.Token — 本番で URL に載せる短命トークンの作り方: https://hexdocs.pm/phoenix/Phoenix.Token.html
- mix phx.gen.auth ガイド — セッショントークンをハッシュで保存する考え方: https://hexdocs.pm/phoenix/mix_phx_gen_auth.html
- Plug.Conn — `conn` の中身と `assign/3`, `halt/1`: https://hexdocs.pm/plug/Plug.Conn.html
- Bandit（HTTP / WebSocket サーバー）: https://hexdocs.pm/bandit/

ブラウザ側

- MDN `Window.sessionStorage`（タブ単位の保存領域）: https://developer.mozilla.org/en-US/docs/Web/API/Window/sessionStorage
- MDN `Window.localStorage`: https://developer.mozilla.org/en-US/docs/Web/API/Window/localStorage
- MDN WebSocket API: https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API
- RFC 6455 — The WebSocket Protocol（`Upgrade: websocket` と 101）: https://www.rfc-editor.org/rfc/rfc6455

中継

- Caddy `reverse_proxy`（WebSocket を透過的に通す）: https://caddyserver.com/docs/caddyfile/directives/reverse_proxy
- Caddy `default_sni`（SNI 無し接続に使う証明書）: https://caddyserver.com/docs/caddyfile/options#default-sni
- Caddy Automatic HTTPS / `tls internal`（ローカル CA）: https://caddyserver.com/docs/automatic-https
- Docker Compose `ports`（ホストの全インターフェースに公開）: https://docs.docker.com/reference/compose-file/services/#ports
- MDN Server Name Indication（SNI）: https://developer.mozilla.org/en-US/docs/Glossary/SNI
- Vite `server.proxy`（`ws: true`）: https://vite.dev/config/server-options#server-proxy

このリポジトリ内

- 縦串の読み方: [../dev/code-walkthrough.md](../dev/code-walkthrough.md)（§4 認証 Plug、§8.5 デモ手順、§10 Phoenix の標準形）
- 次のタスク: [../dev/parallel-tasks.md](../dev/parallel-tasks.md)（T2 / T3 / B1）
