# ゲームUIの書体選定（2026-09-08）

## 採用

- Barlow Condensed 600/700：英字のSTART、VICTORY、DEFEAT、タイマー。狭い横幅と太い線で視線を集める。タイマーはtabular-numsと表示幅を確保。
- しっぽり明朝600：選出告知と「結果は…」の大きな日本語のみ。本文や入力欄に使わない。
- Noto Sans JP Variable：チャット400–500、プレイヤー名・操作500–600、日本語見出し600–700。OSによる日本語書体の違いを抑える。

Rajdhaniは角張ったSF的な印象が街並みと離れるため置き換え。全体を明朝にすると設定や会話が読みにくくなるため、演出文に限定した。採用はこのプロジェクトに対するデザイン判断であり、特定ゲームと同一の書体であるという主張ではない。

Fontsourceでローカル配信し、Google Fontsへの実行時通信は行わない。Barlowは使用するLatin600/700、しっぽり明朝は600のみ読み込む。日本語はunicode-rangeで必要なファイルのみ取得する。

参照：
- https://github.com/jpt/barlow
- https://github.com/fontdasu/ShipporiMincho
- https://github.com/notofonts/noto-cjk
