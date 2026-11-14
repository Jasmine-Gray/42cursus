# ゲームUIのCSS

共通CSSの正本は `_docs/mock/src/index.css` と `_docs/mock/src/styles/`。
編集後に `make ui-styles` を実行すると `frontend/src/` に同期される。
`make ui-styles-check` は差分だけを検証し、ファイルを変更しない。

フロントエンドは自身のディレクトリ内のCSSを読み込むため、Dockerの
`frontend/` ビルドコンテキストでも外部ファイルを参照せずに動く。
FE側のCSSを直接編集せず、正本と同期結果を一緒にコミットする。

デザインの基準は `/home` のレイアウトと色合い（2026-09-12、ユーザー確認）。
ヴィクトリア朝の背景、落ち着いた真鍮色、プレイヤーカードとPLAYメニューの構成を踏襲する。
勝敗の全画面演出に限り、勝利は青緑と白、敗北は深紅と黒へ切り替える。

## 各ファイルの担当

| ファイル | 担当 |
| --- | --- |
| `foundation.css` | Tailwindトークン、色、フォント、ベース要素 |
| `shell.css` | 画面全体、ヘッダー、ナビゲーション、スクロール領域 |
| `scene.css` | ヴィクトリア朝の背景、霧、炎、前進する街路 |
| `controls.css` | 共通ボタン、入力、タグ、カードの基本スタイル |
| `characters.css` | 顔アイコン、透過立ち絵、キャラクター選択、参加者カード |
| `home.css` | ホームのプレイヤーカードとPLAYメニュー |
| `rooms.css` | 部屋一覧・作成・招待・ロビー |
| `auth.css` | ログイン、登録、規約 |
| `social.css` | プロフィール、フレンド |
| `game.css` | 入場、カウントダウン、お題、議論、タイマー、チャット |
| `results.css` | 開票、選出、退場、勝敗演出、最終結果 |
| `motion.css` | 共通で参照するkeyframes |
| `accessibility.css` | 動きを減らす設定での演出抑制 |

`index.css` はライブラリと上記モジュールの読み込みだけを担当する。
インポート順はカスケードの一部なので、変更時は画面を確認する。
画面固有の変更は担当モジュールへ置き、末尾に全画面向けの上書きを追加しない。
既存の同じセレクターがあれば、その定義を編集する。

## 部屋メニューの共通構成

`/rooms`・`/join/:code`・`/rooms/new` は `RoomMenuLayout` を共用する。
メニュー、入力欄、部屋情報、決定操作の位置はコンポーネント側で固定する。
メニューはホームの `PlayMenu` を共用し、PCでは2つを横並びにする。
メニュー・決定・補助操作はホームと同じ高さ64px、文字左・矢印右上。
暗いプレートと深紅の決定色は `controls.css` の共通定義を使う。
決定操作の幅は192px、コピーは入力欄と同じ高さ48px。
エラーとコピー通知の領域は確保し、内容の変更でボタンを押し下げない。
画面配置は `rooms.css`、ボタンの見た目は `controls.css` に置く。
ページ側で高さ・色・幅を上書きしない。

- 正本: `_docs/mock/src/components/game-ui/room-menu-layout.tsx`
- FE: `frontend/src/components/game-ui/room-menu-layout.tsx`
- データ・検証・遷移: 両側の `pages/home-pages.tsx`（FEは `src/mocks/`）

## 開始カウントダウン

ガス灯のある玄関へ進み、敷居を越えて暗転する3秒のMP4を全画面で再生する。
3・2・1はBarlow Condensedの小さな数字を下側に置き、入口を隠さない。
最後の約400msは数字も隠れる完全な黒。その後、お題画面（観戦者は議論画面）へ進む。
旧街路レイヤー・カメラ移動・霧のブラウザー合成は削除し、映像にまとめた。

ゲームの開始時刻は従来の `endsAt` が正本。動画の終了や読み込みで延長しない。
ロビーで先読みし、読み込みが遅れたときは現在時刻に合わせてシークする。
タブを隠すと再生停止し、復帰時には期限と再生位置を確認する。
再生失敗・動きを減らす設定では入口の静止画に数字を重ね、最後に暗転する。
動画の制作方法と画像生成プロンプトは [entrance-art.md](entrance-art.md)。

## 検証

```sh
make ui-styles-check
cd frontend && npm run build && npm test
```

モックは `_docs/mock/` で `npm run build` / `npm run dev`。
今回の分割では1366×768の15画面で、分割前後のcomputed styleと画面を比較した。
動きの確認は通常設定と `prefers-reduced-motion: reduce` の両方で行う。
開発資料用の `_docs/dev/style.css` とアーカイブはゲームUIの同期対象外。
