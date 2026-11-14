// =============================================================================
//  _tools/media/render-entrance.mjs
// -----------------------------------------------------------------------------
//  Created : 2026-09-13  yayoi111 (d/character-ui-cleanup) 入場動画・ポスターの出力
//  Authors : 2026-09-12  yayoi111 (d/character-ui-cleanup)
//            render-outcomes.mjs の動画・静止画出力を再利用
// -----------------------------------------------------------------------------
//  関連 : _tools/media/render-entrance.mjs, _tools/media/src/entrance.tsx
// =============================================================================

import { bundle } from "@remotion/bundler";
import {
  renderMedia,
  renderStill,
  selectComposition,
} from "@remotion/renderer";
import { copyFile, mkdir } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const directory = dirname(fileURLToPath(import.meta.url));
const repository = resolve(directory, "../..");
const publicDir = resolve(directory, "out/entrance-assets");
await mkdir(publicDir, { recursive: true });
await copyFile(
  resolve(directory, "public/entrance.webp"),
  resolve(publicDir, "entrance.webp"),
);
await copyFile(
  resolve(repository, "_docs/mock/public/scenes/mist.webp"),
  resolve(publicDir, "mist.webp"),
);
const browserExecutable = process.env.REMOTION_BROWSER_EXECUTABLE || undefined;
const serveUrl = await bundle({
  entryPoint: resolve(directory, "src/entrance-index.ts"),
  publicDir,
});
const composition = await selectComposition({
  serveUrl,
  id: "Entrance",
  browserExecutable,
});
await renderMedia({
  composition,
  serveUrl,
  outputLocation: resolve(directory, "out/entrance.mp4"),
  codec: "h264",
  crf: 19,
  pixelFormat: "yuv420p",
  browserExecutable,
  concurrency: 3,
});
for (const frame of [0, 30, 60, 78]) {
  await renderStill({
    composition,
    serveUrl,
    output: resolve(directory, `out/entrance-${frame}.jpg`),
    frame,
    imageFormat: "jpeg",
    browserExecutable,
  });
}
for (const target of [
  "frontend/public/cinematics",
  "_docs/mock/public/cinematics",
]) {
  await copyFile(
    resolve(directory, "out/entrance.mp4"),
    resolve(repository, target, "entrance.mp4"),
  );
  await copyFile(
    resolve(directory, "out/entrance-0.jpg"),
    resolve(repository, target, "entrance-poster.jpg"),
  );
}
console.log(
  "Rendered Entrance: 1920×1080, 30 fps, 3 seconds, final 400 ms black.",
);
