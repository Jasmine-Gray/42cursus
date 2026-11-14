// =============================================================================
//  _tools/media/src/entrance.tsx
// -----------------------------------------------------------------------------
//  Created : 2026-09-13  yayoi111 (d/character-ui-cleanup) 玄関を通り抜けて暗転する開始映像
// -----------------------------------------------------------------------------
//  関連 : _tools/media/render-entrance.mjs, _tools/media/src/entrance.tsx
// =============================================================================

import {
  AbsoluteFill,
  Composition,
  Img,
  interpolate,
  staticFile,
  useCurrentFrame,
} from "remotion";

const clamp = { extrapolateLeft: "clamp", extrapolateRight: "clamp" } as const;

export function Entrance() {
  const frame = useCurrentFrame();
  const travel = Math.pow(Math.min(frame / 75, 1), 1.15);
  // Perspective magnification increases as the camera reaches the threshold.
  const scale = 1 / (1 - travel * 0.91);
  const step = Math.sin((frame / 30) * Math.PI * 2);
  const darkness = interpolate(frame, [61, 78], [0, 1], clamp);

  return (
    <AbsoluteFill style={{ background: "#000", overflow: "hidden" }}>
      <AbsoluteFill
        style={{
          scale,
          translate: `${step * 2}px ${Math.sin((frame / 30) * Math.PI * 4) * 1.2}px`,
          transformOrigin: "51% 46%",
        }}
      >
        <Img
          src={staticFile("entrance.webp")}
          style={{ width: "100%", height: "100%", objectFit: "cover" }}
        />
        <Img
          src={staticFile("mist.webp")}
          style={{
            position: "absolute",
            inset: 0,
            width: "100%",
            height: "100%",
            objectFit: "cover",
            translate: `${frame * 0.7 - 40}px ${80 - frame * 0.2}px`,
            opacity: interpolate(frame, [0, 45, 65], [0.07, 0.03, 0], clamp),
            maskImage: "linear-gradient(transparent 45%, #000)",
          }}
        />
      </AbsoluteFill>
      <AbsoluteFill style={{ background: "#000", opacity: darkness }} />
    </AbsoluteFill>
  );
}

export function EntranceRoot() {
  return (
    <Composition
      id="Entrance"
      component={Entrance}
      width={1920}
      height={1080}
      fps={30}
      durationInFrames={90}
    />
  );
}
