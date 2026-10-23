#!/usr/bin/env python3
"""End-to-end test for the bonus Hit & Blow IRC bot (ircbot).

It starts ircserv and ircbot, connects a raw IRC client, and exercises the
bot's full command surface: help, rules, game start/status/guess/win/giveup,
invalid input handling, per-context game isolation, and direct messages.

Run from the repository root after `make`:

    python3 tests/scripts/bot_test.py [--server ./ircserv] [--bot ./ircbot]

Exit code 0 means every check passed; non-zero means a check failed.
"""

import argparse
import itertools
import os
import re
import socket
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from irc_test_util import wrap_argv, valgrind_clean, wait_ready  # noqa: E402

PASSWORD = "pass"
HOST = "127.0.0.1"
BOT_NICK = "TestBot"


def all_codes():
    """Every valid Hit & Blow answer: 3 distinct digits, leading zero allowed."""
    return ["".join(p) for p in itertools.permutations("0123456789", 3)]


def count_hit_blow(answer, guess):
    hits = sum(1 for i in range(3) if guess[i] == answer[i])
    blows = sum(1 for i in range(3) if guess[i] != answer[i] and guess[i] in answer)
    return hits, blows


class Client(object):
    def __init__(self, host, port, nick):
        self.sock = socket.create_connection((host, port), timeout=5)
        self.sock.settimeout(0.2)
        self.buffer = ""
        self.nick = nick
        self.send("PASS %s" % PASSWORD)
        self.send("NICK %s" % nick)
        self.send("USER %s 0 * :%s" % (nick, nick))
        time.sleep(0.3)
        self.drain()

    def send(self, line):
        self.sock.sendall((line + "\r\n").encode())

    def drain(self, settle=0.5):
        """Read whatever is currently available within a short settle window."""
        deadline = time.time() + settle
        while time.time() < deadline:
            try:
                data = self.sock.recv(4096)
                if not data:
                    break
                self.buffer += data.decode(errors="replace")
                deadline = time.time() + 0.15
            except socket.timeout:
                pass
        out, self.buffer = self.buffer, ""
        return out

    def ask_bot(self, target, text, timeout=4.0):
        """Send a PRIVMSG and return the bot's reply text, or None on timeout."""
        self.send("PRIVMSG %s :%s" % (target, text))
        pattern = re.compile(r":%s!\S+ PRIVMSG \S+ :(.*)" % re.escape(BOT_NICK))
        deadline = time.time() + timeout
        while time.time() < deadline:
            chunk = self.drain(settle=0.3)
            for line in chunk.splitlines():
                m = pattern.match(line.rstrip("\r"))
                if m:
                    return m.group(1).rstrip("\r")
        return None

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


FAILURES = []


def check(name, ok, detail=""):
    status = "PASS" if ok else "FAIL"
    print("[%s] %s%s" % (status, name, (" -- " + detail) if detail else ""))
    if not ok:
        FAILURES.append(name)


def solve_game(client, target):
    """Play until the bot reports a win; assert the win message and return it."""
    candidates = all_codes()
    for _ in range(12):  # a constraint solver needs far fewer than this
        guess = candidates[0]
        reply = client.ask_bot(target, "!guess %s" % guess)
        if reply is None:
            return None
        if "cleared" in reply:
            return reply
        m = re.search(r"(\d+) hit, (\d+) blow", reply)
        if not m:
            return None
        hits, blows = int(m.group(1)), int(m.group(2))
        candidates = [
            c for c in candidates if count_hit_blow(c, guess) == (hits, blows)
        ]
        if not candidates:
            return None
    return None


def run(server_bin, bot_bin, port, valgrind=False, vlog="."):
    srv_log = os.path.join(vlog, "ircserv.valgrind.log") if valgrind else ""
    bot_log = os.path.join(vlog, "ircbot.valgrind.log") if valgrind else ""
    srv = subprocess.Popen(
        wrap_argv([server_bin, str(port), PASSWORD], valgrind, srv_log),
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    srv_ready = wait_ready(HOST, port, srv, 30 if valgrind else 8)
    bot = subprocess.Popen(
        wrap_argv([bot_bin, HOST, str(port), PASSWORD, BOT_NICK, "#botcheck"],
                  valgrind, bot_log),
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    time.sleep(4.0 if valgrind else 1.0)
    client = None
    try:
        if not srv_ready or srv.poll() is not None:
            check("server stays up", False, "ircserv exited early or never bound")
            return
        if bot.poll() is not None:
            check("bot stays up", False, "ircbot exited early")
            return

        client = Client(HOST, port, "alice")
        client.send("JOIN #botcheck")
        time.sleep(0.5)
        client.drain()

        # Help and rules
        help_reply = client.ask_bot("#botcheck", "!help")
        check("!help lists commands", help_reply is not None
              and "!hb start" in help_reply and "!guess" in help_reply,
              repr(help_reply))
        rules_reply = client.ask_bot("#botcheck", "!hb rules")
        check("!hb rules explains Hit/Blow", rules_reply is not None
              and "Hit" in rules_reply and "Blow" in rules_reply, repr(rules_reply))

        # Guess before a game exists
        no_game = client.ask_bot("#botcheck", "!guess 123")
        check("guess with no game is rejected", no_game is not None
              and "No Hit & Blow game" in no_game, repr(no_game))

        # Start, status, invalid guess, win
        start_reply = client.ask_bot("#botcheck", "!hb start")
        check("!hb start begins a game", start_reply is not None
              and "started" in start_reply.lower(), repr(start_reply))

        dup_start = client.ask_bot("#botcheck", "!hb start")
        check("!hb start twice is rejected", dup_start is not None
              and "already running" in dup_start.lower(), repr(dup_start))

        status_reply = client.ask_bot("#botcheck", "!hb status")
        check("!hb status reports a running game", status_reply is not None
              and "running" in status_reply.lower() and "alice" in status_reply,
              repr(status_reply))

        bad_guess = client.ask_bot("#botcheck", "!guess 112")
        check("invalid guess is rejected", bad_guess is not None
              and "Invalid guess" in bad_guess, repr(bad_guess))

        win_reply = solve_game(client, "#botcheck")
        check("solver wins and bot announces the answer", win_reply is not None
              and "cleared" in win_reply and re.search(r"Answer was \d{3}", win_reply)
              is not None, repr(win_reply))

        # After a win the game is over
        after_win = client.ask_bot("#botcheck", "!hb status")
        check("game ends after a win", after_win is not None
              and "No Hit & Blow game" in after_win, repr(after_win))

        # Giveup reveals the answer
        client.ask_bot("#botcheck", "!hb start")
        giveup = client.ask_bot("#botcheck", "!hb giveup")
        check("!hb giveup stops and reveals the answer", giveup is not None
              and "stopped" in giveup.lower()
              and re.search(r"Answer was \d{3}", giveup) is not None, repr(giveup))

        # Unknown command
        unknown = client.ask_bot("#botcheck", "!bogus")
        check("unknown command is reported", unknown is not None
              and "Unknown command" in unknown, repr(unknown))

        # Direct message context is independent from the channel
        dm_help = client.ask_bot(BOT_NICK, "!help")
        check("bot answers direct messages", dm_help is not None
              and "!hb start" in dm_help, repr(dm_help))
        client.ask_bot(BOT_NICK, "!hb start")
        dm_status = client.ask_bot(BOT_NICK, "!hb status")
        chan_status = client.ask_bot("#botcheck", "!hb status")
        check("DM game is isolated from channel game", dm_status is not None
              and "running" in dm_status.lower()
              and chan_status is not None and "No Hit & Blow game" in chan_status,
              "dm=%r chan=%r" % (dm_status, chan_status))

        check("server still alive at end", srv.poll() is None)
        check("bot still alive at end", bot.poll() is None)
    finally:
        if client is not None:
            client.close()
        for proc in (bot, srv):
            if proc.poll() is None:
                proc.terminate()
                try:
                    proc.wait(timeout=12 if valgrind else 3)
                except subprocess.TimeoutExpired:
                    proc.kill()
        if valgrind:
            for label, log in (("server", srv_log), ("bot", bot_log)):
                ok, detail = valgrind_clean(log)
                check("%s is Valgrind-clean" % label, ok, detail)


def main():
    parser = argparse.ArgumentParser(description="Hit & Blow bot end-to-end test")
    parser.add_argument("--server", default="./ircserv")
    parser.add_argument("--bot", default="./ircbot")
    parser.add_argument("--port", type=int, default=6699)
    parser.add_argument("--valgrind", action="store_true")
    parser.add_argument("--vlog", default=".")
    args = parser.parse_args()

    for path, label in ((args.server, "server"), (args.bot, "bot")):
        if not os.path.exists(path):
            print("error: %s binary not found at %s (run 'make')"
                  % (label, path))
            return 2

    run(args.server, args.bot, args.port, args.valgrind, args.vlog)

    print("")
    if FAILURES:
        print("FAILED %d check(s): %s" % (len(FAILURES), ", ".join(FAILURES)))
        return 1
    print("All Hit & Blow bot checks passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
