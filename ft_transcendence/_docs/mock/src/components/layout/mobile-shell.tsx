import { ArrowLeft, DoorClosed, Search, Users, HatGlasses } from "lucide-react";
import { useEffect, useRef, useState, type ReactNode } from "react";
import { Link, useLocation } from "wouter";

import { VictorianScene } from "@/components/game-ui/victorian-scene";
import { GameMark } from "@/components/game-ui/game-mark";
import { PlayerAvatar } from "@/components/game-ui/player-avatar";
import { MobilePortalProvider } from "@/components/layout/mobile-portal-context";
import { buttonVariants } from "@/components/ui/button";
import { cn } from "@/lib/utils";

type MobileShellProps = {
  children: ReactNode;
  title?: string;
  backHref?: string;
  actions?: ReactNode;
  bottomNav?: boolean;
  darkHeader?: boolean;
  ambientScene?: boolean;
};

export function MobileShell({
  children,
  title,
  backHref,
  actions,
  bottomNav,
  darkHeader,
  ambientScene = true,
}: MobileShellProps) {
  const portalContainerRef = useRef<HTMLDivElement>(null);
  const sceneRef = useRef<HTMLDivElement>(null);
  const [sceneVisible, setSceneVisible] = useState(true);
  const [documentVisible, setDocumentVisible] = useState(true);
  const [reducedMotion, setReducedMotion] = useState(false);

  useEffect(() => {
    const preference = window.matchMedia?.("(prefers-reduced-motion: reduce)");
    const updatePreference = () =>
      setReducedMotion(preference?.matches ?? false);
    const updateVisibility = () => setDocumentVisible(!document.hidden);
    updatePreference();
    updateVisibility();
    preference?.addEventListener("change", updatePreference);
    document.addEventListener("visibilitychange", updateVisibility);
    return () => {
      preference?.removeEventListener("change", updatePreference);
      document.removeEventListener("visibilitychange", updateVisibility);
    };
  }, []);

  useEffect(() => {
    if (!sceneRef.current || typeof IntersectionObserver === "undefined")
      return;
    const observer = new IntersectionObserver(([entry]) => {
      setSceneVisible(entry.isIntersecting);
    });
    observer.observe(sceneRef.current);
    return () => observer.disconnect();
  }, []);

  return (
    <MobilePortalProvider value={portalContainerRef}>
      <div
        className="mystery-scene"
        data-cinematic={!ambientScene}
        ref={sceneRef}
        data-motion-paused={!sceneVisible || !documentVisible || reducedMotion}
      >
        <div className="scene-environment" aria-hidden="true">
          <VictorianScene animated={ambientScene} />
          {ambientScene && <div className="scene-drifting-mist" />}
          <div className="scene-vignette" />
        </div>
        <div className="game-frame">
          <div className="mobile-game-shell relative isolate z-10 flex min-h-0 flex-1 flex-col overflow-hidden">
            {bottomNav && (
              <header className="game-menu-header">
                <Link href="/home" aria-label="ホームへ戻る">
                  <GameMark inverse />
                </Link>
                <TopNavigation />
              </header>
            )}
            {title ? (
              <header
                className={cn(
                  "case-page-header z-20 shrink-0",
                  darkHeader
                    ? "border-brass/30 bg-night text-white"
                    : "border-border bg-card",
                )}
              >
                <div className="case-header-parts">
                  {backHref && (
                    <Link
                      aria-label="前の画面へ戻る"
                      className={cn(
                        buttonVariants({ size: "icon", variant: "ghost" }),
                        "shrink-0",
                        darkHeader &&
                          "text-white hover:bg-white/10 hover:text-white",
                      )}
                      href={backHref}
                    >
                      <ArrowLeft className="size-5" aria-hidden="true" />
                    </Link>
                  )}
                  <h1 className="case-title-plaque">{title}</h1>
                  {actions}
                </div>
              </header>
            ) : null}

            <div className="case-scroll min-h-0 flex-1 overflow-y-auto">
              {children}
            </div>

            <div
              ref={portalContainerRef}
              className="pointer-events-none absolute inset-0 z-40"
              data-mobile-portal
            />
          </div>
        </div>
      </div>
    </MobilePortalProvider>
  );
}

export function BrandHeader() {
  return (
    <header className="case-brand-header border-brass/30 bg-night flex items-center justify-between border-b px-5 py-4 text-white">
      <Link
        aria-label="ホームへ戻る"
        className="focus-visible:ring-ring/50 rounded-lg outline-none focus-visible:ring-3"
        href="/home"
      >
        <GameMark inverse />
      </Link>
      <Link
        aria-label="プロフィールを開く"
        className="focus-visible:ring-ring/50 rounded-full outline-none focus-visible:ring-3"
        href="/profile"
      >
        <PlayerAvatar
          className="size-9"
          src="/avatars/yu.webp"
          initials="YU"
          name="田中 ユウ"
        />
      </Link>
    </header>
  );
}

const navItems = [
  { href: "/home", label: "ホーム", icon: DoorClosed },
  { href: "/rooms", label: "部屋", icon: Search },
  { href: "/friends", label: "フレンド", icon: Users },
  { href: "/profile", label: "自分", icon: HatGlasses },
];

function TopNavigation() {
  const [location] = useLocation();

  return (
    <nav
      className="case-navigation bg-card z-20 shrink-0 border-t px-2 pt-2 pb-[max(0.5rem,env(safe-area-inset-bottom))]"
      aria-label="メインナビゲーション"
    >
      {navItems.map((item) => {
        const active =
          location === item.href ||
          (item.href === "/rooms" &&
            (location.startsWith("/rooms/") || location.startsWith("/join/")));
        const Icon = item.icon;

        return (
          <Link
            key={item.href}
            className={cn(
              "focus-visible:ring-ring/50 flex min-h-12 flex-col items-center justify-center gap-1 rounded-lg text-xs font-medium outline-none focus-visible:ring-3",
              active
                ? "bg-primary/10 text-primary"
                : "text-muted-foreground hover:bg-muted hover:text-foreground",
            )}
            href={item.href}
            aria-current={active ? "page" : undefined}
          >
            <Icon className="size-5" aria-hidden="true" />
            {item.label}
          </Link>
        );
      })}
    </nav>
  );
}
