import { useCallback, useEffect, useRef, useState } from "react";
import { motion, useReducedMotion } from "motion/react";
import { PlayerAvatar } from "@/components/game-ui/player-avatar";
import { BallotStacks } from "@/components/game-ui/ballot-stacks";
import { EliminationFilm } from "@/components/game-ui/elimination-film";
import { Button } from "@/components/ui/button";
import type { Player, revealedVotes } from "@/data/mock-data";

type ResultCinematicProps = {
  players: Player[];
  votes: typeof revealedVotes;
  selectedPlayer: Player;
  voteCount: number;
  currentPlayer: Player;
  won: boolean;
  onComplete: () => void;
};
type Phase = "votes" | "selected" | "elimination" | "resolution" | "outcome";

function OutcomeAtmosphere({
  won,
  reducedMotion,
  entering,
}: {
  won: boolean;
  reducedMotion: boolean;
  entering: boolean;
}) {
  const videoRef = useRef<HTMLVideoElement>(null);
  const [playFilm] = useState(entering);
  const [failed, setFailed] = useState(false);
  const showFilm = playFilm && !reducedMotion && !failed;
  const name = won ? "victory" : "defeat";

  useEffect(() => {
    const video = videoRef.current;
    if (!showFilm || !video) return;
    let active = true;
    video.play().catch(() => {
      if (active) setFailed(true);
    });
    return () => {
      active = false;
      video.pause();
    };
  }, [showFilm]);

  return (
    <div className="outcome-atmosphere" aria-hidden="true">
      <img
        className="outcome-city"
        src={`/cinematics/${name}-still.jpg`}
        alt=""
      />
      {showFilm && (
        <video
          ref={videoRef}
          className="outcome-film"
          src={`/cinematics/${name}.mp4`}
          muted
          playsInline
          preload="auto"
          onError={() => setFailed(true)}
        />
      )}
      <div className="outcome-perimeter" />
      <motion.div
        className="outcome-shade"
        initial={reducedMotion || !playFilm ? false : { opacity: 0 }}
        animate={{ opacity: 1 }}
        transition={{ delay: 2.5, duration: 1.5, ease: "easeOut" }}
      />
    </div>
  );
}

export function ResultCinematic({
  players,
  votes,
  selectedPlayer,
  voteCount,
  currentPlayer,
  won,
  onComplete,
}: ResultCinematicProps) {
  const [phase, setPhase] = useState<Phase>("votes");
  const [visibleVotes, setVisibleVotes] = useState(0);
  const reducedMotion = useReducedMotion();
  const completeElimination = useCallback(
    () =>
      setPhase((current) =>
        current === "elimination" ? "resolution" : current,
      ),
    [],
  );

  useEffect(() => {
    if (phase !== "votes") return;
    const timers = votes.map((_, index) =>
      window.setTimeout(() => setVisibleVotes(index + 1), 850 + index * 1100),
    );
    return () => timers.forEach(window.clearTimeout);
  }, [phase, votes]);

  useEffect(() => {
    if (phase === "outcome" || phase === "elimination") return;
    const next = {
      votes: "selected",
      selected: "elimination",
      resolution: "outcome",
    } as const;
    const duration = {
      votes: 850 + votes.length * 1100 + 2000,
      selected: 3200,
      resolution: 4000,
    };
    const timeout = window.setTimeout(
      () => setPhase(next[phase]),
      duration[phase],
    );
    return () => window.clearTimeout(timeout);
  }, [phase, votes.length]);

  return (
    <main
      className="result-cinematic"
      data-phase={phase}
      data-outcome={
        phase === "resolution" || phase === "outcome"
          ? won
            ? "win"
            : "loss"
          : undefined
      }
      data-reduced-motion={reducedMotion || undefined}
    >
      <link
        rel="preload"
        as="image"
        href={`/cinematics/${won ? "victory" : "defeat"}-still.jpg`}
      />
      {(phase === "resolution" || phase === "outcome") && (
        <OutcomeAtmosphere
          won={won}
          reducedMotion={!!reducedMotion}
          entering={phase === "resolution"}
        />
      )}
      {phase === "votes" && (
        <section className="cinematic-votes">
          <p className="cinematic-eyebrow">THE BALLOTS</p>
          <h1>それぞれの選択</h1>
          <BallotStacks
            players={players}
            votes={votes.slice(0, visibleVotes)}
          />
        </section>
      )}
      {phase === "selected" && (
        <section
          className="cinematic-selection"
          data-selected-self={selectedPlayer.isCurrentUser || undefined}
        >
          <div className="cinematic-selected-copy" role="status">
            <>
              <motion.p
                initial={reducedMotion ? false : { opacity: 0 }}
                animate={{ opacity: 1 }}
                transition={{ duration: 1 }}
              >
                選ばれたのは、
              </motion.p>
              <motion.h1
                initial={reducedMotion ? false : { opacity: 0, y: 12 }}
                animate={{ opacity: 1, y: 0 }}
                transition={{ delay: 0.8, duration: 1 }}
              >
                {selectedPlayer.name}
              </motion.h1>
              <motion.span
                initial={reducedMotion ? false : { opacity: 0 }}
                animate={{ opacity: 1 }}
                transition={{ delay: 1.5, duration: 0.8 }}
              >
                {selectedPlayer.isCurrentUser
                  ? "あなたが選ばれました。"
                  : `${voteCount}票`}
              </motion.span>
            </>
          </div>
          <div
            className="cinematic-selected-art"
            key={phase}
            aria-hidden="true"
          >
            <div className="cinematic-character-shadow" />
            <motion.div
              className="cinematic-selected-figure"
              initial={reducedMotion ? false : { opacity: 0, y: 12 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 1.1 }}
            >
              <PlayerAvatar
                presentation="portrait"
                src={selectedPlayer.avatarSrc}
                name={selectedPlayer.name}
                initials={selectedPlayer.initials}
              />
            </motion.div>
          </div>
        </section>
      )}
      {phase === "elimination" && (
        <EliminationFilm
          player={selectedPlayer}
          onComplete={completeElimination}
        />
      )}
      {phase === "resolution" && (
        <section
          className="cinematic-resolution"
          aria-label={won ? "勝利の演出" : "敗北の演出"}
        >
          <p className="sr-only">演出の後に勝敗を表示します。</p>
        </section>
      )}
      {phase === "outcome" && (
        <motion.section
          className="cinematic-outcome"
          initial={reducedMotion ? false : { opacity: 0 }}
          animate={{ opacity: 1 }}
          transition={{ duration: 0.8, ease: "easeOut" }}
        >
          <div className="outcome-title">
            <motion.h1
              initial={
                reducedMotion
                  ? false
                  : {
                      opacity: 0,
                      scale: 1.09,
                      rotateX: 12,
                      rotateZ: won ? -0.6 : 0.6,
                    }
              }
              animate={{ opacity: 1, scale: 1, rotateX: 0, rotateZ: 0 }}
              transition={{ duration: 1.6, ease: "easeOut" }}
            >
              {won ? "VICTORY" : "DEFEAT"}
            </motion.h1>
          </div>
          <p className="outcome-player">
            <span>{currentPlayer.name}</span>
            <strong>{won ? "勝利" : "敗北"}</strong>
          </p>
          <motion.div
            className="outcome-action"
            initial={reducedMotion ? false : { opacity: 0 }}
            animate={{ opacity: 1 }}
            transition={{ delay: 1, duration: 0.8, ease: "easeOut" }}
          >
            <Button autoFocus onClick={onComplete}>
              結果を見る
            </Button>
          </motion.div>
        </motion.section>
      )}
      {phase !== "outcome" && (
        <Button
          variant="ghost"
          size="sm"
          className="cinematic-skip"
          onClick={() => setPhase("outcome")}
        >
          演出をスキップ
        </Button>
      )}
    </main>
  );
}
