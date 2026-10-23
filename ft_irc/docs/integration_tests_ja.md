# ft_irc 統合テストガイド

このドキュメントは、`tests/scripts/irc_integration_test.py` と
`tests/Makefile` で実行する統合テストの読み方をまとめたものです。

## まず実行するコマンド

通常の mandatory 統合テストは、リポジトリルートで次を実行します。

```bash
make
```

`make` は Docker 上の Ubuntu 環境を使い、`ircserv` をコンテナ内で
ビルドしてから mandatory profile の全ケースを実行します。

ビルドだけしたい場合は次を使います。

```bash
make build
```

提出用の再ビルドは従来どおり次で実行できます。

```bash
make re
```

## よく使うテストコマンド

```bash
make
make -f tests/Makefile list
make -f tests/Makefile test-1
make -f tests/Makefile test-4
make -f tests/Makefile test-9
make -f tests/Makefile test-10
make -f tests/Makefile test-11
make -f tests/Makefile transport-smoke
make -f tests/Makefile test PROFILE=rfc
make -f tests/Makefile strict-test-6
```

| コマンド | 意味 |
| --- | --- |
| `make` | mandatory profile の全ケースを実行する |
| `make build` | ローカルで `ircserv` だけをビルドする |
| `make re` | clean してから `ircserv` を再ビルドする |
| `make -f tests/Makefile list` | ケース一覧とチェック内容を表示する |
| `make -f tests/Makefile test-N` | `test-N` だけを実行する |
| `make -f tests/Makefile transport-smoke` | protocol 未統合の transport 基本動作だけを確認する |
| `make -f tests/Makefile test PROFILE=rfc` | より厳密な RFC/error reply 寄りのチェックを実行する |
| `make -f tests/Makefile strict-test-6` | `test-6` を Valgrind 付きで実行する |

## profile の違い

| profile | 目的 |
| --- | --- |
| `mandatory` | 42 subject の必須範囲として見たい基本動作を確認する |
| `rfc` | numeric error、permission error、parser edge case などをより厳しく確認する |

`mandatory` が通っていて `rfc` が落ちる場合は、基本要件は満たしているが
厳密な IRC 挙動では差分がある、という読み方をします。

## exit status の読み方

| 状態 | exit status | 意味 |
| --- | ---: | --- |
| 全ケース pass | `0` | テスト成功 |
| 1件以上 fail | `1` | テスト失敗 |
| `test-1` 内の不正起動 | `ircserv` 側は non-zero | 不正引数なので non-zero が期待値 |

例:

```text
[SUMMARY] 11/11 passed, 0 failed
```

この場合、runner の exit status は `0` です。

```text
[SUMMARY] 6/11 passed, 5 failed
```

この場合、runner の exit status は `1` です。

`test-1` は `./ircserv` を不正引数で起動します。ここでは `ircserv` が
`exit=1` など non-zero で終わること自体が OK です。

## test ケース一覧

| test | 内容 | 見ていること |
| --- | --- | --- |
| `test-1` | 起動引数チェック | 引数不足、不正 port、範囲外 port で即 non-zero exit するか |
| `test-2` | 登録と non-blocking | idle client がいても別 client が `PASS/NICK/USER` で登録でき、`001` を受け取るか |
| `test-3` | fragmented registration | `PASS/NICK/USER` が TCP 分割されても、正しく結合して登録できるか |
| `test-4` | 空行・空白入力 | 空行や空白だけの入力で壊れず、その後も登録できるか |
| `test-5` | parser edge | trailing text の空白が保持されるか |
| `test-6` | disconnect/HUP | 通常 close、abortive close、途中コマンド close 後も server が生きているか |
| `test-7` | transport hardening | 接続バースト、fd 再利用、再接続で transport が壊れないか |
| `test-8` | command burst / buffering | command 一括送信、CRLF 分割、大きい未完了入力で壊れないか |
| `test-9` | JOIN / PRIVMSG | channel join、channel message、direct message が届き、第三者に漏れないか |
| `test-10` | 2 client roundtrip | Alice/Bob が channel と direct message を双方向に送受信できるか |
| `test-11` | operator / mode | `KICK`, `INVITE`, `TOPIC`, `MODE +i/+t/+k/+l/+o` の基本動作と `+l` の join block |

## transport 単体チェック

`transport-1` / `transport-2` は、`001` や `JOIN` reply などの protocol 実装を
要求しません。socket accept、raw input、close、reconnect、bind failure を
transport 単体で確認するためのケースです。

standalone の transport 実装を試す場合は、binary を直接指定します。

```bash
python3 tests/scripts/irc_integration_test.py --binary /tmp/transport_candidate \
  --case transport-1
python3 tests/scripts/irc_integration_test.py --binary /tmp/transport_candidate \
  --case transport-1 --case transport-2
```

## ログの読み方

### client が送ったもの

```text
[    cmd]: client-B(active): $ 'PASS pass\r\n'
```

これは shell コマンドではありません。意味は、`client-B(active)` の socket に
`PASS pass\r\n` を送った、というテストログ上の擬似コマンドです。

raw bytes の場合は次のように出ます。

```text
[    cmd]: client-A(frag12345): $ <raw-bytes>
[ output]:
           <input>
             PA
```

これは `PASS` の先頭 `PA` だけを先に送って、TCP 分割入力をテストしている
ことを表します。

### client が受け取ったもの

```text
[ output]:
           <match>
             numeric 001
           <client-A(blank12345) received>
             :server 001 blank12345 :Welcome ...
```

`<match>` は期待値として見つかった意味です。`numeric 001` は登録成功 reply
を見つけたことを表します。

`<client-A(... ) received>` は、その client socket が実際に受け取った内容です。

### server が出したもの

```text
[ output]:
           <server stdout+stderr>
             Server listening on port 43369$
             [Client 4]: PASS pass$
```

`<server stdout+stderr>` は、server process の標準出力と標準エラーをまとめた
ものです。client 側の受信内容とは別物として読みます。

### `^M$` の意味

ログは `cat -e` 風に表示します。

| 表示 | 意味 |
| --- | --- |
| `^M` | `\r` |
| `$` | 行末 |
| `^M$` | IRC の `\r\n` 行末 |

## pass/fail の見方

各 test は最後に次のような summary を出します。

```text
[SUMMARY] 1/1 passed, 0 failed
```

full suite では次のように全体件数が出ます。

```text
[SUMMARY] 11/11 passed, 0 failed
```

`[ result]: [OK] ...` はその操作ステップの期待値が満たされたことを表します。
`[ result]: [KO] ...` はその時点で期待値が満たされなかったことを表します。

失敗した test でも `commands:` block が出ます。どの `case-XX` のどの操作で
`[KO]` になったかを先に確認し、必要なら後続の `details:` tail で server log を
確認します。
