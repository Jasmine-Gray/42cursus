// =============================================================================
//  _tools/media/src/outcome.tsx
// -----------------------------------------------------------------------------
//  Created : 2026-09-12  yayoi111 (d/character-ui-cleanup) 勝敗の空間合成
// -----------------------------------------------------------------------------
//  役割 : 一台のカメラ、奥行きごとの霧、光源に沿う光を同じ時間軸で合成
//  関連 : _tools/media/render-outcomes.mjs
// =============================================================================

import {
  AbsoluteFill,
  Composition,
  Img,
  staticFile,
  useCurrentFrame,
} from "remotion";

function progress(frame: number, start: number, end: number) {
  const t = Math.max(0, Math.min(1, (frame - start) / (end - start)));
  return t * t * (3 - 2 * t);
}

export function Outcome({ won }: { won: boolean }) {
  const frame = useCurrentFrame();
  const reveal = progress(frame, 0, 48);
  const travel = progress(frame, 0, 200);
  const light = progress(frame, 20, 112);
  const settle = progress(frame, 105, 200);
  const arc = Math.sin(travel * Math.PI);
  const cameraScale = won ? 1.085 - travel * 0.045 : 1.04 + travel * 0.045;

  return (
    <AbsoluteFill style={{ background: "#040607", overflow: "hidden" }}>
      <AbsoluteFill
        style={{
          opacity: reveal,
          scale: cameraScale,
          translate: `${arc * (won ? -14 : 12)}px ${arc * (won ? 8 : -8)}px`,
          rotate: `${arc * (won ? -0.35 : 0.3)}deg`,
          transformOrigin: "54% 60%",
        }}
      >
        <Img
          src={staticFile(`${won ? "victory" : "defeat"}-alley.webp`)}
          style={{ width: "100%", height: "100%", objectFit: "cover" }}
        />
        {/* Light shares the skyline's source. Its softness is baked into the film. */}
        {won && (
          <svg
            viewBox="0 0 1920 1080"
            style={{
              position: "absolute",
              inset: 0,
              width: "100%",
              height: "100%",
              opacity: light * (0.28 - settle * 0.1),
              mixBlendMode: "screen",
            }}
          >
            <defs>
              <filter id="light-softness">
                <feGaussianBlur stdDeviation="28" />
              </filter>
              <linearGradient id="daylight" x1="0" y1="0" x2="0.2" y2="1">
                <stop stopColor="#deffed" stopOpacity=".8" />
                <stop offset="1" stopColor="#9dd1b7" stopOpacity="0" />
              </linearGradient>
            </defs>
            <g
              filter="url(#light-softness)"
              fill="url(#daylight)"
              style={{
                rotate: `${-2 + light * 3}deg`,
                transformOrigin: "1450px 0px",
              }}
            >
              <path d="M1320 -100 1490 -100 1080 1010 360 1010Z" />
              <path d="M1500 -100 1580 -100 1580 800 1250 850Z" opacity=".6" />
            </g>
          </svg>
        )}
        {/* The distant bank stays within the street; close banks pass the lens. */}
        {[0, 1, 2].map((depth) => {
          const side = depth === 1 ? -1 : 1;
          const depthScale = [0.62, 1.2, 1.6][depth];
          const angle = travel * (won ? 1.25 : -1) + depth * 1.8;
          const x =
            depth === 0
              ? Math.sin(angle) * 26
              : side * (won ? 160 + travel * 180 : 290 - travel * 160) +
                Math.sin(angle) * 70;
          const y =
            depth === 0
              ? 140
              : 200 + Math.cos(angle) * 45 + (won ? travel * 55 : -travel * 50);
          const opacity =
            depth === 0
              ? 0.15
              : won
                ? 0.27 - light * 0.19
                : 0.08 + light * 0.19;
          return (
            <Img
              key={depth}
              src={staticFile("mist.webp")}
              style={{
                position: "absolute",
                inset: 0,
                width: "100%",
                height: "100%",
                objectFit: "contain",
                transformOrigin: "54% 63%",
                scale: depthScale * (1 + travel * (won ? 0.24 : 0.14)),
                translate: `${x}px ${y}px`,
                rotate: `${side * (Math.sin(angle) * 3 + depth)}deg`,
                opacity: opacity * reveal,
                filter: won
                  ? "sepia(.2) hue-rotate(60deg) brightness(.9)"
                  : "sepia(1) saturate(2.8) hue-rotate(308deg) brightness(.36)",
                maskImage:
                  depth === 0
                    ? "radial-gradient(ellipse 26% 30% at 54% 62%, #000, transparent)"
                    : "linear-gradient(transparent 20%, #000 60%)",
              }}
            />
          );
        })}
        <AbsoluteFill
          style={{
            background: won
              ? "radial-gradient(ellipse at 54% 48%, transparent 30%, #06130eb3 100%)"
              : "radial-gradient(ellipse at 54% 48%, transparent 22%, #1a0107e6 100%)",
            opacity: 0.2 + light * (won ? 0.2 : 0.5),
          }}
        />
      </AbsoluteFill>
    </AbsoluteFill>
  );
}

export function OutcomeRoot() {
  return (
    <>
      <Composition
        id="Victory"
        component={Outcome}
        defaultProps={{ won: true }}
        width={1920}
        height={1080}
        fps={30}
        durationInFrames={210}
      />
      <Composition
        id="Defeat"
        component={Outcome}
        defaultProps={{ won: false }}
        width={1920}
        height={1080}
        fps={30}
        durationInFrames={210}
      />
    </>
  );
}
