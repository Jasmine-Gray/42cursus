import { useEffect, useState, type CSSProperties } from 'react'

const frameCount = 16
const crops = { near: { x: 406, y: 169, width: 68, height: 91 }, middle: { x: 1198, y: 403, width: 24, height: 38 }, far: { x: 1005, y: 474, width: 18, height: 29 } }
type Lamp = keyof typeof crops
const strips = new Map<Lamp, Promise<string>>()

// Sample the original photograph, then produce discrete raster frames.
// The displacement is zero at the crop boundary and lamp burner.
function createFlameStrip(lamp: Lamp) {
  const cached = strips.get(lamp)
  if (cached) return cached
  const crop = crops[lamp]
  const promise = (async () => {
    const source = new Image()
    source.src = '/scenes/victorian-alley.png'
    await source.decode()
    const patch = document.createElement('canvas')
    patch.width = crop.width
    patch.height = crop.height
    const context = patch.getContext('2d')
    if (!context) throw new Error('Flame frame canvas unavailable')
    context.drawImage(
      source,
      (crop.x * source.naturalWidth) / 1672,
      (crop.y * source.naturalHeight) / 941,
      (crop.width * source.naturalWidth) / 1672,
      (crop.height * source.naturalHeight) / 941,
      0,
      0,
      crop.width,
      crop.height,
    )
    const original = context.getImageData(0, 0, crop.width, crop.height)
    const atlas = document.createElement('canvas')
    atlas.width = crop.width * frameCount
    atlas.height = crop.height
    const atlasContext = atlas.getContext('2d')
    if (!atlasContext) throw new Error('Flame strip canvas unavailable')
    for (let frame = 0; frame < frameCount; frame++) {
      const output = context.createImageData(crop.width, crop.height)
      const phase = (frame / frameCount) * Math.PI * 2
      for (let y = 0; y < crop.height; y++) {
        for (let x = 0; x < crop.width; x++) {
          const nx = (x - crop.width / 2) / (crop.width * 0.426)
          const ny = (y - crop.height * 0.484) / (crop.height * 0.462)
          const weight = Math.pow(Math.max(0, 1 - nx * nx - ny * ny), 2)
          const tip = (crop.height - y) / crop.height
          const dx =
            (Math.sin(phase + y / crop.height * 5.9) * (crop.width * 0.103) +
              Math.sin(phase * 2 - y / crop.height * 3.64) * (crop.width * 0.029)) *
            weight *
            tip
          const dy = Math.cos(phase + x / crop.width * 3.74) * (crop.height * 0.055) * weight
          const sx = Math.max(0, Math.min(crop.width - 1, x + dx))
          const sy = Math.max(0, Math.min(crop.height - 1, y + dy))
          const x0 = Math.floor(sx),
            y0 = Math.floor(sy)
          const x1 = Math.min(x0 + 1, crop.width - 1),
            y1 = Math.min(y0 + 1, crop.height - 1)
          const fx = sx - x0,
            fy = sy - y0
          for (let channel = 0; channel < 4; channel++) {
            const a = original.data[(y0 * crop.width + x0) * 4 + channel]
            const b = original.data[(y0 * crop.width + x1) * 4 + channel]
            const c = original.data[(y1 * crop.width + x0) * 4 + channel]
            const d = original.data[(y1 * crop.width + x1) * 4 + channel]
            output.data[(y * crop.width + x) * 4 + channel] =
              (a * (1 - fx) + b * fx) * (1 - fy) + (c * (1 - fx) + d * fx) * fy
          }
        }
      }
      atlasContext.putImageData(output, frame * crop.width, 0)
    }
    return atlas.toDataURL('image/png')
  })()
  strips.set(lamp, promise)
  return promise
}

export function FlameFrames({ lamp = 'near' }: { lamp?: Lamp }) {
  const crop = crops[lamp]
  const [strip, setStrip] = useState<string>()
  useEffect(() => {
    let disposed = false
    void createFlameStrip(lamp)
      .then((value) => {
        if (!disposed) setStrip(value)
      })
      .catch(() => {
        strips.delete(lamp)
      })
    return () => {
      disposed = true
    }
  }, [lamp])
  if (!strip) return null
  return (
    <svg
      x={crop.x}
      y={crop.y}
      width={crop.width}
      height={crop.height}
      viewBox={`0 0 ${crop.width} ${crop.height}`}
      overflow="hidden"
      data-flame-frames={frameCount}
      data-lamp={lamp}
    >
      <image
        className="flame-frame-strip"
        href={strip}
        width={crop.width * frameCount}
        height={crop.height}
        style={
          {
            '--flame-strip-end': `${-crop.width * frameCount}px`,
          } as CSSProperties
        }
      />
    </svg>
  )
}
