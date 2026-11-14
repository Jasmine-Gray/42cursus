import { ArrowUpRight } from "lucide-react";
import { motion, useReducedMotion } from "motion/react";
import { Link, useLocation } from "wouter";

export function PlayMenu() {
  const reducedMotion = useReducedMotion();
  const [location] = useLocation();
  return (
    <section
      className="home-play-section hunt-play-menu"
      aria-label="ゲームに参加"
    >
      <h2 className="hunt-menu-eyebrow">PLAY</h2>
      <div className="quick-play-actions">
        {[
          { href: "/rooms", label: "URLで参加" },
          { href: "/rooms/new", label: "部屋を作る" },
        ].map((item, index) => (
          <motion.div
            key={item.href}
            initial={reducedMotion ? false : { opacity: 0, x: -12 }}
            animate={{ opacity: 1, x: 0 }}
            transition={{ duration: 0.45, delay: index * 0.1, ease: "easeOut" }}
          >
            <Link
              href={item.href}
              className="hunt-play-action"
              data-tone={index === 1 ? "accent" : undefined}
              aria-current={
                location === item.href ||
                (item.href === "/rooms" && location.startsWith("/join/"))
                  ? "page"
                  : undefined
              }
            >
              <span>{item.label}</span>
              <ArrowUpRight aria-hidden="true" className="size-5" />
            </Link>
          </motion.div>
        ))}
      </div>
    </section>
  );
}
