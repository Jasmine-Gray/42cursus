// =============================================================================
//  _tools/media/src/elimination.tsx
// -----------------------------------------------------------------------------
//  Created : 2026-09-12  yayoi111 (d/character-ui-cleanup) 一人称の退場映像
// -----------------------------------------------------------------------------
//  Updated : 2026-09-12  yayoi111 (d/character-ui-cleanup) 背景物のない透過霧
//  役割 : 接近・刃の予備動作・斬撃・視点の落下・暗転をフレームで制御
//  関連 : _tools/media/render-elimination.mjs
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
const WIDTH = 1920;
const HEIGHT = 1080;
const FPS = 30;
const FRAMES = 216;

function ease(frame: number, from: number, to: number) {
  const x = Math.max(0, Math.min(1, (frame - from) / (to - from)));
  return x * x * (3 - 2 * x);
}

// Fixed samples ensure the exact same particles at every render and frame seek.
function noise(seed: number) {
  const value = Math.sin(seed * 127.1 + 311.7) * 43758.5453;
  return value - Math.floor(value);
}

export function Elimination() {
  const frame = useCurrentFrame();
  const impact = 78;
  const fallen = ease(frame, impact + 7, 124);
  const jolt = frame < impact ? 0 : Math.exp(-(frame - impact) / 9);
  const shakeX = Math.sin(frame * 2.3) * jolt * 27;
  const shakeY = Math.cos(frame * 1.7) * jolt * 17;
  const cameraRoll = -fallen * 24 + jolt * Math.sin(frame) * 1.9;
  const reveal = ease(frame, 0, 22);
  const vignette = interpolate(
    frame,
    [0, 58, 85, 155, 215],
    [0.7, 0.44, 0.65, 0.82, 1],
    clamp,
  );
  const attackOpacity = interpolate(
    frame,
    [28, 39, 72, 86],
    [0, 1, 1, 0],
    clamp,
  );
  const attackRotation = interpolate(
    frame,
    [28, 58, 68, 73, 80, 87],
    [24, 7, 13, -2, -75, -112],
    clamp,
  );
  const attackX = interpolate(
    frame,
    [28, 58, 68, 73, 80, 87],
    [790, 340, 400, 160, -490, -1120],
    clamp,
  );
  const attackY = interpolate(
    frame,
    [28, 58, 68, 73, 80, 87],
    [170, 90, 40, -60, -80, 150],
    clamp,
  );
  const attackScale = interpolate(
    frame,
    [28, 58, 69, 81, 87],
    [0.74, 1.03, 1.05, 1.9, 2.1],
    clamp,
  );
  const attackSpeed = frame > 72 && frame < 86 ? 1 : 0;

  return (
    <AbsoluteFill style={{ background: "#040607", overflow: "hidden" }}>
      <AbsoluteFill
        style={{
          opacity: reveal,
          scale: 1.16 + frame * 0.00065 + fallen * 0.32,
          rotate: `${cameraRoll}deg`,
          translate: `${shakeX}px ${shakeY - fallen * 245}px`,
          transformOrigin: "52% 68%",
        }}
      >
        <Img
          src={staticFile("alley.png")}
          style={{
            width: WIDTH,
            height: HEIGHT,
            objectFit: "cover",
            filter: `saturate(${0.54 - fallen * 0.28}) brightness(${0.72 - fallen * 0.16}) blur(${jolt * 3}px)`,
          }}
        />
        <AbsoluteFill
          style={{
            background:
              "linear-gradient(90deg, #03090b70, transparent 40%, #04050750)",
          }}
        />
        <AbsoluteFill
          style={{
            background:
              "radial-gradient(ellipse at 26% 35%, #c493452a, transparent 26%)",
            opacity: 0.78 + Math.sin(frame * 0.29) * 0.04,
          }}
        />
        {[0, 1, 2].map((layer) => (
          <Img
            key={layer}
            src={staticFile("fog.webp")}
            style={{
              position: "absolute",
              width: 2450,
              height: 1450,
              objectFit: "cover",
              left: -340 + Math.sin(frame / 160 + layer * 2) * 190,
              top: 20 + layer * 95 - fallen * 80,
              opacity: (0.085 + layer * 0.03) * (1 + fallen),
              mixBlendMode: "screen",
              scale: 1 + layer * 0.16,
            }}
          />
        ))}
      </AbsoluteFill>

      {/* One practical foreground blade, sampled several times for exposure blur. */}
      {[3, 2, 1, 0].map((sample) => (
        <Img
          key={sample}
          src={staticFile("sabre.png")}
          style={{
            position: "absolute",
            left: 100,
            top: 15,
            width: 1720,
            height: 1147,
            objectFit: "contain",
            opacity: attackOpacity * (sample === 0 ? 0.98 : attackSpeed * 0.13),
            translate: `${attackX + sample * attackSpeed * 85}px ${attackY + sample * attackSpeed * 18}px`,
            rotate: `${attackRotation + sample * attackSpeed * 8}deg`,
            scale: attackScale,
            transformOrigin: "82% 76%",
            filter: `brightness(${0.65 + ease(frame, 47, 68) * 0.43}) blur(${sample * attackSpeed * 4}px)`,
          }}
        />
      ))}

      {/* A single short exposure change on contact; no repeated flashing. */}
      <AbsoluteFill
        style={{
          background: "#e0d5bf",
          opacity: frame === impact ? 0.48 : frame === impact + 1 ? 0.12 : 0,
        }}
      />
      <AbsoluteFill
        style={{
          background: "#030405",
          opacity: frame >= impact + 2 && frame <= impact + 5 ? 0.95 : 0,
        }}
      />

      {/* Dust and rain pass the fallen camera; these are particles, not UI marks. */}
      {Array.from({ length: 48 }, (_, index) => {
        const start = impact + 3 + noise(index + 18) * 14;
        const age = Math.max(0, frame - start);
        const opacity = frame < start ? 0 : Math.max(0, 0.6 - age / 90);
        return (
          <div
            key={index}
            style={{
              position: "absolute",
              left: 960 + (noise(index + 3) - 0.5) * (260 + age * 32),
              top:
                510 + (noise(index + 5) - 0.5) * age * 12 + age * age * 0.075,
              width: 1 + noise(index + 7) * 4,
              height: 2 + noise(index + 11) * 7,
              borderRadius: "45%",
              background: index % 4 === 0 ? "#c1a36b" : "#6b7979",
              opacity,
              rotate: `${age * (noise(index) - 0.5) * 15}deg`,
              filter: index % 3 === 0 ? "blur(2px)" : undefined,
            }}
          />
        );
      })}

      <AbsoluteFill
        style={{
          background: `radial-gradient(ellipse at 50% ${48 + fallen * 12}%, transparent 12%, rgb(2 4 5 / ${vignette}) 83%)`,
        }}
      />
      <AbsoluteFill
        style={{
          opacity: 0.075,
          mixBlendMode: "soft-light",
          translate: `${(frame % 3) - 1}px ${(frame % 5) - 2}px`,
          backgroundImage: `url("data:image/svg+xml,%3Csvg viewBox='0 0 180 180' xmlns='http://www.w3.org/2000/svg'%3E%3Cfilter id='n'%3E%3CfeTurbulence type='fractalNoise' baseFrequency='.89' numOctaves='3' stitchTiles='stitch'/%3E%3C/filter%3E%3Cpath fill='%23ffffff' filter='url(%23n)' d='M0 0h180v180H0z'/%3E%3C/svg%3E")`,
        }}
      />
      <AbsoluteFill
        style={{ background: "#040607", opacity: ease(frame, 165, 211) }}
      />
      <div
        style={{
          position: "absolute",
          inset: "0 0 auto",
          height: 70,
          background: "#030405",
        }}
      />
      <div
        style={{
          position: "absolute",
          inset: "auto 0 0",
          height: 70,
          background: "#030405",
        }}
      />
    </AbsoluteFill>
  );
}

export function EliminationRoot() {
  return (
    <Composition
      id="Elimination"
      component={Elimination}
      width={WIDTH}
      height={HEIGHT}
      fps={FPS}
      durationInFrames={FRAMES}
    />
  );
}
