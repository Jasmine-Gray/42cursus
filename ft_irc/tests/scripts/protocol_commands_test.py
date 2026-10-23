#!/usr/bin/env python3
"""Behavior test for protocol commands documented in the README quick reference
that the integration suite did not yet exercise: PING/PONG, QUIT, NAMES, WHO,
and CAP.

Run from the repository root after `make`:

    python3 tests/scripts/protocol_commands_test.py --server ./ircserv

Exit code 0 means every command behaved as documented.
"""

import argparse
import os
import re
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from irc_test_util import (  # noqa: E402
    Client, Reporter, wrap_argv, valgrind_clean, wait_ready)

HOST = "127.0.0.1"
CHANNEL = "#proto"


def run(server_bin, port, valgrind, vlog):
    server_log = os.path.join(vlog, "ircserv.valgrind.log") if valgrind else ""
    srv = subprocess.Popen(
        wrap_argv([server_bin, str(port), "pass"], valgrind, server_log),
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    srv_ready = wait_ready(HOST, port, srv, 30 if valgrind else 8)
    rep = Reporter()
    alice = bob = None
    try:
        if not srv_ready or srv.poll() is not None:
            rep.check("server stays up", False,
                      "ircserv exited early or never bound")
            return rep
        alice = Client(HOST, port, "alice")
        bob = Client(HOST, port, "bob")

        # PING -> PONG with the same token
        alice.send("PING :token123")
        rep.check("PING gets a matching PONG",
                  alice.expect(r"\bPONG\b.*token123") is not None)
        alice.send("PING")
        rep.check("PING without a token returns ERR_NEEDMOREPARAMS (461)",
                  alice.expect(r"\b461\b") is not None)

        # NAMES lists members (353) and ends (366)
        alice.send("JOIN %s" % CHANNEL)
        bob.send("JOIN %s" % CHANNEL)
        time.sleep(0.4)
        alice.drain(); bob.drain()
        alice.send("NAMES %s" % CHANNEL)
        names_resp = ""
        for _ in range(12):
            names_resp += alice.drain(settle=0.3)
            if re.search(r"\b353\b", names_resp) and re.search(r"\b366\b", names_resp):
                break
        m353 = re.search(r"353[^\r\n]*", names_resp)
        names = m353.group(0) if m353 else None
        rep.check("NAMES returns RPL_NAMREPLY (353) listing members",
                  names is not None and "alice" in names and "bob" in names,
                  repr(names))
        rep.check("NAMES ends with RPL_ENDOFNAMES (366)",
                  re.search(r"\b366\b", names_resp) is not None)

        # WHO is acknowledged with RPL_ENDOFWHO (315)
        alice.send("WHO %s" % CHANNEL)
        rep.check("WHO is acknowledged with RPL_ENDOFWHO (315)",
                  alice.expect(r"\b315\b") is not None)

        # CAP LS gets a CAP response (used by real clients on connect)
        alice.send("CAP LS")
        rep.check("CAP LS gets a CAP response",
                  alice.expect(r"\bCAP\b.*\bLS\b") is not None)

        # Joining a channel that has a topic delivers RPL_TOPIC (332) with text
        alice.send("TOPIC %s :welcome to the channel" % CHANNEL)
        time.sleep(0.3)
        alice.drain()
        carol = Client(HOST, port, "carol")
        carol.send("JOIN %s" % CHANNEL)
        topic_line = carol.expect(r"\b332\b.*%s" % re.escape(CHANNEL))
        rep.check("JOIN delivers RPL_TOPIC (332) with the topic text",
                  topic_line is not None and "welcome to the channel" in topic_line,
                  repr(topic_line))
        carol.close()

        # QUIT notifies a co-member and closes the connection
        bob.send("QUIT :leaving now")
        rep.check("QUIT is broadcast to a channel co-member",
                  alice.expect(r":bob!\S+ QUIT :") is not None)
        time.sleep(0.3)
        rep.check("server still alive after a client quits", srv.poll() is None)
    finally:
        for c in (alice, bob):
            if c is not None:
                c.close()
        if srv.poll() is None:
            srv.terminate()
            try:
                srv.wait(timeout=8 if valgrind else 3)
            except subprocess.TimeoutExpired:
                srv.kill()
        if valgrind:
            ok, detail = valgrind_clean(server_log)
            rep.check("server is Valgrind-clean", ok, detail)
    return rep


def main():
    parser = argparse.ArgumentParser(description="core protocol command test")
    parser.add_argument("--server", default="./ircserv")
    parser.add_argument("--port", type=int, default=6730)
    parser.add_argument("--valgrind", action="store_true")
    parser.add_argument("--vlog", default=".")
    args = parser.parse_args()

    if not os.path.exists(args.server):
        print("error: server binary not found at %s (run 'make')" % args.server)
        return 2

    rep = run(args.server, args.port, args.valgrind, args.vlog)
    return rep.summary("core protocol command checks")


if __name__ == "__main__":
    sys.exit(main())
