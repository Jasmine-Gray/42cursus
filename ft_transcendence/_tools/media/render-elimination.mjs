// =============================================================================
//  _tools/media/render-elimination.mjs
// -----------------------------------------------------------------------------
//  Created : 2026-09-12  yayoi111 (d/character-ui-cleanup) 退場映像の書き出し
// -----------------------------------------------------------------------------
//  Updated : 2026-09-12  yayoi111 (d/character-ui-cleanup) 透過霧に差替え
//  関連 : _tools/media/src/elimination.tsx
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
const publicDir = resolve(directory, "out/assets");
await mkdir(publicDir, { recursive: true });
for (const [source, destination] of [
  ["_docs/mock/public/scenes/victorian-alley.png", "alley.png"],
  ["_docs/mock/public/scenes/mist.webp", "fog.webp"],
  ["_tools/media/public/sabre.png", "sabre.png"],
])
  await copyFile(resolve(repository, source), resolve(publicDir, destination));

const browserExecutable = process.env.REMOTION_BROWSER_EXECUTABLE || undefined;
const serveUrl = await bundle({
  entryPoint: resolve(directory, "src/index.ts"),
  publicDir,
});
const composition = await selectComposition({
  serveUrl,
  id: "Elimination",
  browserExecutable,
});
const outputLocation = resolve(directory, "out/elimination.mp4");
await renderMedia({
  composition,
  serveUrl,
  outputLocation,
  codec: "h264",
  crf: 18,
  pixelFormat: "yuv420p",
  browserExecutable,
  concurrency: 3,
});
await renderStill({
  composition,
  serveUrl,
  output: resolve(directory, "out/elimination-poster.jpg"),
  frame: 36,
  imageFormat: "jpeg",
  browserExecutable,
});
for (const target of [
  "frontend/public/cinematics",
  "_docs/mock/public/cinematics",
]) {
  await mkdir(resolve(repository, target), { recursive: true });
  await copyFile(
    outputLocation,
    resolve(repository, target, "elimination.mp4"),
  );
  await copyFile(
    resolve(directory, "out/elimination-poster.jpg"),
    resolve(repository, target, "elimination-poster.jpg"),
  );
}
console.log(
  "Rendered and installed: elimination.mp4 (1920×1080, 30 fps, 7.2 s)",
);
