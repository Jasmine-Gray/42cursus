# 入場動画

方式: 組み込み画像生成で入口の背景素材を生成し、Remotionで前進・霧・暗転を合成。動画生成APIは使用していない。

- 元素材: `_tools/media/public/entrance.webp`
- 動画: `_docs/mock/public/cinematics/entrance.mp4` / `frontend/public/cinematics/entrance.mp4`
- 静止画: 同じディレクトリの `entrance-poster.jpg`
- 合成: `_tools/media/src/entrance.tsx`
- 出力: 1920×1080、30fps、90フレーム。最後の12フレームは完全な黒。

カメラは入口中央の消失点へ進み、柱と扉が画面外へ抜ける。数字は映像に焼き込まず、ゲームの期限に合わせてUI側で表示する。

再出力:

```sh
cd _tools/media
REMOTION_BROWSER_EXECUTABLE="/Applications/Google Chrome.app/Contents/MacOS/Google Chrome" npm run render:entrance
```

生成プロンプト（参照: `_docs/mock/public/scenes/victorian-alley.png`）:

```text
Use case: historical-scene.
Asset type: high-resolution cinematic background plate for a first-person dolly-through-a-door shot in a Victorian mystery game, landscape 16:9.
Input image: reference ONLY for photographic art direction, masonry, gaslight and night exposure.
Primary request: camera is now facing directly into a wide OPEN dark entrance in one of these London houses, from outside about three meters away at human eye height. A worn black oak door has already swung fully inward against the right interior wall. A substantial stone door surround, soot-dark brick facade, a single restrained warm gas lantern to the left, damp stone threshold and modest foreground cobbles. The doorway opens into a long UNLIT corridor, extremely dark charcoal fading to almost pure black at its center. Recognizable dimensional reveal on both sides, low wooden interior panelling just barely visible, no distant lamp or window. We will physically advance the camera straight through this doorway and end in darkness.
Composition: absolutely frontal centered doorway, rectilinear architectural perspective, verticals straight, dark clear open passage centered horizontally at x=50%, eye-level vanishing point y=46%. Open aperture covers approximately x=34% to 66%, from y=18% to 85%. Door frame surrounded by textured facade visible on both sides, full doorway visible, no steps blocking the walking path. Natural lens, not ultrawide. Dim practical light gently rakes the stone and wood, retain texture in surroundings but the interior is genuinely dark.
Style: grounded premium game cinematic environment, photographic realism, subtle atmospheric haze, tactile aged stone, worn brass and real wood, low saturation warm charcoal and amber matching reference, serious quiet mystery.
Constraints: no people, no text, no house numbers, no logos, no UI, no bright opening, no fantasy ornament, no floating particles, no symmetrical twin lanterns, no exaggerated glowing fog. Single finished landscape scene.
```

