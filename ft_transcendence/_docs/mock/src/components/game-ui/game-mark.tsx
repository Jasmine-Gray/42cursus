type GameMarkProps = { inverse?: boolean }

export function GameMark({ inverse = false }: GameMarkProps) {
  return <span className="wolf-mark" data-inverse={inverse} role="img" aria-label="狼の紋章" />
}
