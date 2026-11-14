import { useEffect, useRef, useState } from "react";
import { useReducedMotion } from "motion/react";
import { PlayerAvatar } from "@/components/game-ui/player-avatar";
import type { Player } from "@/data/mock-data";

type EliminationFilmProps = { player: Player; onComplete: () => void };

export function EliminationFilm({ player, onComplete }: EliminationFilmProps) {
  const video = useRef<HTMLVideoElement>(null);
  const finished = useRef(false);
  const [failed, setFailed] = useState(false);
  const reducedMotion = useReducedMotion();
  const showStill = reducedMotion || failed;

  function complete() {
    if (finished.current) return;
    finished.current = true;
    onComplete();
  }

  useEffect(() => {
    let cancelled = false;
    const current = video.current;
    // A media failure or reduced-motion setting gets a quiet, bounded still.
    const timeout = window.setTimeout(
      () => {
        if (finished.current) return;
        finished.current = true;
        onComplete();
      },
      showStill ? 2200 : 16000,
    );
    if (!showStill && current) {
      const playback = current.play();
      playback?.catch(() => {
        if (!cancelled) setFailed(true);
      });
    }
    return () => {
      cancelled = true;
      window.clearTimeout(timeout);
      current?.pause();
    };
  }, [onComplete, showStill]);

  return (
    <section className="elimination-film" aria-label="退場の演出">
      {showStill ? (
        <div className="elimination-still">
          <PlayerAvatar
            presentation="portrait"
            src={player.avatarSrc}
            name={player.name}
            initials={player.initials}
          />
          <p>{player.name}</p>
          <span>退場</span>
        </div>
      ) : (
        <video
          ref={video}
          className="elimination-video"
          src="/cinematics/elimination.mp4"
          poster="/cinematics/elimination-poster.jpg"
          muted
          playsInline
          preload="auto"
          onEnded={complete}
          onError={() => setFailed(true)}
          aria-hidden="true"
        />
      )}
      <p className="sr-only" role="status">
        {player.name}が退場します。
      </p>
    </section>
  );
}
