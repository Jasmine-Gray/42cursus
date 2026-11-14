# Mock screenshots

> 最終更新: 2026-08-16

Vite React モックを 390 × 844 px のモバイル表示で撮影した確認用画像です。PCでもアプリ本体は同じモバイル幅で表示されます。
各画面に対応するAPIは [_docs/spec/api/](../../_docs/spec/api/README.md) の対応表を参照してください。

## 認証

| 01 ログイン | 02 新規登録 | 03 アバター設定 |
| --- | --- | --- |
| <img src="01-login.jpg" width="220" alt="ログイン"> | <img src="02-register.jpg" width="220" alt="新規登録"> | <img src="03-avatar-setup.jpg" width="220" alt="アバター設定"> |

## ゲーム開始前

| 04 ホーム | 05 招待URL検索 | 06 招待確認 |
| --- | --- | --- |
| <img src="04-home.jpg" width="220" alt="ホーム"> | <img src="05-room-search.jpg" width="220" alt="招待URL検索"> | <img src="06-room-invite.jpg" width="220" alt="招待確認"> |

| 07 部屋作成 | 08 ロビー（ホスト） | 09 ロビー（参加者） |
| --- | --- | --- |
| <img src="07-room-create.jpg" width="220" alt="部屋作成"> | <img src="08-lobby-host.jpg" width="220" alt="ホストのロビー"> | <img src="09-lobby-player-wolf.jpg" width="220" alt="参加者のロビー"> |

| 10 ロビー（観戦者） | | |
| --- | --- | --- |
| <img src="10-lobby-spectator.jpg" width="220" alt="観戦者のロビー"> | | |

## ゲーム中

| 11 開始カウントダウン | 12 市民のお題確認 | 13 ウルフのお題確認 |
| --- | --- | --- |
| <img src="11-game-countdown.jpg" width="220" alt="開始カウントダウン"> | <img src="12-word-reveal-citizen.jpg" width="220" alt="市民のお題確認"> | <img src="13-word-reveal-wolf.jpg" width="220" alt="ワードウルフのお題確認"> |

| 14 議論（市民） | 15 議論（ウルフ） | 16 観戦画面 |
| --- | --- | --- |
| <img src="14-discussion-citizen.jpg" width="220" alt="市民の議論"> | <img src="15-discussion-wolf.jpg" width="220" alt="ワードウルフの議論"> | <img src="16-discussion-spectator.jpg" width="220" alt="観戦画面"> |

| 17 観戦者向け投票 | 18 一時停止 | 19 投票 |
| --- | --- | --- |
| <img src="17-spectator-vote.jpg" width="220" alt="観戦者向け投票"> | <img src="18-game-paused.jpg" width="220" alt="一時停止"> | <img src="19-game-vote.jpg" width="220" alt="投票"> |

| 20 結果発表 | | |
| --- | --- | --- |
| <img src="20-game-result.jpg" width="220" alt="結果発表"> | | |

## ユーザーと必須ページ

| 21 フレンド | 22 プロフィール・設定 | 23 プライバシーポリシー |
| --- | --- | --- |
| <img src="21-friends.jpg" width="220" alt="フレンド"> | <img src="22-profile.jpg" width="220" alt="プロフィール・設定"> | <img src="23-privacy.jpg" width="220" alt="プライバシーポリシー"> |

| 24 利用規約 | 25 モック一覧 | |
| --- | --- | --- |
| <img src="24-terms.jpg" width="220" alt="利用規約"> | <img src="25-mock-catalog.jpg" width="220" alt="モック一覧"> | |

## 再生成

```sh
npm run dev -- --host 127.0.0.1 --port 5175
MOCK_BASE_URL=http://127.0.0.1:5175 npm run screenshots
```

JPEG 品質 84 で保存し、共有時のファイルサイズを抑えています。
