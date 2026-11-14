# Mystery Word UI Mock

[`../`](../README.md)（docs）の課題要件をもとにした、スマートフォン向けワードウルフの画面モックです。PC で開いた場合も中央に最大幅 `448px` のモバイル画面として表示されます。

参加者へ似た2つのお題を配り、議論後に全員が少数派だと思う人物へ投票するワードウルフです。

## 起動

```bash
npm install
npm run dev
```

起動後は `/mocks` を開くと、すべての画面へ個別に移動できます。ルート `/` はログイン画面です。

## 画面モック

- 認証: `/login`, `/register`
- ゲーム開始前: `/home`, `/rooms`（招待URL検索）, `/join/CASE-042`（共有リンク）, `/rooms/new`, `/lobby`
- お題確認: `/game/reveal/citizen`, `/game/reveal/wolf`
- 役割別の議論: `/game/citizen`, `/game/wolf`, `/game/spectator`
- 観戦中の投票: `/game/spectator/vote`
- ゲーム進行: `/game/paused`, `/game/vote`, `/game/result`
- ユーザー: `/friends`, `/profile`
- 必須ページ: `/privacy`, `/terms`
- 共有用一覧: `/mocks`

ログインや投票はバックエンドへ送信しないモック動作です。市民とワードウルフの画面は、結果発表まで自分が多数派・少数派のどちらか分からない仕様にしています。

## UI 構成

- React 19 + TypeScript + Vite
- Tailwind CSS
- shadcn/ui（Base UI）を `src/components/ui` に保持
- ゲーム固有部品を `src/components/game-ui` に集約
- Wouter による画面単位の URL
- React Hook Form + Zod による認証フォーム検証
- 生成した6種類のゲームキャラクターを `public/avatars` に配置
- アバター選択と、複数のお題ジャンル・お題の近さを設定できる部屋作成

## 品質チェック

```bash
npm run lint
npm test
npm run build
npm audit
```

現在はフロントエンドのみの画面モックです。API やリアルタイム通信への接続は含みません。
