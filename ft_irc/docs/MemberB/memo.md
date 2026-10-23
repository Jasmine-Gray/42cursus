# Member B 設計メモ

## 方針
- numeric reply の番号はとりあえずハードコーディング（定数化は後でMember Cと相談）

---

## 責任範囲フロー

```
クライアントから1行受け取る（Member Aが渡してくる）
        ↓
┌─────────────────────────────┐
│        Member B の領域        │
│                               │
│  CommandParser::parseLine()   │
│  → command + params に分解    │
│                               │
│  CommandDispatcher::dispatch()│
│  → コマンドに応じてハンドラー呼び出し│
│                               │
│  各ハンドラー                  │
│  → バリデーション              │
│  → Member Cのモデルを操作      │
│  → 返答をキューに積む          │
│                               │
└─────────────────────────────┘
        ↓
Member Aがキューから取り出してsend()
```

---

## IRCメッセージ文法（BNF）

### メッセージ全体の構造
```
<message> ::= [':' <prefix> <SPACE>] <command> <params> <crlf>
<prefix>  ::= <servername> | <nick> ['!' <user>] ['@' <host>]
<command> ::= <letter>{<letter>} | <number><number><number>
<params>  ::= <SPACE> [':' <trailing> | <middle> <params>]
<middle>  ::= スペース・NUL・CR・LF以外の文字列（':'始まり不可）
<trailing>::= NUL・CR・LF以外の文字列（空文字OK、スペースOK）
<crlf>    ::= CR LF
```

### ポイント
- prefix は省略可（クライアント→サーバーは基本省略）
- command は大文字に正規化して処理する
- trailing は ':' で始まり、スペースを含められる
- メッセージ全体は512文字以内（CRLF込み）
- 空メッセージは無視する

### 各コマンドのパラメータ形式
| コマンド | パラメータ |
|---------|-----------|
| PASS | \<password\> |
| NICK | \<nickname\> |
| USER | \<username\> \<hostname\> \<servername\> \<realname\> |
| JOIN | \<channel\>{,\<channel\>} [\<key\>{,\<key\>}] |
| PRIVMSG | \<target\> \<text\> |
| KICK | \<channel\> \<user\> [\<comment\>] |
| INVITE | \<nickname\> \<channel\> |
| TOPIC | \<channel\> [\<topic\>] |
| MODE | \<channel\> \<modestring\> [\<args\>] |

### numeric reply のフォーマット
```
:server <3桁数字> <nick> <内容>\r\n
```
例：
```
:ft_irc 332 alice #channel :Welcome
:ft_irc 353 alice = #channel :alice bob charlie
```

---

## コマンド一覧

| コマンド | フェーズ | 成功時の返答 | 失敗時の返答 | 依存 |
|---------|---------|------------|------------|------|
| PASS | M1 | なし | 461, 462, 464 | - |
| NICK | M1 | なし | 431, 433 | - |
| USER | M1 | 001 (welcome) | 461, 462 | - |
| JOIN | M2 | 331/332, 353, 366 | 461, 471, 473, 475 | ChannelManager |
| PRIVMSG | M2 | なし | 401, 404, 461 | ChannelManager, Server |
| KICK | M3 | なし | 461, 482 | ChannelManager |
| INVITE | M3 | 341 | 461, 482 | ChannelManager |
| TOPIC | M3 | 331/332 | 461, 482 | ChannelManager |
| MODE | M3 | なし | 461, 472, 482 | ChannelManager |

---

## コマンド詳細

### PASS
**役割**: 接続パスワードの検証

**フロー**:
1. params が空 → `461 ERR_NEEDMOREPARAMS`
2. 登録済み → `462 ERR_ALREADYREGISTRED`
3. パスワード不一致 → `464 ERR_PASSWDMISMATCH`
4. 一致 → `passwordAccepted_ = true`、3つ揃えば `001` welcome送信

**制約**:
- NICK/USER より前に送る必要がある（推奨順序）
- 登録後は変更不可
- ft_irc では起動時にパスワード必須なので PASS なしは登録不可

---

### NICK
**役割**: ニックネームの設定・変更

**フロー**:
1. params が空 → `431 ERR_NONICKNAMEGIVEN`
2. nick が既に使われている → `433 ERR_NICKNAMEINUSE`
3. 成功 → `nickname_` を更新、3つ揃えば `001` welcome送信

**制約**:
- 最大9文字
- 使える文字: letter/number/special（RFCのBNF参照）
- 登録後も変更可能

---

### USER
**役割**: ユーザー情報の登録

**フロー**:
1. params が4未満 → `461 ERR_NEEDMOREPARAMS`
2. 登録済み → `462 ERR_ALREADYREGISTRED`
3. 成功 → `username_`, `realname_` を更新、3つ揃えば `001` welcome送信

**制約**:
- 登録後は変更不可
- hostname と servername はサーバー側で無視する（セキュリティ上の理由）
- realname は trailing（`:` 始まり）で空白を含む可能性あり

---

### JOIN
**役割**: チャンネルへの参加

**フロー**:
1. params が空 → `461 ERR_NEEDMOREPARAMS`
2. invite-only (+i) かつ未招待 → `473 ERR_INVITEONLYCHAN`
3. key設定済み (+k) かつ不一致 → `475 ERR_BADCHANNELKEY`
4. 満員 (+l) → `471 ERR_CHANNELISFULL`
5. 成功 →
   - チャンネル全員に `JOIN` メッセージをブロードキャスト
   - `331 RPL_NOTOPIC` または `332 RPL_TOPIC`
   - `353 RPL_NAMREPLY`（ユーザー一覧）
   - `366 RPL_ENDOFNAMES`

**未解決**:
- RPL_NAMREPLY のユーザー一覧は ChannelManager から取得する予定
- ReplyBuilder に topic/namreply 用メソッドの追加が必要

---

### PRIVMSG
**役割**: メッセージ送信（ユーザー宛て・チャンネル宛て）

**フロー（チャンネル宛て）**:
1. params が2未満 → `461 ERR_NEEDMOREPARAMS`
2. `#` 始まり → `channelManager_.sendToChannel()` を呼ぶ
3. 失敗 → `404 ERR_CANNOTSENDTOCHAN`

**フロー（ユーザー宛て）**:
1. nick で ClientSession を探す
2. 見つからない → `401 ERR_NOSUCHNICK`
3. 見つかった → その ClientSession に `enqueueReply()`

**未解決**:
- ユーザー宛ては「nickでClientSessionを探す」仕組みが必要
- Server クラスにクライアントリストが必要 → Member A と相談

---

### KICK
**役割**: チャンネルからユーザーを強制退出させる

**フロー**:
1. params が2未満 → `461 ERR_NEEDMOREPARAMS`
2. チャンネルが存在しない → `403 ERR_NOSUCHCHANNEL`
3. 自分がチャンネルにいない → `442 ERR_NOTONCHANNEL`
4. 自分がオペレーターでない → `482 ERR_CHANOPRIVSNEEDED`
5. 対象ユーザーがチャンネルにいない → `441 ERR_USERNOTINCHANNEL`
6. 成功 → チャンネル全員に `KICK` メッセージをブロードキャスト

**制約**:
- channel operator のみ実行可能
- コメント（reason）は省略可、省略時はnickをデフォルトにする

---

### INVITE
**役割**: invite-only チャンネルにユーザーを招待する

**フロー**:
1. params が2未満 → `461 ERR_NEEDMOREPARAMS`
2. チャンネルが存在しない → `403 ERR_NOSUCHCHANNEL`
3. 自分がチャンネルにいない → `442 ERR_NOTONCHANNEL`
4. 自分がオペレーターでない（+i の場合）→ `482 ERR_CHANOPRIVSNEEDED`
5. 対象ユーザーが既にチャンネルにいる → `443 ERR_USERONCHANNEL`
6. 成功 →
   - 送信者に `341 RPL_INVITING`
   - 対象ユーザーに `INVITE` メッセージ

**制約**:
- +i モードのチャンネルではオペレーターのみ実行可能
- 招待されたユーザーはinviteリストに追加される

---

### TOPIC
**役割**: チャンネルのトピックを取得・設定する

**フロー（取得）**:
1. params が1つだけ → トピック取得
2. トピックあり → `332 RPL_TOPIC`
3. トピックなし → `331 RPL_NOTOPIC`

**フロー（設定）**:
1. params が2つ → トピック設定
2. チャンネルが存在しない → `403 ERR_NOSUCHCHANNEL`
3. 自分がチャンネルにいない → `442 ERR_NOTONCHANNEL`
4. +t モードかつオペレーターでない → `482 ERR_CHANOPRIVSNEEDED`
5. 成功 → チャンネル全員に `TOPIC` メッセージをブロードキャスト

**制約**:
- +t モードのとき、オペレーターのみ変更可能
- +t なしのとき、全員が変更可能

---

### MODE
**役割**: チャンネルのモードを設定・解除する

**フロー**:
1. params が2未満 → `461 ERR_NEEDMOREPARAMS`
2. チャンネルが存在しない → `403 ERR_NOSUCHCHANNEL`
3. オペレーターでない → `482 ERR_CHANOPRIVSNEEDED`
4. 不明なモード文字 → `472 ERR_UNKNOWNMODE`
5. 成功 → チャンネル全員に `MODE` メッセージをブロードキャスト

**対応モード**:
| フラグ | 意味 | 引数 |
|--------|------|------|
| +i / -i | invite-only | なし |
| +t / -t | トピック変更をオペレーターのみに制限 | なし |
| +k / -k | チャンネルキー（パスワード）設定 | +k: キー文字列 |
| +o / -o | オペレーター権限の付与・剥奪 | nick |
| +l / -l | 最大ユーザー数の設定 | +l: 数値 |

**未解決**:
- モードの状態管理は Member C の ChannelManager が持つ
- Member B はモード文字をパースして ChannelManager に渡すだけ
