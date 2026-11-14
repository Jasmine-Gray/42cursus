# ワードウルフの画像・動画制作ツール

探偵テーマのタイトル動画、背景、告知画像などを制作するための開発環境。
Remotion とアプリで必要な Zod のメジャーバージョンが異なるため、
`frontend/` とは別の npm パッケージとして管理する。

## 導入したもの

| ツール | 用途 |
| --- | --- |
| Remotion / CLI 4.0.520 | React で動画を制作し、Studio でプレビュー、MP4・静止画を書き出す |
| @remotion/transitions 4.0.520 | 動画のフェードやスライドなどの場面転換 |
| Sharp 0.35.4 | Node.js で画像のリサイズ、合成、WebP・AVIF への変換 |
| React / React DOM 19.2.8、TypeScript 6.0.2 | 動画の制作基盤 |
| Zod 4.4.3 | Remotion の対応バージョン |
| Remotion公式AIスキル | このリポジトリの動画制作をAIに依頼するための手順と参考資料 |

ゲーム画面のアニメーションには、導入済みの `frontend/` の `motion` を使う。
動画を書き出すときは Remotion のフレームに基づくアニメーションを使う。
Sharp は制作時に Node.js で利用し、ブラウザーのコードには import しない。

## 再インストール・確認

```sh
cd _tools/media
npm ci
npm run check
```

この環境は任意の制作ツールなので、ゲームの `make` 起動には含めていない。
初めて動画を書き出す端末では、以下で描画用ブラウザーを用意できる。

```sh
npm run remotion -- browser ensure
```

## 退場映像

`src/elimination.tsx` の `Elimination` composition が、斬撃から暗転までの
1920×1080 / 30 fps / 約7.2秒の映像。ゲームに表示する文字や役職は含めない。
刃の素材は `public/sabre.png`。既存の街路・霧と合成し、RemotionでMP4にする。

```sh
npm run render:elimination
```

書き出しは `out/` に保存し、採用するMP4とポスターを
`frontend/public/cinematics/` と `_docs/mock/public/cinematics/` へコピーする。
ブラウザーの場所を指定する場合は `REMOTION_BROWSER_EXECUTABLE` を使う。

```sh
npm run remotion -- studio src/index.ts --no-open
```

ゲーム内ではミュートで再生し、動画の `ended` で次の演出へ進む。
再生に失敗した場合や動きを減らす設定では、静止した退場表示へ切り替える。

### 画像素材の作り方

`public/sabre.png` は組み込みの `image_gen` で2026-09-12に生成。
[プロンプト](elimination-art.md)。透過を保ってコピーし、外部の有料動画APIは使っていない。

## 参照元

- [Remotion の既存プロジェクトへの導入](https://www.remotion.dev/docs/brownfield)
- [Remotion公式AIスキル](https://www.remotion.dev/docs/ai/skills)
- [スキル配布元](https://github.com/remotion-dev/skills)
- [Sharp の導入](https://sharp.pixelplumbing.com/install/)
- [Motion の導入](https://motion.dev/docs/react-installation)
