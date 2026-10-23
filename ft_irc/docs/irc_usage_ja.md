# ft_irc 操作マニュアル

このドキュメントは、チーム内で `ircserv` を実際に触るためのメモです。
IRC に慣れていない人が、接続方法、チャンネル参加、メッセージ送信、
operator コマンド、mode の意味を一通り確認できることを目的にしています。

## まず覚えること

IRC は「サーバーに接続したユーザー」が「チャンネル」に入って会話する
テキストプロトコルです。

よく使う言葉は次の通りです。

| 用語 | 意味 |
| --- | --- |
| server | `./ircserv 6667 pass` で起動する IRC サーバー |
| client | irssi、nc、HexChat など、サーバーへ接続するプログラム |
| nick | IRC 上で見える名前。例: `bob`, `alice` |
| user | 登録時に送る username。nick と同じでなくてもよい |
| channel | 会話部屋。基本的に `#test` のように `#` から始まる |
| operator | チャンネル管理権限を持つユーザー。表示では `@bob` のようになる |

`bob` や `alice` は特別な名前ではありません。説明用の例です。
`testuser`, `user1`, `sato` など任意の nick で構いません。

## サーバーを起動する

リポジトリルートでビルドします。

```bash
make
```

サーバーを起動します。

```bash
./ircserv 6667 pass
```

この例では次の意味です。

| 値 | 意味 |
| --- | --- |
| `6667` | 接続を待ち受ける port |
| `pass` | クライアントが接続時に送る password |

このターミナルは閉じずにそのままにします。
別ターミナルから irssi や nc で接続します。

## irssi で接続する

起動コマンドで一気に接続する場合:

```bash
irssi -c localhost -p 6667 -w pass -n bob
```

irssi の中から接続する場合:

```text
/set nick bob
/connect 127.0.0.1 6667 pass
```

`-password` という option は irssi にはありません。
次はエラーになります。

```text
/connect -password pass -nick bob 127.0.0.1 6667
```

接続に成功すると、次のような表示が出ます。

```text
Welcome to ft_irc
```

`Waiting for CAP LS response...` が出ることがありますが、CAP 応答後に
`Welcome to ft_irc` が出れば接続と登録は成功しています。

## チャンネルに入る

IRC のチャンネル名は基本的に `#` から始めます。

```text
/join #test
```

成功すると次のような表示になります。

```text
bob has joined #test
[@bob]
```

最初にチャンネルへ入った人は operator になります。
irssi では `@bob` のように `@` が付きます。

次は channel 名として不正なので避けます。

```text
/join bob
```

`bob` は nick としては使えますが、channel 名ではありません。
この場合、`No such channel` が出ます。

## 2人で接続して動作確認する

1つ目のターミナル:

```bash
./ircserv 6667 pass
```

2つ目のターミナル:

```bash
irssi -c localhost -p 6667 -w pass -n bob
```

irssi 内で:

```text
/join #test
```

3つ目のターミナル:

```bash
irssi -c localhost -p 6667 -w pass -n alice
```

irssi 内で:

```text
/join #test
/msg #test hello
```

期待すること:

| 操作 | 期待結果 |
| --- | --- |
| `bob` が `/join #test` | `bob` が `@bob` として operator になる |
| `alice` が `/join #test` | `bob` 側に `alice has joined #test` が見える |
| `alice` が `/msg #test hello` | `bob` 側に `alice` からの `hello` が届く |

## irssi の基本コマンド

| やりたいこと | irssi command |
| --- | --- |
| サーバーに接続 | `/connect 127.0.0.1 6667 pass` |
| nick を変える | `/nick newnick` |
| チャンネルに入る | `/join #test` |
| チャンネルへ送信 | `/msg #test hello` |
| 特定ユーザーへ送信 | `/msg bob hello` |
| チャンネルを出る | `/part #test` |
| topic を見る | `/topic #test` |
| topic を設定 | `/topic #test new topic` |
| mode を見る | `/mode #test` |
| mode を設定 | `/mode #test +i` |
| ユーザーを kick | `/kick #test alice reason` |
| ユーザーを invite | `/invite alice #test` |
| 接続を切る | `/disconnect` |
| irssi を終了 | `/quit` |

`/quit` は irssi 自体を終了します。ft_irc は raw `QUIT` command にも対応しているので、
同じ channel にいる user には `:nick QUIT :message` が届きます。

## nc で raw IRC を試す

irssi は便利ですが、裏で `CAP`, `WHO`, `MODE` などを自動で送ることがあります。
プロトコルを直接確認したいときは `nc` が便利です。

使い分けは次のイメージです。

| ツール | 向いていること |
| --- | --- |
| `nc -C` | raw IRC command を手で送り、server の返事をそのまま見る |
| `ncat --crlf` | `nc -C` が使えない環境で raw IRC を見る |
| `socat` | CRLF を明示して raw IRC を見る。少し上級者向け |
| `telnet` | 最後の代替。環境によって余計な表示や挙動がある |
| irssi | 本物の IRC client との互換確認。最終確認向き |

実装中に「今どの command を送って、何が返ってきたか」を見たいなら、
まず `nc -C` を使うのが一番分かりやすいです。
irssi は `/join` や `/msg` などを raw IRC に変換してくれますが、
そのぶん裏で何が送られたかが見えにくくなります。

```bash
nc -C localhost 6667
```

接続したら、手で IRC command を送ります。

```text
PASS pass
NICK bob
USER bob 0 * :Bob
JOIN #test
PRIVMSG #test :hello
PING :12345
```

登録に必要なのは基本的にこの3つです。

```text
PASS pass
NICK bob
USER bob 0 * :Bob
```

`USER` の形式は次です。

```text
USER <username> 0 * :<realname>
```

`NICK` と `USER` は同じ文字列でなくても構いません。
画面に表示されたり、`PRIVMSG bob :hello` の宛先になるのは `NICK` です。

### `nc -C` が使えない場合

環境によっては `nc` に `-C` option がないことがあります。
その場合は、次のどれかを使います。

```bash
ncat --crlf 127.0.0.1 6667
```

または:

```bash
socat - TCP:127.0.0.1:6667,crlf
```

`telnet` でも接続できます。

```bash
telnet 127.0.0.1 6667
```

ただし IRC の正式な行末は CRLF、つまり `\r\n` です。
`nc -C`, `ncat --crlf`, `socat ...,crlf` はこの行末を扱いやすくしてくれます。

### raw 確認のおすすめ手順

2つのターミナルで `nc -C` を起動します。

1人目:

```bash
nc -C 127.0.0.1 6667
```

```text
PASS pass
NICK bob
USER bob 0 * :Bob
JOIN #test
```

2人目:

```bash
nc -C 127.0.0.1 6667
```

```text
PASS pass
NICK alice
USER alice 0 * :Alice
JOIN #test
PRIVMSG #test :hello bob
PRIVMSG bob :direct hello
```

1人目の画面に、2人目の `JOIN` と `PRIVMSG` が届けば、
channel broadcast と direct message の基本確認ができます。

期待する返事の例:

```text
:ft_irc 001 bob :Welcome to ft_irc
:bob JOIN #test
:ft_irc 331 bob #test :No topic is set
:ft_irc 353 bob = #test :@bob
:ft_irc 366 bob #test :End of /NAMES list
:alice JOIN #test
:alice PRIVMSG #test :hello bob
:alice PRIVMSG bob :direct hello
```

## raw command 早見表

| command | 目的 | 例 |
| --- | --- | --- |
| `PASS` | 接続 password を送る | `PASS pass` |
| `NICK` | 表示名、宛先名を設定する | `NICK bob` |
| `USER` | username / realname を登録する | `USER bob 0 * :Bob` |
| `CAP` | client capability の問い合わせ | `CAP LS` |
| `PING` | 接続確認。server は `PONG` を返す | `PING :12345` |
| `JOIN` | channel に入る | `JOIN #test` |
| `PART` | channel から出る | `PART #test :bye` |
| `NAMES` | channel の nick 一覧を見る | `NAMES #test` |
| `WHO` | client の自動問い合わせに応答する | `WHO #test` |
| `QUIT` | 接続を終了し、共有 channel に通知する | `QUIT :bye` |
| `PRIVMSG` | channel または user にメッセージを送る | `PRIVMSG #test :hello` |
| `KICK` | channel から user を追い出す | `KICK #test alice :bye` |
| `INVITE` | user を channel に招待する | `INVITE alice #test` |
| `TOPIC` | topic を見る、または設定する | `TOPIC #test :new topic` |
| `MODE` | channel mode を見る、または変更する | `MODE #test +i` |

`WHO`, `NAMES`, `QUIT` は mandatory の中心コマンドではありませんが、
reference client が自然に動くように最小互換として対応しています。
`NAMES` は `353` と `366`、`WHO` は `315` を返します。

## ft_irc mini RFC

ここでは、ft_irc が扱う command を「簡単な RFC」っぽく整理します。

表の読み方:

| 表記 | 意味 |
| --- | --- |
| `<name>` | 必須引数 |
| `[name]` | 省略可能な引数 |
| `:<text>` | 空白を含められる trailing parameter |
| `#channel` | `#` から始まる channel 名 |
| `nick` | user の nickname |

### 登録系コマンド

IRC では、通常 `PASS`, `NICK`, `USER` がそろうと登録完了になります。
登録完了すると server から `001 RPL_WELCOME` が返ります。

| command | 構文 | 必須引数 | 機能 |
| --- | --- | --- | --- |
| `PASS` | `PASS <password>` | password | server 接続 password を送る |
| `NICK` | `NICK <nickname>` | nickname | IRC 上で見える名前、宛先名を設定する |
| `USER` | `USER <username> 0 * :<realname>` | username, realname | 登録用の user 情報を送る |

例:

```text
PASS pass
NICK bob
USER bob 0 * :Bob
```

成功例:

```text
:ft_irc 001 bob :Welcome to ft_irc
```

注意:

| command | よくあるエラー | 意味 |
| --- | --- | --- |
| `PASS` | `461 ERR_NEEDMOREPARAMS` | password がない |
| `PASS` | `464 ERR_PASSWDMISMATCH` | password が違う |
| `PASS` / `USER` | `462 ERR_ALREADYREGISTRED` | 登録後に再登録しようとした |
| `NICK` | `431 ERR_NONICKNAMEGIVEN` | nick がない |
| `NICK` | `433 ERR_NICKNAMEINUSE` | その nick はすでに使われている |
| `USER` | `461 ERR_NEEDMOREPARAMS` | 引数が足りない |

### 接続補助コマンド

irssi などの client が自動で送ることがあります。

| command | 構文 | 必須引数 | 機能 |
| --- | --- | --- | --- |
| `CAP` | `CAP <subcommand> [args]` | subcommand | client capability の問い合わせ。ft_irc は空の capability を返す |
| `PING` | `PING :<token>` | token | 接続確認。server は `PONG` を返す |
| `PONG` | `PONG :<token>` | token | PING への返答。ft_irc では受け取っても特に処理しない |

例:

```text
CAP LS
PING :12345
```

成功例:

```text
:ft_irc CAP * LS :
:ft_irc PONG ft_irc :12345
```

### reference client 補助コマンド

mandatory の中心ではありませんが、irssi や WeeChat などの reference client が
自然に動くように、ft_irc は `NAMES`, `WHO`, `QUIT` に最小互換で対応しています。

| command | 構文 | 必須引数 | 機能 |
| --- | --- | --- | --- |
| `NAMES` | `NAMES [channel]` | なし | channel の nick 一覧を返す |
| `WHO` | `WHO [target]` | なし | WHO query の終了応答を返す |
| `QUIT` | `QUIT [:message]` | なし | 接続を閉じ、共有 channel の user に通知する |

`NAMES #test` の成功例:

```text
:ft_irc 353 bob = #test :alice @bob
:ft_irc 366 bob #test :End of /NAMES list
```

`WHO #test` の成功例:

```text
:ft_irc 315 bob #test :End of /WHO list
```

`QUIT :bye` の成功例:

```text
:bob QUIT :bye
```

`WHO` は本格的な `352 RPL_WHOREPLY` までは返さず、client が待ち続けたり
`421 Unknown command` になったりしないための最小応答です。

### JOIN

channel に参加します。
channel が存在しない場合は、この JOIN によって新しく作られます。
最初に入った user は channel operator になります。

| 項目 | 内容 |
| --- | --- |
| 構文 | `JOIN <channel> [key]` |
| 必須引数 | channel |
| 省略可能 | key |
| 成功時の送信先 | channel 全員。本人も含む |

例:

```text
JOIN #test
JOIN #secret secretkey
```

成功例:

```text
:bob JOIN #test
:ft_irc 331 bob #test :No topic is set
:ft_irc 353 bob = #test :@bob
:ft_irc 366 bob #test :End of /NAMES list
```

よくあるエラー:

| numeric | 意味 |
| --- | --- |
| `461 ERR_NEEDMOREPARAMS` | channel が指定されていない |
| `403 ERR_NOSUCHCHANNEL` | channel 名が不正。例: `JOIN test` |
| `471 ERR_CHANNELISFULL` | `+l` の人数上限に達している |
| `473 ERR_INVITEONLYCHAN` | `+i` で招待されていない |
| `475 ERR_BADCHANNELKEY` | `+k` の key がない、または違う |

### PART

channel から退出します。

| 項目 | 内容 |
| --- | --- |
| 構文 | `PART <channel> [:message]` |
| 必須引数 | channel |
| 省略可能 | message |
| 成功時の送信先 | channel 全員。本人も含む |

例:

```text
PART #test
PART #test :bye
```

成功例:

```text
:bob PART #test :bye
```

よくあるエラー:

| numeric | 意味 |
| --- | --- |
| `461 ERR_NEEDMOREPARAMS` | channel が指定されていない |
| `403 ERR_NOSUCHCHANNEL` | channel が存在しない |
| `442 ERR_NOTONCHANNEL` | 自分がその channel にいない |

### PRIVMSG

メッセージを送ります。
送信先は channel または nick です。

| 項目 | 内容 |
| --- | --- |
| 構文 | `PRIVMSG <target> :<message>` |
| 必須引数 | target, message |
| target | `#channel` または `nick` |
| channel 宛て | channel 全員。ただし送信者本人は除く |
| nick 宛て | 指定 nick の user 1人 |

例:

```text
PRIVMSG #test :hello channel
PRIVMSG bob :hello bob
```

成功例:

```text
:alice PRIVMSG #test :hello channel
:alice PRIVMSG bob :hello bob
```

よくあるエラー:

| numeric | 意味 |
| --- | --- |
| `461 ERR_NEEDMOREPARAMS` | target または message が足りない |
| `401 ERR_NOSUCHNICK` | nick 宛てで、その nick が存在しない |
| `404 ERR_CANNOTSENDTOCHAN` | channel に送れない。例: channel に入っていない |

RFC 的には、target がない場合 `411 ERR_NORECIPIENT`、本文が空の場合
`412 ERR_NOTEXTTOSEND` もあります。ただし現在の ft_irc では mandatory 範囲を優先し、
主に `461` / `401` / `404` を使っています。

### KICK

channel から user を追い出します。
実行者は channel operator である必要があります。

| 項目 | 内容 |
| --- | --- |
| 構文 | `KICK <channel> <nickname> [:reason]` |
| 必須引数 | channel, nickname |
| 省略可能 | reason |
| 成功時の送信先 | channel 全員 |

例:

```text
KICK #test alice :bye
```

成功例:

```text
:bob KICK #test alice
```

よくあるエラー:

| numeric | 意味 |
| --- | --- |
| `461 ERR_NEEDMOREPARAMS` | channel または nickname が足りない |
| `403 ERR_NOSUCHCHANNEL` | channel が存在しない |
| `442 ERR_NOTONCHANNEL` | 自分がその channel にいない |
| `441 ERR_USERNOTINCHANNEL` | target user がその channel にいない |
| `482 ERR_CHANOPRIVSNEEDED` | 自分が operator ではない |

### INVITE

user を channel に招待します。
特に `+i` invite-only channel に入ってもらうために使います。
実行者は channel operator である必要があります。

| 項目 | 内容 |
| --- | --- |
| 構文 | `INVITE <nickname> <channel>` |
| 必須引数 | nickname, channel |
| 成功時の送信先 | 招待された user |
| 成功時の numeric | 実行者に `341 RPL_INVITING` |

例:

```text
INVITE alice #test
```

成功例:

```text
:bob INVITE alice #test
:ft_irc 341 bob #test alice
```

よくあるエラー:

| numeric | 意味 |
| --- | --- |
| `461 ERR_NEEDMOREPARAMS` | nick または channel が足りない |
| `442 ERR_NOTONCHANNEL` | 自分がその channel にいない |
| `443 ERR_USERONCHANNEL` | target user はすでに channel にいる |
| `401 ERR_NOSUCHNICK` | target nick が存在しない |
| `482 ERR_CHANOPRIVSNEEDED` | 自分が operator ではない |

### TOPIC

channel の topic を見たり、設定したりします。

| 項目 | 内容 |
| --- | --- |
| topic 確認 | `TOPIC <channel>` |
| topic 設定 | `TOPIC <channel> :<topic>` |
| 必須引数 | channel |
| topic 設定時の送信先 | channel 全員 |

例:

```text
TOPIC #test
TOPIC #test :new topic
```

成功例:

```text
:ft_irc 331 bob #test :No topic is set
:ft_irc 332 bob #test :new topic
:bob TOPIC #test :new topic
```

よくあるエラー:

| numeric | 意味 |
| --- | --- |
| `461 ERR_NEEDMOREPARAMS` | channel が指定されていない |
| `442 ERR_NOTONCHANNEL` | 自分がその channel にいない |
| `482 ERR_CHANOPRIVSNEEDED` | `+t` の channel で、自分が operator ではない |

### MODE

channel の状態を見たり、変更したりします。
mode 変更は channel operator だけができます。

| 項目 | 内容 |
| --- | --- |
| mode 確認 | `MODE <channel>` |
| mode 変更 | `MODE <channel> <mode> [argument]` |
| 必須引数 | channel。変更時は mode も必須 |
| 成功時の送信先 | channel 全員 |

対応 mode:

| mode | 引数 | 意味 |
| --- | --- | --- |
| `+i` / `-i` | なし | invite-only を設定、解除 |
| `+t` / `-t` | なし | topic 変更を operator のみに制限、解除 |
| `+k` | key | channel key を設定 |
| `-k` | 省略可 | channel key を解除 |
| `+l` | number | channel 人数上限を設定 |
| `-l` | なし | 人数上限を解除 |
| `+o` / `-o` | nick | user に operator 権限を付与、削除 |

例:

```text
MODE #test
MODE #test +i
MODE #test -i
MODE #test +k secret
MODE #test -k
MODE #test +l 2
MODE #test -l
MODE #test +o alice
MODE #test -o alice
```

成功例:

```text
:ft_irc 324 bob #test +it
:bob MODE #test +i
:bob MODE #test +o alice
```

よくあるエラー:

| numeric | 意味 |
| --- | --- |
| `461 ERR_NEEDMOREPARAMS` | mode や必要な argument が足りない |
| `403 ERR_NOSUCHCHANNEL` | channel が存在しない |
| `442 ERR_NOTONCHANNEL` | 自分がその channel にいない |
| `472 ERR_UNKNOWNMODE` | 未対応の mode |
| `441 ERR_USERNOTINCHANNEL` | `+o` / `-o` 対象 nick が channel にいない |
| `482 ERR_CHANOPRIVSNEEDED` | 自分が operator ではない |

現在の ft_irc の `MODE` は、基本的に `+i`, `-i`, `+k secret` のように
1回の command で1つの mode を扱う想定です。
`MODE #test +it` のような複数 mode 同時指定は、本物の IRC ではよくありますが、
この実装では主対象ではありません。

## 送信範囲のイメージ

IRC command は「誰に送られるか」が重要です。

| command | 誰に送るか | 例 |
| --- | --- | --- |
| `JOIN` | channel 全員。本人も含む | `:bob JOIN #test` |
| `PART` | channel 全員。本人も含む | `:bob PART #test :bye` |
| `KICK` | channel 全員 | `:bob KICK #test alice` |
| `TOPIC` | channel 全員 | `:bob TOPIC #test :new topic` |
| `MODE` | channel 全員 | `:bob MODE #test +i` |
| `INVITE` | 招待された user | `:bob INVITE alice #test` |
| `PRIVMSG #channel` | channel 全員。ただし本人は除く | `:alice PRIVMSG #test :hello` |
| `PRIVMSG nick` | 指定した user 1人 | `:alice PRIVMSG bob :hello` |

そのため server 側には大きく2種類の送信処理があります。

| 関数 | 役割 |
| --- | --- |
| `broadcastToChannel(channel, message, excludeFd)` | channel の全 member に送る。`excludeFd` の fd は除外する |
| `sendToNick(nick, message)` | nick から fd を探して、その user 1人へ送る |

`excludeFd = -1` は「除外なし」という意味です。
`PRIVMSG #test :hello` では送信者本人には返さないので、送信者の `getFd()` を渡します。

## operator と channel mode

最初に channel を作った user は operator になります。

```text
/join #test
```

`@bob` のように表示されていれば operator です。

operator だけが実行できる操作:

| command | 意味 |
| --- | --- |
| `KICK` | user を channel から追い出す |
| `INVITE` | invite-only channel に user を招待する |
| `MODE` | channel mode を変更する |
| `TOPIC` | `+t` のとき topic を変更する |

対応している mode:

| mode | 意味 | 例 |
| --- | --- | --- |
| `+i` | invite-only。招待された user だけ join できる | `/mode #test +i` |
| `-i` | invite-only を解除 | `/mode #test -i` |
| `+t` | topic 変更を operator のみに制限 | `/mode #test +t` |
| `-t` | topic 変更制限を解除 | `/mode #test -t` |
| `+k key` | channel key を設定。join に key が必要 | `/mode #test +k secret` |
| `-k` | channel key を解除 | `/mode #test -k` |
| `+l n` | channel 人数上限を設定 | `/mode #test +l 2` |
| `-l` | 人数上限を解除 | `/mode #test -l` |
| `+o nick` | user に operator 権限を付与 | `/mode #test +o alice` |
| `-o nick` | user から operator 権限を削除 | `/mode #test -o alice` |

### invite-only の確認例

operator 側:

```text
/mode #test +i
/invite alice #test
```

alice 側:

```text
/join #test
```

招待前に join すると `Cannot join channel (+i)` になります。
招待後なら join できます。

### channel key の確認例

operator 側:

```text
/mode #test +k secret
```

別 user 側:

```text
/join #test
```

これは失敗します。

```text
/join #test secret
```

これは成功します。

## よくある詰まり方

### `Unknown option: password`

irssi の `/connect` に `-password` はありません。

NG:

```text
/connect -password pass -nick bob 127.0.0.1 6667
```

OK:

```text
/set nick bob
/connect 127.0.0.1 6667 pass
```

または shell から:

```bash
irssi -c localhost -p 6667 -w pass -n bob
```

### `bob: No such channel`

`bob` を channel 名として join しようとしている可能性があります。

channel は `#` から始めます。

```text
/join #test
```

`bob` は nick、`#test` は channel です。

### 外部サーバーにつながっている

次のように表示されている場合、自分たちの `ft_irc` ではなく外部サーバーです。

```text
Connecting to dh.ircnet.ne.jp [125.6.255.10] port 6660
```

自分たちのサーバーを見るなら:

```text
/connect 127.0.0.1 6667 pass
```

外部サーバーを見るのも勉強になりますが、ft_irc のテストとは分けて考えます。

### `Welcome to ft_irc` が出ない

確認すること:

| 確認 | 例 |
| --- | --- |
| server が起動しているか | `./ircserv 6667 pass` |
| port が同じか | server も client も `6667` |
| password が同じか | server の `pass` と client の `pass` |
| nick が重複していないか | 既に `bob` がいるなら別 nick にする |

### irssi が裏で知らない command を送る

irssi は `CAP`, `WHO`, `MODE` などを自動で送ることがあります。
`CAP`, `WHO`, `MODE` は対応済みですが、現在の ft_irc が対応していない command には
`421 Unknown command` が返ることがあります。
手動確認では、まず `/join`, `/msg`, `/mode`, `/topic`, `/kick`, `/invite` を見れば十分です。

## 外部 IRC サーバーを見るときの観察ポイント

外部サーバーに接続すると、本物の IRC サーバーがどんな reply を返すか確認できます。

見るとよいもの:

| 観察対象 | 見る理由 |
| --- | --- |
| 接続直後の numeric | `001`, `002`, `003`, `004`, `005` などが流れる |
| `JOIN` 後の reply | `JOIN`, `332`, `353`, `366` の流れを確認できる |
| `MODE #channel` | `324 RPL_CHANNELMODEIS` の形が分かる |
| 存在しない nick/channel | `401`, `403` などの error が見える |
| `/topic`, `/invite`, `/kick` | operator 権限や channel state の感覚が分かる |

外部サーバーは DNS/ident/接続制限などがあるため、ft_irc より接続処理が長いことがあります。
ft_irc ではそこまで再現する必要はありません。

## raw IRC と irssi 表示の対応

irssi の操作は、裏では raw IRC command に変換されています。

| irssi | raw IRC |
| --- | --- |
| `/join #test` | `JOIN #test` |
| `/msg #test hello` | `PRIVMSG #test :hello` |
| `/msg bob hello` | `PRIVMSG bob :hello` |
| `/topic #test new topic` | `TOPIC #test :new topic` |
| `/mode #test +i` | `MODE #test +i` |
| `/kick #test alice bye` | `KICK #test alice :bye` |
| `/invite alice #test` | `INVITE alice #test` |

server から client へ届く message は、だいたい次の形です。

```text
:<sender> <COMMAND> <target> :<message>
```

例:

```text
:alice PRIVMSG #test :hello
```

numeric reply は次の形です。

```text
:ft_irc <code> <nick> <args> :<message>
```

例:

```text
:ft_irc 001 bob :Welcome to ft_irc
:ft_irc 331 bob #test :No topic is set
:ft_irc 366 bob #test :End of /NAMES list
```

## 最小確認シナリオ

提出前やレビュー前は、最低限これを通すと安心です。

1. server 起動

```bash
./ircserv 6667 pass
```

2. bob 接続

```bash
irssi -c localhost -p 6667 -w pass -n bob
```

```text
/join #test
```

3. alice 接続

```bash
irssi -c localhost -p 6667 -w pass -n alice
```

```text
/join #test
/msg #test hello
```

4. operator command 確認

bob 側:

```text
/topic #test first topic
/mode #test +i
/invite alice #test
/mode #test +k secret
/mode #test -k
/mode #test +l 3
/mode #test -l
```

5. direct message 確認

```text
/msg bob direct hello
```

6. 切断確認

```text
/part #test bye
/disconnect
```

ここまで動けば、手動確認としてはかなり良い状態です。
