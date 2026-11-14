import { mkdir } from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { chromium } from "playwright";

const baseUrl = process.env.MOCK_BASE_URL ?? "http://127.0.0.1:5175";
const projectRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const outputDirectory = path.join(projectRoot, "screenshots");

const screens = [
  ["01-login.jpg", "/login"],
  ["02-register.jpg", "/register"],
  ["03-avatar-setup.jpg", "/avatar/setup"],
  ["04-home.jpg", "/home"],
  ["05-room-search.jpg", "/rooms"],
  ["06-room-invite.jpg", "/join/CASE-042"],
  ["07-room-create.jpg", "/rooms/new"],
  ["08-lobby-host.jpg", "/lobby"],
  ["09-lobby-player-wolf.jpg", "/lobby/player/wolf"],
  ["10-lobby-spectator.jpg", "/lobby/spectator"],
  ["11-game-countdown.jpg", "/game/countdown/citizen", true],
  ["12-word-reveal-citizen.jpg", "/game/reveal/citizen"],
  ["13-word-reveal-wolf.jpg", "/game/reveal/wolf"],
  ["14-discussion-citizen.jpg", "/game/citizen"],
  ["15-discussion-wolf.jpg", "/game/wolf"],
  ["16-discussion-spectator.jpg", "/game/spectator"],
  ["17-spectator-vote.jpg", "/game/spectator/vote"],
  ["18-game-paused.jpg", "/game/paused"],
  ["19-game-vote.jpg", "/game/vote"],
  ["20-game-result.jpg", "/game/result"],
  ["21-friends.jpg", "/friends"],
  ["22-profile.jpg", "/profile"],
  ["23-privacy.jpg", "/privacy"],
  ["24-terms.jpg", "/terms"],
  ["25-mock-catalog.jpg", "/mocks"],
];

await mkdir(outputDirectory, { recursive: true });

const browser = await chromium.launch();
try {
  for (const [filename, pathname, isCountdown] of screens) {
    const page = await browser.newPage({
      viewport: { width: 390, height: 844 },
      deviceScaleFactor: 1,
      colorScheme: "light",
    });
    const route = isCountdown
      ? `${pathname}?startsAt=${Date.now() + 5_000}`
      : pathname;

    await page.goto(new URL(route, baseUrl).toString(), {
      waitUntil: "networkidle",
    });
    await page.evaluate(() => document.fonts.ready);
    await page.waitForTimeout(150);
    await page.screenshot({
      path: path.join(outputDirectory, filename),
      type: "jpeg",
      quality: 84,
      fullPage: false,
    });
    await page.close();
    console.log(`captured ${filename} (${pathname})`);
  }
} finally {
  await browser.close();
}
