#!/usr/bin/env python3
"""Behavior test that follows the README "Operator Commands and Channel Modes".

It walks the exact scenarios documented in README.md with two clients
(`alice`, the operator of `#manual`, and `bob`), plus a third client for the
user-limit case, and asserts that each documented behavior actually happens:

  * invite-only  (+i / -i)   -> 473 before invite, JOIN after invite
  * topic lock   (+t / -t)   -> 482 for a non-operator, success for the operator
  * channel key  (+k / -k)   -> 475 without the key, JOIN with the key
  * operator     (+o / -o)   -> operator action allowed, then 482 after demotion
  * user limit   (+l / -l)   -> 471 when full, JOIN after the limit is lifted
  * other ops    (MODE/TOPIC/KICK)

Run from the repository root after `make`:

    python3 tests/scripts/readme_modes_test.py --server ./ircserv

Exit code 0 means every documented behavior was reproduced.
"""

import argparse
import os
import re
import socket
import subprocess
import sys
import time

PASSWORD = "pass"
HOST = "127.0.0.1"
CHANNEL = "#manual"

FAILURES = []


def check(name, ok, detail=""):
    status = "PASS" if ok else "FAIL"
    print("[%s] %s%s" % (status, name, (" -- " + detail) if detail else ""))
    if not ok:
        FAILURES.append(name)


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

    def drain(self, settle=0.4):
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

    def expect(self, pattern, timeout=4.0):
        """Return the first line matching pattern within timeout, else None."""
        rx = re.compile(pattern)
        deadline = time.time() + timeout
        collected = ""
        while time.time() < deadline:
            collected += self.drain(settle=0.3)
            for line in collected.splitlines():
                if rx.search(line.rstrip("\r")):
                    return line.rstrip("\r")
        return None

    def join(self, channel, key=None):
        self.send("JOIN %s%s" % (channel, (" " + key) if key else ""))

    def part(self, channel):
        self.send("PART %s" % channel)
        time.sleep(0.3)
        self.drain()

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


def join_succeeds(client, channel, key=None):
    client.join(channel, key)
    return client.expect(r"\bJOIN\b\s+:?%s([\r\n]+|$)" % re.escape(channel)) is not None


def join_refused(client, channel, code, key=None):
    client.join(channel, key)
    return client.expect(r"\b%s\b" % code) is not None


def register(host, port, nick):
    return Client(host, port, nick)


def run(server_bin, port):
    srv = subprocess.Popen(
        [server_bin, str(port), PASSWORD],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    time.sleep(1.0)
    alice = bob = carol = None
    try:
        if srv.poll() is not None:
            check("server stays up", False, "ircserv exited early")
            return

        alice = register(HOST, port, "alice")
        bob = register(HOST, port, "bob")
        # alice creates the channel (becomes operator), bob joins.
        check("alice creates #manual as operator", join_succeeds(alice, CHANNEL))
        check("bob joins the open channel", join_succeeds(bob, CHANNEL))
        alice.drain(); bob.drain()

        # --- Invite-only: +i / -i ---
        alice.send("MODE %s +i" % CHANNEL)
        time.sleep(0.3); alice.drain()
        bob.part(CHANNEL)
        check("+i: bob is refused without an invite (473)",
              join_refused(bob, CHANNEL, "473"))
        alice.send("INVITE bob %s" % CHANNEL)
        time.sleep(0.3); bob.drain()
        check("+i: bob joins after being invited", join_succeeds(bob, CHANNEL))
        alice.send("MODE %s -i" % CHANNEL)
        time.sleep(0.3); alice.drain(); bob.drain()

        # --- Topic lock: +t / -t ---
        alice.send("MODE %s +t" % CHANNEL)
        time.sleep(0.3); alice.drain(); bob.drain()
        bob.send("TOPIC %s :hijack" % CHANNEL)
        check("+t: non-operator bob cannot set the topic (482)",
              bob.expect(r"\b482\b") is not None)
        alice.send("TOPIC %s :Official topic" % CHANNEL)
        check("+t: operator alice can set the topic",
              alice.expect(r"TOPIC %s :Official topic|\b332\b" % re.escape(CHANNEL))
              is not None)
        alice.send("MODE %s -t" % CHANNEL)
        time.sleep(0.3); alice.drain(); bob.drain()

        # --- Channel key: +k / -k ---
        alice.send("MODE %s +k secret" % CHANNEL)
        time.sleep(0.3); alice.drain()
        bob.part(CHANNEL)
        check("+k: bob is refused without the key (475)",
              join_refused(bob, CHANNEL, "475"))
        check("+k: bob joins with the correct key",
              join_succeeds(bob, CHANNEL, key="secret"))
        alice.send("MODE %s -k" % CHANNEL)
        time.sleep(0.3); alice.drain(); bob.drain()

        # --- Operator privilege: +o / -o (topic locked so it is operator-gated) ---
        alice.send("MODE %s +t" % CHANNEL)
        alice.send("MODE %s +o bob" % CHANNEL)
        time.sleep(0.4); alice.drain(); bob.drain()
        bob.send("TOPIC %s :set by bob" % CHANNEL)
        check("+o: promoted bob can set the topic",
              bob.expect(r"TOPIC %s :set by bob" % re.escape(CHANNEL)) is not None
              and bob.expect(r"\b482\b", timeout=0.5) is None)
        alice.send("MODE %s -o bob" % CHANNEL)
        time.sleep(0.4); alice.drain(); bob.drain()
        bob.send("TOPIC %s :nope" % CHANNEL)
        check("-o: demoted bob can no longer set the topic (482)",
              bob.expect(r"\b482\b") is not None)
        alice.send("MODE %s -t" % CHANNEL)
        time.sleep(0.3); alice.drain(); bob.drain()

        # --- User limit: +l / -l ---
        alice.send("MODE %s +l 2" % CHANNEL)  # alice + bob already fill it
        time.sleep(0.3); alice.drain()
        carol = register(HOST, port, "carol")
        check("+l: a third client is refused when full (471)",
              join_refused(carol, CHANNEL, "471"))
        alice.send("MODE %s -l" % CHANNEL)
        time.sleep(0.3); alice.drain()
        check("-l: carol joins after the limit is lifted",
              join_succeeds(carol, CHANNEL))

        # --- Other operator commands ---
        alice.send("MODE %s" % CHANNEL)
        check("MODE query reports the active modes (324)",
              alice.expect(r"\b324\b") is not None)
        alice.send("TOPIC %s :New topic text" % CHANNEL)
        check("operator sets the topic",
              alice.expect(r"TOPIC %s :New topic text|\b332\b" % re.escape(CHANNEL))
              is not None)
        alice.send("KICK %s bob :bye" % CHANNEL)
        check("operator KICK removes bob",
              bob.expect(r"\bKICK\b\s+%s\s+bob\b" % re.escape(CHANNEL)) is not None)

        check("server still alive at end", srv.poll() is None)
    finally:
        for c in (carol, bob, alice):
            if c is not None:
                c.close()
        if srv.poll() is None:
            srv.terminate()
            try:
                srv.wait(timeout=3)
            except subprocess.TimeoutExpired:
                srv.kill()


def main():
    parser = argparse.ArgumentParser(description="README mode walkthrough test")
    parser.add_argument("--server", default="./ircserv")
    parser.add_argument("--port", type=int, default=6720)
    args = parser.parse_args()

    if not os.path.exists(args.server):
        print("error: server binary not found at %s (run 'make')" % args.server)
        return 2

    run(args.server, args.port)

    print("")
    if FAILURES:
        print("FAILED %d check(s): %s" % (len(FAILURES), ", ".join(FAILURES)))
        return 1
    print("All README mode-walkthrough behaviors reproduced.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
