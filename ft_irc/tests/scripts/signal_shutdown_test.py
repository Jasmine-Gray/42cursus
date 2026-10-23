#!/usr/bin/env python3
"""Verify the server shuts down cleanly on SIGINT and SIGTERM.

Normal mode: with clients connected and in a channel, sending SIGINT (then
SIGTERM in a fresh run) must make the server exit promptly with status 0.

Valgrind mode (--valgrind): the SIGTERM shutdown path must also be free of
leaks and errors, proving the graceful shutdown releases its resources.

Run from the repository root after `make`:

    python3 tests/scripts/signal_shutdown_test.py --server ./ircserv

Exit code 0 means shutdown behaved as expected.
"""

import argparse
import os
import signal
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from irc_test_util import (  # noqa: E402
    Client, Reporter, wrap_argv, valgrind_clean, wait_ready)

HOST = "127.0.0.1"


def start_with_clients(server_bin, port, valgrind, log):
    srv = subprocess.Popen(
        wrap_argv([server_bin, str(port), "pass"], valgrind, log),
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    clients = []
    if wait_ready(HOST, port, srv, 30 if valgrind else 8):
        for nick in ("alice", "bob"):
            c = Client(HOST, port, nick)
            c.send("JOIN #sig")
            clients.append(c)
        time.sleep(0.4)
        for c in clients:
            c.drain()
    return srv, clients


def shutdown(srv, sig, valgrind):
    srv.send_signal(sig)
    try:
        return srv.wait(timeout=12 if valgrind else 5)
    except subprocess.TimeoutExpired:
        srv.kill()
        return None


def run(server_bin, base_port, valgrind, vlog):
    rep = Reporter()

    if valgrind:
        log = os.path.join(vlog, "ircserv-sigterm.valgrind.log")
        srv, clients = start_with_clients(server_bin, base_port, True, log)
        if srv.poll() is not None:
            rep.check("server stays up before signal", False)
        else:
            code = shutdown(srv, signal.SIGTERM, True)
            for c in clients:
                c.close()
            rep.check("SIGTERM shutdown exits (not killed)", code is not None,
                      "exit=%s" % code)
            ok, detail = valgrind_clean(log)
            rep.check("graceful SIGTERM shutdown is Valgrind-clean", ok, detail)
        return rep

    for sig, name, port in (
        (signal.SIGINT, "SIGINT", base_port),
        (signal.SIGTERM, "SIGTERM", base_port + 1),
    ):
        srv, clients = start_with_clients(server_bin, port, False, "")
        if srv.poll() is not None:
            rep.check("%s: server stays up before signal" % name, False)
            continue
        code = shutdown(srv, sig, False)
        for c in clients:
            c.close()
        rep.check("%s makes the server exit promptly" % name, code is not None,
                  "exit=%s" % code)
        rep.check("%s exit status is 0 (clean)" % name, code == 0,
                  "exit=%s" % code)
    return rep


def main():
    parser = argparse.ArgumentParser(description="signal shutdown test")
    parser.add_argument("--server", default="./ircserv")
    parser.add_argument("--port", type=int, default=6740)
    parser.add_argument("--valgrind", action="store_true")
    parser.add_argument("--vlog", default=".")
    args = parser.parse_args()

    if not os.path.exists(args.server):
        print("error: server binary not found at %s (run 'make')" % args.server)
        return 2

    rep = run(args.server, args.port, args.valgrind, args.vlog)
    return rep.summary("signal shutdown checks")


if __name__ == "__main__":
    sys.exit(main())
