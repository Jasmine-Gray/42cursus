# frontend

React + TypeScript（Vite）。ディレクトリ所有・命名・分割の規約は [_docs/dev/conventions.html](../_docs/dev/conventions.html) を参照。

API仕様はここには書かない。契約から生成される [_docs/spec/api/](../_docs/spec/api/README.md) が正本で、`src/types/` は生成物（`make gen`・手編集禁止）。

画像・動画の制作環境（Remotion / Sharp）と使い方は [_tools/media/README.md](../_tools/media/README.md) を参照。画面のアニメーションには導入済みの `motion` を使う。

ビクトリア朝の背景とゲームUIは `/login` で確認できます。開始時は数字のカウントダウンだけを表示します。背景・ゲームUIの詳細は [デザインの制作メモ](../_docs/dev/visual-design.md) を参照。

未接続画面は `src/mocks/` に隔離した `_docs/mock/src` の移植を使用します。`App.tsx` の部屋・ロビー・ゲーム・フレンドのルートが対象です。ログイン・登録・ホーム・プロフィール・アバター設定の実API接続は維持しています。

モックのチャット、申請、投票などはローカルの状態で、サーバーへの保存・実際の通知は行いません。対戦相手・タイマー・結果はサンプルです。担当者がAPIを接続した際は、該当ルートを `src/pages/` の実装へ差し替えてください。開発用ログインと画面一覧への導線は撤去しました。
