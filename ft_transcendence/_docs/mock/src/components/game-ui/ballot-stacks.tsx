import type { CSSProperties } from "react";
import { motion, useReducedMotion } from "motion/react";
import { PlayerAvatar } from "@/components/game-ui/player-avatar";
import type { Player, revealedVotes } from "@/data/mock-data";

type BallotStacksProps = { players: Player[]; votes: typeof revealedVotes };

export function BallotStacks({ players, votes }: BallotStacksProps) {
  const reducedMotion = useReducedMotion();
  const counts = players.map(
    (player) => votes.filter((vote) => vote.targetId === player.id).length,
  );
  const highest = Math.max(0, ...counts);
  const lastVote = votes.at(-1);
  const lastVoter = players.find((player) => player.id === lastVote?.voterId);
  const lastTarget = players.find((player) => player.id === lastVote?.targetId);

  return (
    <>
      <div
        className="ballot-board"
        aria-label="参加者ごとの得票"
        style={
          {
            "--players": players.length,
            "--max-votes": Math.max(1, highest),
          } as CSSProperties
        }
      >
        {players.map((target, index) => {
          const received = votes
            .filter((vote) => vote.targetId === target.id)
            .toReversed();
          const leading = highest > 0 && counts[index] === highest;
          return (
            <section
              className="ballot-column"
              key={target.id}
              data-player-id={target.id}
              data-leading={leading || undefined}
              data-current-player={target.isCurrentUser || undefined}
              aria-label={`${target.name}、${counts[index]}票${leading ? "、最多票" : ""}`}
            >
              <ol
                className="ballot-stack"
                aria-label={`${target.name}への投票`}
              >
                {received.map((vote) => {
                  const voter = players.find(
                    (player) => player.id === vote.voterId,
                  );
                  if (!voter) return null;
                  return (
                    <motion.li
                      className="ballot-ticket"
                      key={voter.id}
                      data-voter-id={voter.id}
                      data-current-player={voter.isCurrentUser || undefined}
                      initial={
                        reducedMotion
                          ? false
                          : { opacity: 0, y: -72, scale: 1.08, rotate: -3 }
                      }
                      animate={{ opacity: 1, y: 0, scale: 1, rotate: 0 }}
                      transition={{ duration: 0.85, ease: [0.16, 1, 0.3, 1] }}
                    >
                      <PlayerAvatar
                        className="size-6"
                        src={voter.avatarSrc}
                        name={voter.name}
                        initials={voter.initials}
                      />
                      <span>{voter.name}</span>
                    </motion.li>
                  );
                })}
              </ol>
              <div className="ballot-candidate">
                <PlayerAvatar
                  className="size-14"
                  src={target.avatarSrc}
                  name={target.name}
                  initials={target.initials}
                />
                <strong>{target.name}</strong>
                <span className="ballot-count" aria-hidden="true">
                  <motion.b
                    key={counts[index]}
                    initial={
                      reducedMotion ? false : { opacity: 0.3, scale: 1.2 }
                    }
                    animate={{ opacity: 1, scale: 1 }}
                    transition={{ duration: 0.6 }}
                  >
                    {counts[index]}
                  </motion.b>
                  <span>票</span>
                </span>
              </div>
            </section>
          );
        })}
      </div>
      <p className="sr-only" role="status">
        {lastVoter && lastTarget
          ? `${lastVoter.name}は${lastTarget.name}に投票しました。`
          : "開票を始めます。"}
      </p>
    </>
  );
}
