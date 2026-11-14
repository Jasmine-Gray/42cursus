# ゲームUIのAPI接続メモ

2026-09-12。ゲーム進行のAPI接続を担当する人へのメモ。
個別の投票先の公開は未実装。キャラクター画像の保存は下記のとおり実装済み。

## 契約変更が必要：結果確定後の全員の投票先

現在の `_docs/spec/api/openapi.yaml` の `GET /games/{gameId}/result` (`getGameResult`) は、自分の投票先 `myVote` と全員の得票数のみ返す。`GameResult.players` は「個別の投票先は公開しない」と明記されている。`_docs/spec/api/asyncapi.yaml` の `result.published` も同じ非公開仕様。

今回のUI要件は、結果確定後に全員の「投票した人 → 投票先」を表示すること。新規エンドポイントではなく、両契約の結果ペイロードへ投票一覧を追加する案が適する。

提案フィールド：`votes: [{ userId, targetUserId }]`。これは未確定の提案で、生成型への手書き追加はしない。未投票者は配列に含めず、UIでは参加者と照合して「未投票」を表示。各投票者は最大1件。公開はRESULT以降に限定し、`vote.progress` は人数のみのままにする。結果取得と結果イベントで同じ内容を返し、再接続時も復元できるようにする。

契約更新 → spec-lint → gen → spec-docs → BEの結果取得・配信変更 → FE接続・契約検証の順に進める。

## 既存契約で対応できるもの

| UI | 使用する既存情報 |
| --- | --- |
| 自分の勝敗・勝利／敗北演出 | `GameResult.winner` と本人の `players[].role` |
| 市民／人狼の紋章 | `CITIZEN` / `WORD_WOLF` を表示名・アイコンへ変換 |
| 最多票の人物と票数 | `players[].votesReceived`。同数最多・投票0件では一人を選ばない |
| お題・テーマ | 本人用情報の `word` / `category` |
| 準備状態 | `room.ready` 送信と `room.state.players[].ready` |
| 砂時計・秒表示 | フェーズ情報の `deadline` と `paused` / `reconnectDeadline` |
| カードめくり・開票・選出・退場 | FE内の演出。追加API不要 |
| 勝敗画面で「結果を見る」を待つ | FEの表示状態。サーバーの進行や再戦期限は停止しない |

## 現在のモックとの差し替え箇所

- `frontend/src/mocks/data/mock-data.ts` の `revealedVotes` は固定の6票。`voteResults` はこの配列から集計して表示の整合性を保っている。
- `frontend/src/mocks/pages/game-pages.tsx` の勝敗判定は固定の単独最多ケース用。実接続ではサーバーの `winner` / `reason` を正とし、同票・未投票・観戦者を分岐する。
- 同ファイルの `mock-discussion-deadline` はローカルの5分締切。実接続ではサーバーのフェーズ締切に置き換え、お題確認画面に入った時刻で勝手に開始しない。
- 本人識別は固定fixtureの `isCurrentUser` ではなく、認証中のユーザーIDと照合する。
- 投票は既存の `POST /games/{gameId}/votes` (`castVote`) に接続する。演出中に投票先を変更しない。

## 結果の表示順

`ResultCinematic` は参加者を横に並べ、投票した人のカードを投票先へ順に積む。
公開済みの票だけで最多票を計算し、同票の場合は全員の列を光らせる。
その後、選ばれた人物、共通の退場動画、
勝利／敗北それぞれの演出、`VICTORY` / `DEFEAT` の順に進む。
「結果を見る」を押すまで最終結果へ移動しない。
役職名・陣営の紋章・人狼のお題は最終結果で初めて描画する。
演出のスキップでも勝敗表示で止まり、結果の確認を待つ。
退場動画は再生終了を待つ。再生失敗・動きを減らす設定では静止表示へ切り替え、
読み込み停止にも待機上限を設ける。APIの進行タイマーは変更しない。

これは表示順の制御であり、実接続時の結果APIに含まれる情報をブラウザの
開発ツールから隠す仕組みではない。サーバーは投票完了前に役職を公開しないこと。

## キャラクター画像の保存（実装済み）

既存の `PUT /users/me/avatar` (`updateAvatar`) のmultipartアップロードを使う。
PNG / JPEG / WebP、5 MiB以下、幅・高さはそれぞれ1〜4096 px。
縦横比は自由で、正方形への切り抜きや背景合成はせず、元の画像と透過を保存する。
種類と寸法はバックエンドで画像ヘッダーから判定し、申告されたMIMEだけには依存しない。
`avatarUrl` の形式は変わらないため、DBマイグレーションと追加フィールドは不要。

アップロード先は `backend/priv/static/uploads/avatars/`。
Composeの `avatar_uploads` ボリュームで永続化し、CaddyとViteは `/uploads/*` を
バックエンドへ転送する。通常の再起動・再ビルドで画像は残る。

`PlayerAvatar` は小さい顔アイコンと `presentation="portrait"` の立ち絵を分ける。
立ち絵は `object-fit: contain` で輪郭全体を表示する。
14種類のプリセットは既存IDを維持して更新済み。
正本モックのアップロード保存はローカルストレージ、フロントエンドの設定画面は実APIを使う。
