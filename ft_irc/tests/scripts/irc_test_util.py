"""Shared helpers for the standalone ft_irc Python tests.

Provides a minimal IRC client, a process launcher that can optionally wrap a
binary in Valgrind, and a Valgrind log validator. Kept dependency-free and
C++98-project-agnostic so each test script can `import irc_test_util`.
"""

import os
import re
import socket
import time

PASSWORD = "pass"


def valgrind_argv(logpath):
    return [
        "valgrind",
        "--leak-check=full",
        "--show-leak-kinds=all",
        "--errors-for-leak-kinds=definite,indirect",
        "--track-origins=yes",
        "--error-exitcode=99",
        "--log-file=" + logpath,
    ]


def wrap_argv(argv, valgrind, logpath):
    """Prefix argv with Valgrind when requested."""
    return (valgrind_argv(logpath) + list(argv)) if valgrind else list(argv)


def wait_ready(host, port, proc, timeout):
    """Wait until the server accepts a TCP connection (handles slow Valgrind
    cold starts). Returns False if it never became ready or the process exited.
    """
    deadline = time.time() + timeout
    while time.time() < deadline:
        if proc.poll() is not None:
            return False
        try:
            probe = socket.create_connection((host, port), timeout=1)
            probe.close()
            return True
        except OSError:
            time.sleep(0.2)
    return False


def valgrind_clean(logpath):
    """Return (ok, detail) by parsing a Valgrind --log-file report.

    Tolerates 'still reachable' blocks (expected when a process is stopped by a
    signal mid-run) but requires zero errors and zero definite/indirect leaks.
    """
    if not os.path.exists(logpath):
        return (False, "no valgrind log at %s" % logpath)
    text = open(logpath, errors="replace").read()
    m = re.search(r"ERROR SUMMARY:\s+(\d+)\s+errors", text)
    errors = int(m.group(1)) if m else -1

    def lost(kind):
        mm = re.search(r"%s lost:\s+([\d,]+)\s+bytes" % kind, text)
        return int(mm.group(1).replace(",", "")) if mm else 0

    definite = lost("definitely")
    indirect = lost("indirectly")
    ok = errors == 0 and definite == 0 and indirect == 0
    detail = "errors=%s, definitely lost=%d, indirectly lost=%d" % (
        errors, definite, indirect)
    return (ok, detail)


class Client(object):
    """A tiny line-oriented IRC client used by the standalone tests."""

    def __init__(self, host, port, nick, register=True):
        self.sock = socket.create_connection((host, port), timeout=5)
        self.sock.settimeout(0.2)
        self.buffer = ""
        self.nick = nick
        if register:
            self.send("PASS %s" % PASSWORD)
            self.send("NICK %s" % nick)
            self.send("USER %s 0 * :%s" % (nick, nick))
            time.sleep(0.3)
            self.drain()

    def send(self, line):
        self.sock.sendall((line + "\r\n").encode("latin-1"))

    def send_raw(self, data):
        self.sock.sendall(data.encode("latin-1"))

    def drain(self, settle=0.4):
        deadline = time.time() + settle
        while time.time() < deadline:
            try:
                data = self.sock.recv(4096)
                if not data:
                    break
                self.buffer += data.decode("latin-1")
                deadline = time.time() + 0.15
            except socket.timeout:
                pass
        out, self.buffer = self.buffer, ""
        return out

    def expect(self, pattern, timeout=4.0):
        rx = re.compile(pattern)
        deadline = time.time() + timeout
        collected = ""
        while time.time() < deadline:
            collected += self.drain(settle=0.3)
            for line in collected.splitlines():
                if rx.search(line.rstrip("\r")):
                    return line.rstrip("\r")
        return None

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


class Reporter(object):
    """Collects pass/fail results for a single test script."""

    def __init__(self):
        self.failures = []

    def check(self, name, ok, detail=""):
        status = "PASS" if ok else "FAIL"
        print("[%s] %s%s" % (status, name, (" -- " + detail) if detail else ""))
        if not ok:
            self.failures.append(name)

    def summary(self, label):
        print("")
        if self.failures:
            print("FAILED %d check(s): %s"
                  % (len(self.failures), ", ".join(self.failures)))
            return 1
        print("All %s passed." % label)
        return 0
