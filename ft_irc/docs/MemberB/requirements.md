
### MemberB

## 責任範囲フロー

\```
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
\```

## 一覧
- パース
- コマンド
    - PASS
    - NICK
    - USER
    - JOIN
    - PRIVMSG
    - KICK
    - INVITE
    - TOPIC
    - MODE


## コマンド一覧

| コマンド | フェーズ | 成功時の返答 | 失敗時の返答 | 依存 |
|---------|---------|------------|------------|------|
| PASS | M1 | - | 461, 462, 464 | - |
| NICK | M1 | - | 431, 433 | - |
| USER | M1 | 001 (welcome) | 461, 462 | - |
| JOIN | M2 | 331/332, 353, 366 | 461, 471, 473, 475 | ChannelManager |
| PRIVMSG | M2 | - | 401, 404, 461 | ChannelManager, Server |
| KICK | M3 | - | 461, 482 | ChannelManager |
| INVITE | M3 | 341 | 461, 482 | ChannelManager |
| TOPIC | M3 | 331/332 | 461, 482 | ChannelManager |
| MODE | M3 | - | 461, 472, 482 | ChannelManager |


## IRCメッセージ文法（BNF）

### メッセージ全体の構造
\```
<message> ::= [':' <prefix> <SPACE>] <command> <params> <crlf>
<prefix>  ::= <servername> | <nick> ['!' <user>] ['@' <host>]
<command> ::= <letter>{<letter>} | <number><number><number>
<params>  ::= <SPACE> [':' <trailing> | <middle> <params>]
<middle>  ::= スペース・NUL・CR・LF以外の文字列（':'始まり不可）
<trailing>::= NUL・CR・LF以外の文字列（空文字OK、スペースOK）
<crlf>    ::= CR LF
\```

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

### numeric replyのフォーマット
\```
:server <3桁数字> <nick> <内容>\r\n
\```
例：
\```
:ft_irc 332 alice #channel :Welcome
:ft_irc 353 alice = #channel :alice bob charlie
\```
