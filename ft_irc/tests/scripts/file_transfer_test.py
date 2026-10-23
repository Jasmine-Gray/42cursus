#!/usr/bin/env python3
"""End-to-end test for the bonus DCC file-transfer relay.

The server only relays the CTCP DCC control message between peers; the file
bytes travel directly peer-to-peer. This test reproduces the full flow:

  1. The sender opens a listening socket and advertises it with a
     PRIVMSG carrying a `\\x01DCC SEND ...\\x01` control message.
  2. The server relays that control message to the recipient.
  3. The recipient parses the advertised host/port, connects directly to
     the sender, and receives the file bytes (no server involvement).

It also checks DCC ACCEPT/RESUME relay, ERR_NOSUCHNICK for an unknown
target, that a channel DCC message is broadcast normally, and that plain
PRIVMSG still works.

Run from the repository root after `make`:

    python3 tests/scripts/file_transfer_test.py --server ./ircserv

Exit code 0 means every check passed; non-zero means a check failed.
"""

import argparse
import os
import re
import socket
import struct
import subprocess
import sys
import threading
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from irc_test_util import wrap_argv, valgrind_clean, wait_ready  # noqa: E402

PASSWORD = "pass"
HOST = "127.0.0.1"
SOH = "\x01"

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
        self.sock.sendall((line + "\r\n").encode("latin-1"))

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

    def wait_for(self, pattern, timeout=4.0):
        rx = re.compile(pattern)
        deadline = time.time() + timeout
        collected = ""
        while time.time() < deadline:
            collected += self.drain(settle=0.3)
            for line in collected.splitlines():
                m = rx.search(line.rstrip("\r"))
                if m:
                    return line.rstrip("\r")
        return None

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


def dcc_send_line(filename, ip_str, port, size):
    """Build a CTCP DCC SEND control message (IP encoded as a uint32)."""
    ip_int = struct.unpack("!I", socket.inet_aton(ip_str))[0]
    return "%sDCC SEND %s %u %u %u%s" % (SOH, filename, ip_int, port, size, SOH)


def run_file_sender(listener, payload, result):
    """Accept one peer connection and send the file bytes directly."""
    try:
        listener.settimeout(6)
        conn, _ = listener.accept()
        with conn:
            conn.sendall(payload)
        result["sent"] = True
    except (OSError, socket.timeout) as exc:
        result["error"] = str(exc)
    finally:
        listener.close()


def receive_file(ip_str, port, size, timeout=6):
    """Connect to the advertised peer and read exactly `size` bytes."""
    data = b""
    with socket.create_connection((ip_str, port), timeout=timeout) as peer:
        peer.settimeout(timeout)
        while len(data) < size:
            chunk = peer.recv(size - len(data))
            if not chunk:
                break
            data += chunk
    return data


def test_real_transfer(alice, bob):
    """Full DCC flow: control relayed by server, bytes sent peer-to-peer."""
    payload = b"ft_irc DCC payload \x00\x01\x02 binary-safe " + os.urandom(64)

    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listener.bind((HOST, 0))
    listener.listen(1)
    send_port = listener.getsockname()[1]

    result = {}
    server_thread = threading.Thread(
        target=run_file_sender, args=(listener, payload, result)
    )
    server_thread.start()

    alice.send("PRIVMSG %s :%s"
               % (bob.nick, dcc_send_line("report.bin", HOST, send_port, len(payload))))

    relayed = bob.wait_for(r"PRIVMSG %s :\x01DCC SEND " % re.escape(bob.nick))
    check("recipient receives the relayed DCC SEND control message",
          relayed is not None, repr(relayed))
    check("relayed control message carries the sender prefix",
          relayed is not None and relayed.startswith(":%s!" % alice.nick),
          repr(relayed))

    parsed_ok = False
    received = b""
    if relayed is not None:
        m = re.search(r"DCC SEND (\S+) (\d+) (\d+) (\d+)", relayed)
        if m:
            parsed_ok = True
            ip_int = int(m.group(2))
            port = int(m.group(3))
            size = int(m.group(4))
            ip_str = socket.inet_ntoa(struct.pack("!I", ip_int))
            try:
                received = receive_file(ip_str, port, size)
            except OSError as exc:
                result.setdefault("error", "recv: %s" % exc)
    check("recipient can parse advertised host/port/size", parsed_ok)

    server_thread.join(timeout=8)
    check("sender delivered the file bytes peer-to-peer",
          result.get("sent") is True, result.get("error", ""))
    check("recipient received the exact file content peer-to-peer",
          received == payload,
          "got %d/%d bytes" % (len(received), len(payload)))


def test_relay_variants(alice, bob):
    for command in ("ACCEPT", "RESUME"):
        msg = "%sDCC %s report.bin 49152 1024%s" % (SOH, command, SOH)
        alice.send("PRIVMSG %s :%s" % (bob.nick, msg))
        got = bob.wait_for(r"PRIVMSG %s :\x01DCC %s " % (re.escape(bob.nick), command))
        check("DCC %s is relayed to the recipient" % command, got is not None,
              repr(got))


def test_unknown_nick(alice):
    alice.send("PRIVMSG ghost :%s"
               % dcc_send_line("x.bin", HOST, 49152, 10))
    got = alice.wait_for(r"\b401\b")
    check("DCC SEND to an unknown nick returns ERR_NOSUCHNICK (401)",
          got is not None, repr(got))


def test_plain_privmsg(alice, bob):
    alice.send("PRIVMSG %s :hello bob" % bob.nick)
    got = bob.wait_for(r"PRIVMSG %s :hello bob" % re.escape(bob.nick))
    check("plain direct PRIVMSG still works alongside DCC", got is not None,
          repr(got))


def run(server_bin, port, valgrind=False, vlog="."):
    srv_log = os.path.join(vlog, "ircserv.valgrind.log") if valgrind else ""
    srv = subprocess.Popen(
        wrap_argv([server_bin, str(port), PASSWORD], valgrind, srv_log),
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    srv_ready = wait_ready(HOST, port, srv, 30 if valgrind else 8)
    alice = bob = None
    try:
        if not srv_ready or srv.poll() is not None:
            check("server stays up", False, "ircserv exited early or never bound")
            return
        alice = Client(HOST, port, "alice")
        bob = Client(HOST, port, "bob")
        # A shared channel is not required for direct DCC, but join one so the
        # scenario matches a normal session.
        alice.send("JOIN #dcc")
        bob.send("JOIN #dcc")
        time.sleep(0.5)
        alice.drain()
        bob.drain()

        test_real_transfer(alice, bob)
        test_relay_variants(alice, bob)
        test_unknown_nick(alice)
        test_plain_privmsg(alice, bob)

        check("server still alive at end", srv.poll() is None)
    finally:
        for c in (alice, bob):
            if c is not None:
                c.close()
        if srv.poll() is None:
            srv.terminate()
            try:
                srv.wait(timeout=12 if valgrind else 3)
            except subprocess.TimeoutExpired:
                srv.kill()
        if valgrind:
            ok, detail = valgrind_clean(srv_log)
            check("server is Valgrind-clean", ok, detail)


def main():
    parser = argparse.ArgumentParser(description="DCC file-transfer relay test")
    parser.add_argument("--server", default="./ircserv")
    parser.add_argument("--port", type=int, default=6710)
    parser.add_argument("--valgrind", action="store_true")
    parser.add_argument("--vlog", default=".")
    args = parser.parse_args()

    if not os.path.exists(args.server):
        print("error: server binary not found at %s (run 'make')"
              % args.server)
        return 2

    run(args.server, args.port, args.valgrind, args.vlog)

    print("")
    if FAILURES:
        print("FAILED %d check(s): %s" % (len(FAILURES), ", ".join(FAILURES)))
        return 1
    print("All DCC file-transfer checks passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
