// =============================================================================
//  _tools/media/render-outcomes.mjs
// -----------------------------------------------------------------------------
//  Created : 2026-09-12  yayoi111 (d/character-ui-cleanup) 勝敗の合成映像出力
// -----------------------------------------------------------------------------
//  関連 : _tools/media/src/outcome.tsx
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
const publicDir = resolve(directory, "out/outcome-assets");
await mkdir(publicDir, { recursive: true });
for (const [source, destination] of [
  ["_docs/mock/public/cinematics/victory-alley.webp", "victory-alley.webp"],
  ["_docs/mock/public/cinematics/defeat-alley.webp", "defeat-alley.webp"],
  ["_docs/mock/public/scenes/mist.webp", "mist.webp"],
])
  await copyFile(resolve(repository, source), resolve(publicDir, destination));
const browserExecutable = process.env.REMOTION_BROWSER_EXECUTABLE || undefined;
const serveUrl = await bundle({
  entryPoint: resolve(directory, "src/outcome-index.ts"),
  publicDir,
});
for (const id of ["Victory", "Defeat"]) {
  const composition = await selectComposition({
    serveUrl,
    id,
    browserExecutable,
  });
  const name = id.toLowerCase();
  await renderMedia({
    composition,
    serveUrl,
    outputLocation: resolve(directory, `out/${name}.mp4`),
    codec: "h264",
    crf: 19,
    pixelFormat: "yuv420p",
    browserExecutable,
    concurrency: 3,
  });
  await renderStill({
    composition,
    serveUrl,
    output: resolve(directory, `out/${name}-still.jpg`),
    frame: 209,
    imageFormat: "jpeg",
    browserExecutable,
  });
  await renderStill({
    composition,
    serveUrl,
    output: resolve(directory, `out/${name}-middle.jpg`),
    frame: 80,
    imageFormat: "jpeg",
    browserExecutable,
  });
  for (const target of [
    "frontend/public/cinematics",
    "_docs/mock/public/cinematics",
  ]) {
    for (const suffix of [".mp4", "-still.jpg"])
      await copyFile(
        resolve(directory, `out/${name}${suffix}`),
        resolve(repository, target, `${name}${suffix}`),
      );
  }
  console.log(`Rendered ${id}: 1920×1080, 30 fps, 7 seconds.`);
}
