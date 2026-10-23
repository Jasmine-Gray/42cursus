# ft_irc 関数フロー (main からの流れ)

`main()` を起点に、起動 → イベントループ → コマンド処理 → 応答送信までの
関数呼び出しの流れを Mermaid で図示する。

---

## 1. 起動フロー: `main()` → `Server::run()`

```mermaid
flowchart TD
    A["main(argc, argv)"] --> B{"argc == 3 ?"}
    B -->|No| B1["printUsage()<br/>return 1"]
    B -->|Yes| C{"parsePort(argv[1])<br/>1〜65535 ?"}
    C -->|No| C1["エラー出力<br/>return 1"]
    C -->|Yes| D{"password 非空 ?"}
    D -->|No| D1["エラー出力<br/>return 1"]
    D -->|Yes| E["Server server(port, password)"]
    E --> F["server.run()"]
    F --> G["return 終了コード"]
```

---

## 2. イベントループ: `Server::run()`

```mermaid
flowchart TD
    R["Server::run()"] --> S1["signal(SIGPIPE, SIG_IGN)"]
    S1 --> S2{"setupListenSocket()<br/>socket→bind→listen"}
    S2 -->|失敗| S2F["return 1"]
    S2 -->|成功| LOOP["while(true) ループ開始"]

    LOOP --> UP["updatePollEvents()<br/>各fdのPOLLIN/POLLOUTを再設定"]
    UP --> POLL["poll(pollfds, -1)"]
    POLL -->|エラー かつ EINTR| LOOP
    POLL -->|エラー| PF["return 1"]
    POLL -->|イベントあり| FOR["各 pollfd を走査"]

    FOR --> ISSRV{"fd == serverFd_ ?"}
    ISSRV -->|Yes & POLLIN| ACC["acceptClients()"]
    ISSRV -->|No| RD{"POLLIN ?"}
    RD -->|Yes| HRD["handleClientRead(fd)"]
    HRD --> WR
    RD -->|No| WR{"POLLOUT ?"}
    WR -->|Yes| HWR["handleClientWrite(fd)"]
    HWR --> ER
    WR -->|No| ER{"POLLERR / POLLHUP / POLLNVAL ?"}
    ER -->|Yes| CC["closeClient(fd)"]
    ACC --> NEXT["次の fd へ"]
    ER --> NEXT
    CC --> NEXT
    NEXT --> FOR
    FOR -->|走査完了| LOOP
```

`acceptClients()` は新規接続を `accept()` し、ノンブロッキング化して
`clients_` / `pollfds_` / `outputBuffers_` に登録する。

---

## 3. 受信 → 解析 → ディスパッチ: `handleClientRead()`

```mermaid
flowchart TD
    HR["handleClientRead(fd)"] --> RECV["recv() ループ"]
    RECV -->|received > 0| APP["client.appendInput(buf)<br/>受信バッファに追加"]
    RECV -->|received == 0| DISC["closeClient(fd)<br/>切断"]
    RECV -->|EAGAIN/EWOULDBLOCK| RET["return (読み切り)"]

    APP --> PNL{"client.popNextLine(line)<br/>\\r\\n 区切りで1行取り出し"}
    PNL -->|行あり & 非空| DISP["dispatcher_.dispatch(client,<br/>CommandParser::parseLine(line))"]
    DISP --> QPR["queuePendingReplies(fd)<br/>送信者のpending→outputBuffers_へ"]
    QPR --> PNL
    PNL -->|行なし| RECV
```

`CommandParser::parseLine()` は1行を `ParsedCommand`
(コマンド名 + パラメータ列) に分解する。

---

## 4. コマンドディスパッチ: `CommandDispatcher::dispatch()`

```mermaid
flowchart TD
    D["dispatch(client, command)"] --> EMP{"command.empty() ?"}
    EMP -->|Yes| RET["return"]
    EMP -->|No| NAME["name = command.getCommand()"]
    NAME --> SW{"コマンド名で分岐"}

    SW -->|PASS| H1["handlePASS"]
    SW -->|NICK| H2["handleNICK"]
    SW -->|USER| H3["handleUSER"]
    SW -->|PING| H4["handlePING"]
    SW -->|PONG| H5["handlePONG (無応答)"]
    SW -->|JOIN| H6["handleJOIN"]
    SW -->|PRIVMSG| H7["handlePRIVMSG"]
    SW -->|KICK| H8["handleKICK"]
    SW -->|INVITE| H9["handleINVITE"]
    SW -->|TOPIC| H10["handleTOPIC"]
    SW -->|MODE| H11["handleMODE"]
    SW -->|その他| HU["reply(ERR_UNKNOWNCOMMAND)"]

    H1 --> TW["tryWelcome()<br/>登録完了なら RPL_WELCOME(001)"]
    H2 --> TW
    H3 --> TW
```

各ハンドラは3つの協力オブジェクトを使う:

| 依存 | 役割 |
|------|------|
| `replyBuilder_` | 応答文字列を組み立てる (`reply` / `numeric` / `channelMessage` / `pong`) |
| `channelManager_` | チャンネル状態の参照・更新 (join/kick/mode/topic/membership) |
| `server_` | 他クライアントへの配信 (`broadcastToChannel` / `sendToNick` / `changeNickname`) |

---

## 5. ハンドラ内部の典型フロー (例: `handleJOIN` / `handlePRIVMSG`)

```mermaid
flowchart TD
    subgraph JOIN["handleJOIN"]
        J1{"params 空 ?"} -->|Yes| JE["reply(ERR_NEEDMOREPARAMS)"]
        J1 -->|No| J2["招待制/鍵/満員チェック<br/>(channelManager_)"]
        J2 -->|拒否| JE2["reply(ERR_*)"]
        J2 -->|許可| J3["channelManager_.join()"]
        J3 --> J4["server_->broadcastToChannel(<br/>JOIN メッセージ)"]
        J4 --> J5["RPL_TOPIC / RPL_NAMREPLY を本人へ"]
    end

    subgraph PRIVMSG["handlePRIVMSG"]
        P1{"params < 2 ?"} -->|Yes| PE["reply(ERR_NEEDMOREPARAMS)"]
        P1 -->|No| P2{"宛先が #channel ?"}
        P2 -->|Yes| P3["channelManager_.sendToChannel()<br/>=メンバー確認"]
        P3 -->|非メンバー| PE2["reply(ERR_CANNOTSENDTOCHAN)"]
        P3 -->|OK| P4["server_->broadcastToChannel(<br/>本人を除外)"]
        P2 -->|No (nick宛)| P5["server_->sendToNick()"]
        P5 -->|不在| PE3["reply(ERR_NOSUCHNICK)"]
    end
```

---

## 6. 配信が相手に届くまで (POLLOUT による flush)

他クライアント宛の応答は、ハンドラ内で相手の `ClientSession` の
送信キューに積まれるだけ。実際の `send()` は次のループで POLLOUT が
立ってから行われる。

```mermaid
sequenceDiagram
    participant A as Alice (送信者)
    participant Loop as run() ループ
    participant Disp as CommandDispatcher
    participant Srv as Server
    participant B as Bob (受信者 ClientSession)

    A->>Loop: PRIVMSG #room :hi
    Loop->>Disp: handleClientRead → dispatch
    Disp->>Srv: broadcastToChannel(#room, msg, exc= Alice)
    Srv->>B: sendToClient → B.enqueueReply(msg)
    Note over B: Bob の送信キューに積まれる
    Loop->>Loop: 次周回 updatePollEvents()<br/>Bob.hasPendingOutput()→ POLLOUT 付与
    Loop->>Srv: handleClientWrite(Bob.fd)
    Srv->>B: queuePendingReplies → send()
    B-->>A: (Bob のクライアントが受信)
```

---

## まとめ (呼び出し階層)

```
main()
└─ Server::run()
   ├─ setupListenSocket()        // socket / bind / listen
   └─ while(true)
      ├─ updatePollEvents()      // POLLIN/POLLOUT 再計算
      ├─ poll()
      ├─ acceptClients()         // 新規接続
      ├─ handleClientRead(fd)
      │  ├─ ClientSession::appendInput / popNextLine
      │  ├─ CommandParser::parseLine()
      │  ├─ CommandDispatcher::dispatch()
      │  │  └─ handlePASS/NICK/USER/PING/JOIN/PRIVMSG/KICK/INVITE/TOPIC/MODE
      │  │     ├─ ReplyBuilder::reply / pong / channelMessage
      │  │     ├─ ChannelManager::join / kick / setMode / ...
      │  │     └─ Server::broadcastToChannel / sendToNick / changeNickname
      │  └─ queuePendingReplies(fd)
      ├─ handleClientWrite(fd)    // send() で実送信
      └─ closeClient(fd)          // 切断時
```
