# 勝敗の画面演出用背景

2026-09-12、組み込みの `image_gen` で既存のロンドン背景を編集。
建物・石畳・ガス灯・カメラの構図を維持し、勝利は青緑と白の光、敗北は深紅と黒の霧に変更。
元画像は残し、生成画像を WebP に変換して使用する。

- 編集元: `_docs/mock/public/scenes/victorian-alley.png`
- 正本: `_docs/mock/public/cinematics/victory-alley.webp`
- 正本: `_docs/mock/public/cinematics/defeat-alley.webp`
- FE: `frontend/public/cinematics/victory-alley.webp`
- FE: `frontend/public/cinematics/defeat-alley.webp`
- 演出: `_docs/mock/src/components/game-ui/result-cinematic.tsx`
- スタイル: `_docs/mock/src/styles/results.css`

背景は退場動画が終わってから7秒の合成映像として再生し、4秒目から勝敗の文字を重ねる。
映像は共通のカメラと奥行きごとの透過霧、光源に沿う光をRemotionで合成する。
フレーム209の静止画を、動きを減らす設定・スキップ・動画失敗時の表示に使用する。
動画の失敗や再生停止で「結果を見る」への進行は止まらない。
動画を再マウントせず保持するため、文字の表示時にカメラや色が初期状態へ戻らない。
動きを減らす設定では静止した勝敗色を表示する。
「結果を見る」の操作で最終結果に進み、その画面で初めて陣営を公開する。
既存の丸・ダイヤ・粒子と専用キーフレームは削除。
API・勝敗判定・陣営公開のタイミングは変更しない。

## 合成映像の再生成

```sh
cd _tools/media
npm run render:outcomes
npm run render:elimination
```

`REMOTION_BROWSER_EXECUTABLE` でChromeの実行ファイルを指定できる。
勝敗映像は `frontend/public/cinematics/` と `_docs/mock/public/cinematics/` の
`victory.mp4` / `defeat.mp4`、静止画は `victory-still.jpg` / `defeat-still.jpg`。
合成元は `_tools/media/src/outcome.tsx`、書き出しは `_tools/media/render-outcomes.mjs`。

以前の霧素材には看板と街灯が写り込んでいたため、物体を含まない透過霧に差し替えた。
保存先は `_docs/mock/public/scenes/mist.webp` と `frontend/public/scenes/mist.webp`。
共通背景と退場映像もこの素材を使用する。

## Victory — 生成プロンプト

Use case: lighting-weather. Edit this existing Victorian London alley image for a full-screen VICTORY game cinematic. Preserve the exact architecture, cobblestone street, left foreground gas lantern, clock tower and camera composition. Change only the cinematic lighting and atmosphere. Let majestic luminous sea-glass green and soft ivory light break through the fog from the upper sides, lighting the edges of the entire frame and reflecting softly on the wet cobblestones. The gas lamps retain small antique amber flames. Charcoal green shadows, expansive pale mint shafts in mist, a quiet central area of darker atmospheric street for live UI text. Serious, triumphant, premium realistic Victorian game cinematography, rich photographic material detail. This is a game result background, NOT a UI mockup: absolutely no words, no letters, no icons, no geometry symbols, no circle, no diamond, no frame or border, no interface, no people, no confetti, no trophy, no modern neon signs, no magical portals. Widescreen 16:9, fill the entire image, refined restrained volumetric atmosphere, not generic flat recoloring.

## Defeat — 生成プロンプト

Use case: lighting-weather. Edit this existing Victorian London alley image for a full-screen DEFEAT game cinematic. Preserve the exact architecture, cobblestone street, left foreground gas lantern, clock tower and camera composition. Change only the cinematic lighting and atmosphere. Heavy ominous charcoal-black fog gathers from the sides and from above, with deep crimson and restrained scarlet light bleeding through the far edges of the entire frame and across the wet cobblestones. Gas lamps are dim, exhausted amber. The central street remains dark and quiet for live UI text, the left and right peripheral areas are visibly deep red. Defeat, finality, a city swallowed by night. Premium realistic Victorian game cinematography, physically detailed dark brick, natural irregular smoke folds, deliberately strong red-versus-black art direction. This is a game result background, NOT a UI mockup: absolutely no words, no letters, no symbols, no circle, no diamond, no frame or border, no interface, no people, no bodies, no blood, no gore, no skull, no modern neon signs, no explosions. Widescreen 16:9, fill the entire image, not generic flat recoloring.

## 透過霧 — 生成プロンプト

Use case: stylized-concept. Asset type: transparent isolated volumetric mist plate for compositing into a realistic Victorian London game cinematic. Wide canvas 16:9. ONLY natural low-lying pale neutral grey fog and delicate translucent vapor. An asymmetrical, soft-edged long bank of drifting mist along the lower half, with thin curls folding upward at the far sides, open empty transparent space in the center upper half. Photographic practical smoke captured in a dark studio with soft back light; large smooth turbulent folds with fine wispy detail, subtle natural variation in density; wisps fade entirely to alpha around EVERY edge. Genuine transparent alpha background. All fog is semi-transparent; the wisps must allow the composited street underneath to show. This is a VFX texture plate, no scene, no objects, no lanterns, no signs, no text, no symbols, no buildings, no ground, no rectangular background, no checkerboard, no circular vortex, no perfect repeating swirls, no grey wallpaper. The fog should be sparse and organic, not a uniform cloudy sheet. Neutral color so lighting can be tinted during compositing.
