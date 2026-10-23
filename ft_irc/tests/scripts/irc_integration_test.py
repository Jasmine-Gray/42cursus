#!/usr/bin/env python3
"""Black-box integration tests for the mandatory ft_irc requirements.

The tests intentionally use only Python's standard library so they can run in
GitHub Actions and on local 42 machines without depending on a specific nc
variant or IRC client.
"""

from __future__ import print_function

import argparse
import logging
import os
import random
import re
import shlex
import socket
import subprocess
import struct
import sys
import tempfile
import textwrap
import time


PASSWORD = "pass"
HOST = "127.0.0.1"
DEFAULT_TIMEOUT = 3.0
LOGGER = logging.getLogger("ft_irc_test")
COLOR_ENABLED = False
VERBOSE = False
CURRENT_TEST_NAME = ""
CURRENT_CASE_INDEX = 0
DETAIL_LINES = []
COMMAND_LINES = []
FAILURE_DETAIL_LINES = 32
LOG_WRAP_WIDTH = 96
PAYLOAD_LINE_LIMIT = 24
PAYLOAD_LINE_WIDTH = 180
CLIENT_ACTORS = {}
NEXT_CLIENT_ACTOR_INDEX = 0
TRACE_LABEL_WIDTH = len("case-00")
TRACE_CONTINUATION = " " * (TRACE_LABEL_WIDTH + 4)
OUTPUT_CONTINUATION = TRACE_CONTINUATION + "  "
CLIENT_ACTOR_COLOR_NAMES = [
    "actor_0",
    "actor_1",
    "actor_2",
    "actor_3",
    "actor_4",
    "actor_5",
]
TRACE_LABEL_COLORS = {
    "cmd": "input",
    "expect": "expect",
    "output": "info",
    "exit": "warn",
    "result": "result",
}
COLOR_CODES = {
    "reset": "\033[0m",
    "ok": "\033[1;32m",
    "fail": "\033[1;31m",
    "warn": "\033[1;33m",
    "info": "\033[1;34m",
    "input": "\033[1;35m",
    "expect": "\033[1;36m",
    "result": "\033[1;32m",
    "server": "\033[1;34m",
    "client": "\033[1;36m",
    "test": "\033[1;37m",
    "actor_0": "\033[1;35m",
    "actor_1": "\033[1;36m",
    "actor_2": "\033[1;33m",
    "actor_3": "\033[1;32m",
    "actor_4": "\033[1;34m",
    "actor_5": "\033[1;37m",
}


class TestFailure(Exception):
    pass


def fail(message):
    raise TestFailure(message)


def configure_logging():
    if LOGGER.handlers:
        return
    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter(logging.Formatter("%(message)s"))
    LOGGER.addHandler(handler)
    LOGGER.setLevel(logging.INFO)
    LOGGER.propagate = False


def set_color_mode(mode):
    global COLOR_ENABLED

    if mode == "always":
        COLOR_ENABLED = True
    elif mode == "never":
        COLOR_ENABLED = False
    elif os.environ.get("NO_COLOR"):
        COLOR_ENABLED = False
    elif os.environ.get("CLICOLOR_FORCE") not in (None, "", "0"):
        COLOR_ENABLED = True
    elif os.environ.get("GITHUB_ACTIONS") == "true":
        COLOR_ENABLED = True
    else:
        COLOR_ENABLED = sys.stdout.isatty() and os.environ.get("TERM") != "dumb"


def colorize(text, color):
    if not COLOR_ENABLED:
        return text
    return COLOR_CODES[color] + text + COLOR_CODES["reset"]


def tag(label, color):
    return colorize("[%s]" % label, color)


def wrapped_lines(text, first_indent, later_indent=None, width=LOG_WRAP_WIDTH):
    if later_indent is None:
        later_indent = first_indent
    lines = textwrap.wrap(
        text,
        width=width,
        initial_indent=first_indent,
        subsequent_indent=later_indent,
    )
    if lines:
        return lines
    return [first_indent.rstrip()]


def log_wrapped(text, first_indent="       ", later_indent=None):
    for line in wrapped_lines(text, first_indent, later_indent):
        LOGGER.info("%s", line)


def format_log_line(message, *args):
    if args:
        try:
            return message % args
        except TypeError:
            return message
    return message


def detail_log(message="", *args):
    line = format_log_line(message, *args)
    if VERBOSE:
        LOGGER.info("%s", line)
    else:
        DETAIL_LINES.append(line)


def command_log(message="", *args):
    if VERBOSE:
        return
    COMMAND_LINES.append(format_log_line(message, *args))


def trace_label_color(label):
    if label.startswith("case-"):
        return "test"
    return TRACE_LABEL_COLORS.get(label, "info")


def trace_field(label, value=None):
    field = "[%*s]:" % (TRACE_LABEL_WIDTH, label)
    field = colorize(field, trace_label_color(label))
    if value is None:
        return field
    return "%s %s" % (field, value)


def client_actor_for(label):
    global NEXT_CLIENT_ACTOR_INDEX

    if label not in CLIENT_ACTORS:
        index = NEXT_CLIENT_ACTOR_INDEX
        NEXT_CLIENT_ACTOR_INDEX += 1
        if index < 26:
            suffix = chr(ord("A") + index)
        else:
            suffix = str(index + 1)
        CLIENT_ACTORS[label] = (
            "client-%s" % suffix,
            CLIENT_ACTOR_COLOR_NAMES[index % len(CLIENT_ACTOR_COLOR_NAMES)],
        )
    return CLIENT_ACTORS[label]


def format_client_actor(label):
    actor, color = client_actor_for(label)
    return "%s(%s)" % (colorize(actor, color), label)


def client_actor_color(label):
    return client_actor_for(label)[1]


def output_section_name(name):
    labels = {
        "payload": "input",
        "matched": "match",
        "output": "received",
    }
    return labels.get(name, name)


def command_trace(label, value=None):
    command_log("%s", trace_field(label, value))


def output_section_marker(name, actor_color=None):
    marker = "<%s>" % name
    if actor_color is None:
        return marker
    return colorize(marker, actor_color)


def command_trace_output_sections(sections, actor_color=None):
    command_trace("output")
    for name, text in sections:
        command_log("%s%s", TRACE_CONTINUATION, output_section_marker(name, actor_color))
        for line in display_payload_lines(text):
            command_log("%s%s", OUTPUT_CONTINUATION, line)


def command_lines_have_ko():
    return any("result" in line and "KO" in line for line in COMMAND_LINES)


def flush_commands():
    if VERBOSE or not COMMAND_LINES:
        return
    LOGGER.info("  trace:")
    for line in COMMAND_LINES:
        if line == "":
            LOGGER.info("")
        else:
            LOGGER.info("    %s", line)


def flush_details():
    if VERBOSE or not DETAIL_LINES:
        return
    if FAILURE_DETAIL_LINES == 0:
        LOGGER.info("  failure details: hidden; use --verbose for full trace")
        return
    lines = DETAIL_LINES
    server_start = []
    if len(lines) > FAILURE_DETAIL_LINES:
        for line in lines:
            if "[SERVER] start:" in line:
                server_start = [line]
                break
        tail = lines[-FAILURE_DETAIL_LINES:]
        omitted = len(lines) - len(tail) - len(server_start)
        if omitted > 0:
            lines = (
                server_start
                + ["... %d earlier detail line(s) omitted ..." % omitted]
                + tail
            )
        else:
            lines = server_start + tail
        LOGGER.info(
            "  failure details: last %d line(s); use --verbose for full trace",
            FAILURE_DETAIL_LINES,
        )
    else:
        LOGGER.info("  failure details:")
    for line in lines:
        if line == "":
            LOGGER.info("")
        else:
            LOGGER.info("    %s", line)


def unique_name(prefix):
    return "%s%d" % (prefix, random.randint(10000, 99999))


def format_command(command):
    return " ".join(shlex.quote(str(part)) for part in command)


def cat_e_payload(data):
    if not isinstance(data, str):
        data = data.decode("utf-8", errors="replace")
    if data == "":
        return []
    output = []
    line = []
    index = 0
    while index < len(data):
        char = data[index]
        if char == "\r":
            if index + 1 < len(data) and data[index + 1] == "\n":
                output.append("".join(line) + "^M$")
                line = []
                index += 2
                continue
            line.append("^M")
        elif char == "\n":
            output.append("".join(line) + "$")
            line = []
        elif char == "\t":
            line.append("\t")
        elif ord(char) < 32:
            line.append("^%s" % chr(ord(char) + 64))
        elif ord(char) == 127:
            line.append("^?")
        else:
            line.append(char)
        index += 1
    if line:
        output.append("".join(line))
    return output


def truncate_payload_line(line):
    if len(line) <= PAYLOAD_LINE_WIDTH:
        return line
    return "%s ... <%d char(s) omitted>" % (
        line[:PAYLOAD_LINE_WIDTH],
        len(line) - PAYLOAD_LINE_WIDTH,
    )


def display_payload_lines(data):
    lines = cat_e_payload(data)
    visible = lines[:PAYLOAD_LINE_LIMIT]
    output = [truncate_payload_line(line) for line in visible]
    if len(lines) > PAYLOAD_LINE_LIMIT:
        output.append(
            "... <%d more line(s) omitted>" % (len(lines) - PAYLOAD_LINE_LIMIT)
        )
    return output


def print_payload(name, text, actor_color=None):
    print_output_sections([(output_section_name(name), text)], actor_color)


def print_output_sections(sections, actor_color=None):
    detail_log("  %s", trace_field("output"))
    for name, text in sections:
        detail_log(
            "  %s%s",
            TRACE_CONTINUATION,
            output_section_marker(name, actor_color),
        )
        for line in display_payload_lines(text):
            detail_log("  %s%s", OUTPUT_CONTINUATION, line)
    command_trace_output_sections(sections, actor_color)


def start_test_log(name):
    global CLIENT_ACTORS, COMMAND_LINES, CURRENT_TEST_NAME, CURRENT_CASE_INDEX
    global DETAIL_LINES, NEXT_CLIENT_ACTOR_INDEX

    CURRENT_TEST_NAME = name
    CURRENT_CASE_INDEX = 0
    DETAIL_LINES = []
    COMMAND_LINES = []
    CLIENT_ACTORS = {}
    NEXT_CLIENT_ACTOR_INDEX = 0


def log_case(title):
    global CURRENT_CASE_INDEX

    CURRENT_CASE_INDEX += 1
    case_label = "case-%02d" % CURRENT_CASE_INDEX
    detail_log("")
    detail_log("  %s", trace_field(case_label, title))
    command_log("")
    command_trace(case_label, title)


def log_command(command):
    shell_command = "$ %s" % command
    log_command_value(shell_command)


def log_command_value(command):
    detail_log("  %s", trace_field("cmd", command))
    command_trace("cmd", command)


def log_client_command(actor, command):
    log_command_value("%s: $ %s" % (actor, command))


def log_expect(message):
    detail_log("  %s", trace_field("expect", message))
    command_trace("expect", message)


def result_value(ok, message):
    status = "OK" if ok else "KO"
    color = "ok" if ok else "fail"
    return "%s %s" % (colorize("[%s]" % status, color), message)


def log_result(message, ok=True):
    value = result_value(ok, message)
    detail_log("  %s", trace_field("result", value))
    command_trace("result", value)


def print_block(prefix, text):
    if not text:
        return
    normalized = text.replace("\r\n", "\n").replace("\r", "\n")
    for line in normalized.split("\n"):
        if line:
            detail_log("%s%s", prefix, line)
        else:
            detail_log("%s<blank line>", prefix)


def get_free_port():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((HOST, 0))
        return sock.getsockname()[1]
    finally:
        sock.close()


class IrcServer(object):
    def __init__(self, binary, password, wrapper, require_valgrind_clean):
        self.binary = binary
        self.password = password
        self.wrapper = wrapper
        self.require_valgrind_clean = require_valgrind_clean
        self.port = get_free_port()
        self.proc = None
        self.log_file = None
        self.log_path = None

    def __enter__(self):
        fd, self.log_path = tempfile.mkstemp(prefix="ft_irc_server_", suffix=".log")
        self.log_file = os.fdopen(fd, "w+b")
        command = self.wrapper + [self.binary, str(self.port), self.password]
        detail_log("%s start: %s", tag("SERVER", "server"), format_command(command))
        self.proc = subprocess.Popen(
            command,
            stdout=self.log_file,
            stderr=subprocess.STDOUT,
        )
        try:
            self._wait_until_listening()
            detail_log("%s listening: %s:%d", tag("SERVER", "server"), HOST, self.port)
        except Exception:
            self._stop_process()
            if self.log_file is not None:
                self.log_file.close()
            self._print_log()
            raise
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._stop_process()
        if self.log_file is not None:
            self.log_file.close()
        if self.log_path is not None:
            self._print_log()
        if exc_type is None and self.require_valgrind_clean:
            self._assert_valgrind_clean()
        return False

    def _wait_until_listening(self):
        deadline = time.time() + DEFAULT_TIMEOUT
        last_error = None
        while time.time() < deadline:
            if self.proc.poll() is not None:
                fail("server exited before accepting connections")
            try:
                probe_sock = socket.create_connection((HOST, self.port), timeout=0.2)
                probe_sock.close()
                time.sleep(0.05)
                if self.proc.poll() is not None:
                    fail("server exited after a probe connection closed")
                return
            except socket.error as err:
                last_error = err
                time.sleep(0.05)
        fail("server did not listen on %s:%d: %s" % (HOST, self.port, last_error))

    def _stop_process(self):
        if self.proc is not None and self.proc.poll() is None:
            detail_log("%s stop: pid=%d", tag("SERVER", "server"), self.proc.pid)
            self.proc.terminate()
            try:
                self.proc.wait(timeout=2.0)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait(timeout=2.0)

    def _print_log(self):
        if self.log_path is None:
            return
        try:
            with open(self.log_path, "r", errors="replace") as handle:
                log_text = handle.read()
        except IOError:
            return
        detail_log("")
        detail_log("--- server output: %s ---", self.log_path)
        print_output_sections([("server stdout+stderr", log_text)], "server")

    def _assert_valgrind_clean(self):
        try:
            with open(self.log_path, "r", errors="replace") as handle:
                log = handle.read()
        except IOError as err:
            fail("could not read valgrind log: %s" % err)

        if "All heap blocks were freed -- no leaks are possible" not in log:
            required_zero_leaks = [
                "definitely lost",
                "indirectly lost",
                "possibly lost",
            ]
            for leak_kind in required_zero_leaks:
                pattern = r"%s:\s+0 bytes" % re.escape(leak_kind)
                if not re.search(pattern, log):
                    fail("valgrind reported %s. Log: %s" % (leak_kind, self.log_path))
        if not re.search(r"ERROR SUMMARY:\s+0 errors", log):
            fail("valgrind reported memory errors. Log: %s" % self.log_path)

        fd_summary = re.search(
            r"FILE DESCRIPTORS:\s+(\d+)\s+open(?:\s+\((\d+)\s+std\))?\s+at exit",
            log,
        )
        if fd_summary is None:
            fail("valgrind fd summary missing; keep --track-fds=yes enabled")
        open_fds = int(fd_summary.group(1))
        std_fds = int(fd_summary.group(2) or "0")
        if open_fds > std_fds:
            fail(
                "valgrind reported %d non-stdio fd(s) still open. Log: %s"
                % (open_fds - std_fds, self.log_path)
            )


class IrcClient(object):
    def __init__(self, port, label):
        self.label = label
        self.display_label = label
        self.actor_name, self.actor_color_name = client_actor_for(label)
        self.sock = socket.create_connection((HOST, port), timeout=DEFAULT_TIMEOUT)
        self.sock.settimeout(0.1)
        self.buffer = ""
        self.cursor = 0
        self.output_cursor = 0
        detail_log(
            "%s %s connect %s:%d",
            tag("CLIENT", "client"),
            self.actor(),
            HOST,
            port,
        )

    def actor(self):
        return "%s(%s)" % (
            colorize(self.actor_name, self.actor_color_name),
            self.display_label,
        )

    def actor_plain(self):
        return "%s(%s)" % (self.actor_name, self.display_label)

    def actor_color(self):
        return self.actor_color_name

    def close(self):
        detail_log("%s %s close", tag("CLIENT", "client"), self.actor())
        try:
            self.sock.close()
        except socket.error:
            pass

    def abortive_close(self):
        detail_log("%s %s abortive close", tag("CLIENT", "client"), self.actor())
        try:
            self.sock.setsockopt(
                socket.SOL_SOCKET,
                socket.SO_LINGER,
                struct.pack("ii", 1, 0),
            )
        except socket.error:
            pass
        self.close()

    def send_raw(self, data, should_log_input=True):
        if should_log_input:
            log_case("%s raw fragment" % self.actor())
            log_client_command(self.actor(), "<raw-bytes>")
            print_payload("payload", data, self.actor_color())
        if isinstance(data, str):
            data = data.encode("utf-8")
        self.sock.sendall(data)

    def send_cmd(self, line):
        self._update_display_label_from_command(line)
        log_case("%s command" % self.actor())
        log_client_command(self.actor(), shlex.quote(line + "\\r\\n"))
        self.send_raw(line + "\r\n", should_log_input=False)

    def _update_display_label_from_command(self, line):
        parts = line.strip().split()
        if len(parts) < 2 or parts[0].upper() != "NICK":
            return
        nick = parts[1]
        if nick.startswith(":"):
            nick = nick[1:]
        if nick:
            self.display_label = nick

    def wait_for_code(self, code, timeout=DEFAULT_TIMEOUT):
        pattern = re.compile(r"(^|[\r\n]+):?[^ \r\n]*\s+%s\s+" % re.escape(code))
        return self.wait_for_regex(
            pattern,
            timeout,
            "receive numeric %s" % code,
            match_summary="numeric %s" % code,
            success_summary="matched numeric %s" % code,
            failure_summary="before missing numeric %s" % code,
        )

    def wait_for_any_code(self, codes, timeout=DEFAULT_TIMEOUT):
        pattern = re.compile(
            r"(^|[\r\n]+):?[^ \r\n]*\s+(%s)\s+"
            % "|".join(re.escape(c) for c in codes)
        )
        return self.wait_for_regex(
            pattern,
            timeout,
            "receive one of numerics %s" % ", ".join(codes),
            match_summary=lambda match: "numeric %s" % match.group(2),
            success_summary="matched one of numerics %s" % ", ".join(codes),
            failure_summary="before missing one of numerics %s" % ", ".join(codes),
        )

    def wait_for_text(self, text, timeout=DEFAULT_TIMEOUT):
        log_expect("%s should receive text %r" % (self.actor(), text))
        deadline = time.time() + timeout
        while True:
            index = self.buffer.find(text, self.cursor)
            if index != -1:
                self.cursor = index + len(text)
                self._print_new_result("matched text %r" % text, "text %r" % text)
                return self.buffer
            if time.time() >= deadline:
                break
            self._read_once(deadline)
        self._print_new_result("before missing text %r" % text, ok=False)
        fail(
            "%s did not receive text %r. Buffer:\n%s"
            % (self.label, text, self.buffer)
        )

    def wait_for_regex(
        self,
        pattern,
        timeout=DEFAULT_TIMEOUT,
        expectation=None,
        match_summary=None,
        success_summary=None,
        failure_summary=None,
    ):
        if isinstance(pattern, str):
            pattern = re.compile(pattern)
        if expectation is None:
            expectation = "match pattern %r" % pattern.pattern
        log_expect("%s should %s" % (self.actor(), expectation))
        deadline = time.time() + timeout
        while True:
            unread = self.buffer[self.cursor :]
            match = pattern.search(unread)
            if match:
                self.cursor += match.end()
                if callable(match_summary):
                    matched = match_summary(match)
                elif match_summary is not None:
                    matched = match_summary
                else:
                    matched = match.group(0)
                self._print_new_result(
                    success_summary or "matched pattern %r" % pattern.pattern,
                    matched,
                )
                return self.buffer
            if time.time() >= deadline:
                break
            self._read_once(deadline)
        self._print_new_result(
            failure_summary or "before missing pattern %r" % pattern.pattern,
            ok=False,
        )
        fail(
            "%s did not receive pattern %r. Buffer:\n%s"
            % (self.label, pattern.pattern, self.buffer)
        )

    def drain(self, timeout=0.2):
        deadline = time.time() + timeout
        while time.time() < deadline:
            previous = self.buffer
            self._read_once(deadline)
            if self.buffer == previous:
                break
        self.cursor = len(self.buffer)
        self._print_new_result("drain")
        return self.buffer

    def expect_no_new_data(self, timeout=0.5):
        log_expect("%s should receive no new data" % self.actor())
        start = len(self.buffer)
        deadline = time.time() + timeout
        while time.time() < deadline:
            self._read_once(deadline)
        if len(self.buffer) != start:
            self._print_new_result("unexpected data", ok=False)
            fail(
                "%s received unexpected data:\n%s"
                % (self.label, self.buffer[start:])
            )
        self.cursor = len(self.buffer)
        log_result("%s no new data" % self.actor())

    def expect_no_regex(self, pattern, timeout=0.5, expectation=None):
        if isinstance(pattern, str):
            pattern = re.compile(pattern)
        if expectation is None:
            expectation = "not match pattern %r" % pattern.pattern
        log_expect("%s should %s" % (self.actor(), expectation))
        deadline = time.time() + timeout
        start = len(self.buffer)
        while True:
            data = self.buffer[start:]
            if pattern.search(data):
                self._print_new_result(
                    "unexpected pattern %r" % pattern.pattern,
                    ok=False,
                )
                fail(
                    "%s unexpectedly received pattern %r. New data:\n%s"
                    % (self.label, pattern.pattern, data)
                )
            if time.time() >= deadline:
                break
            self._read_once(deadline)
        self.cursor = len(self.buffer)
        self._print_new_result("no unexpected data")

    def _read_once(self, deadline):
        remaining = max(0.01, min(0.1, deadline - time.time()))
        self.sock.settimeout(remaining)
        try:
            chunk = self.sock.recv(4096)
            if not chunk:
                fail("%s connection closed. Buffer:\n%s" % (self.label, self.buffer))
            self.buffer += chunk.decode("utf-8", errors="replace")
        except socket.timeout:
            pass

    def _print_new_result(self, reason, matched=None, ok=True):
        data = self.buffer[self.output_cursor:]
        sections = []
        if matched is not None:
            sections.append(("match", matched))
        sections.append(("%s received" % self.actor_plain(), data))
        print_output_sections(sections, self.actor_color())
        log_result("%s %s" % (self.actor(), reason), ok=ok)
        self.output_cursor = len(self.buffer)


def require_success(process, description):
    if process.returncode != 0:
        fail(
            "%s failed with exit code %d\nstdout:\n%s\nstderr:\n%s"
            % (description, process.returncode, process.stdout, process.stderr)
        )


def require_failure(process, description):
    if process.returncode == 0:
        fail("%s unexpectedly succeeded" % description)


def is_rfc_profile(args):
    return args.profile == "rfc"


def print_process_output(process, description, ok):
    print_output_sections(
        [
            ("stdout", process.stdout),
            ("stderr", process.stderr),
        ]
    )
    detail_log("  %s", trace_field("exit", process.returncode))
    command_trace("exit", process.returncode)
    log_result("%s exit=%d" % (description, process.returncode), ok=ok)


def run_argument_case(command, description):
    log_case(description)
    log_command(format_command(command))
    log_expect("exit non-zero within 1.00s")
    try:
        process = subprocess.run(
            command,
            text=True,
            capture_output=True,
            timeout=1.0,
        )
    except subprocess.TimeoutExpired:
        log_result("%s timed out" % description, ok=False)
        fail("%s did not exit quickly" % description)
    ok = process.returncode != 0
    print_process_output(process, description, ok)
    if not ok:
        require_failure(process, description)


def run_occupied_port_case(args):
    blocker = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        blocker.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        blocker.bind((HOST, 0))
        blocker.listen(1)
        port = blocker.getsockname()[1]
        command = args.server_wrapper + [args.binary, str(port), PASSWORD]
        run_argument_case(command, "occupied port validation")
    finally:
        blocker.close()


def connect_client(server, label):
    return IrcClient(server.port, label)


def run_server(args):
    return IrcServer(
        args.binary,
        PASSWORD,
        args.server_wrapper,
        args.require_valgrind_clean,
    )


def require_server_alive(server, description):
    if server.proc is not None and server.proc.poll() is not None:
        fail("%s: server exited with status %d" % (description, server.proc.returncode))
    log_result("%s server still running" % description)


def register_client(server, nick):
    client = connect_client(server, nick)
    client.send_cmd("PASS %s" % PASSWORD)
    client.send_cmd("NICK %s" % nick)
    client.send_cmd("USER %s 0 * :%s" % (nick, nick))
    client.wait_for_code("001")
    client.drain()
    return client


def expect_join(client, channel):
    client.wait_for_regex(
        r"\bJOIN\b\s+:?%s([\r\n]+|$)" % re.escape(channel),
        expectation="receive JOIN for %s" % channel,
        match_summary="JOIN %s" % channel,
        success_summary="matched JOIN for %s" % channel,
        failure_summary="before missing JOIN for %s" % channel,
    )


def expect_privmsg(client, target, message):
    client.wait_for_regex(
        r"\bPRIVMSG\b\s+%s\s+:%s([\r\n]+|$)"
        % (re.escape(target), re.escape(message)),
        expectation="receive PRIVMSG to %s with text %r" % (target, message),
        match_summary="PRIVMSG %s :%s" % (target, message),
        success_summary="matched PRIVMSG to %s" % target,
        failure_summary="before missing PRIVMSG to %s" % target,
    )


def join_channel(client, channel, key=None):
    if key is None:
        client.send_cmd("JOIN %s" % channel)
    else:
        client.send_cmd("JOIN %s %s" % (channel, key))
    expect_join(client, channel)


def send_channel_message(sender, receiver, channel, message):
    sender.send_cmd("PRIVMSG %s :%s" % (channel, message))
    expect_privmsg(receiver, channel, message)


def close_clients(*clients):
    for client in clients:
        if client is not None:
            client.close()


def expect_no_join(client, channel):
    client.expect_no_regex(
        r"\bJOIN\b\s+:?%s([\r\n]+|$)" % re.escape(channel),
        expectation="not receive JOIN for %s" % channel,
    )


def expect_no_privmsg(client, target, message):
    client.expect_no_regex(
        r"\bPRIVMSG\b\s+%s\s+:%s([\r\n]+|$)"
        % (re.escape(target), re.escape(message)),
        expectation="not receive PRIVMSG to %s with text %r" % (target, message),
    )


def expect_no_privmsg_text(client, message):
    client.expect_no_regex(
        r"\bPRIVMSG\b[^\r\n]*:%s([\r\n]+|$)" % re.escape(message),
        expectation="not receive any PRIVMSG with text %r" % message,
    )


def wait_for_buffer_text(client, text, timeout=DEFAULT_TIMEOUT, description=None):
    if description is None:
        description = "receive text %r" % text
    log_expect("%s should %s" % (client.actor(), description))
    deadline = time.time() + timeout
    while True:
        index = client.buffer.find(text, client.cursor)
        if index != -1:
            client.cursor = index + len(text)
            log_result("%s matched %s" % (client.actor(), description))
            return
        if time.time() >= deadline:
            break
        client._read_once(deadline)
    log_result("%s missing %s" % (client.actor(), description), ok=False)
    fail(
        "%s did not receive %s. Buffer tail:\n%s"
        % (client.label, description, client.buffer[-1024:])
    )


def drain_without_printing_payload(client, reason, timeout=0.2):
    deadline = time.time() + timeout
    hidden_start = client.output_cursor
    while time.time() < deadline:
        previous = client.buffer
        client._read_once(deadline)
        if client.buffer == previous:
            break
    client.cursor = len(client.buffer)
    client.output_cursor = len(client.buffer)
    log_result(
        "%s %s (%d byte(s) hidden)"
        % (client.actor(), reason, len(client.buffer) - hidden_start)
    )


class TestDefinition(object):
    def __init__(self, name, title, summary, checks, func):
        self.name = name
        self.title = title
        self.summary = summary
        self.checks = checks
        self.func = func

    def label(self):
        return "%s %s" % (self.name, self.title)


TESTS = []
TEST_BY_NAME = {}


def test_order_key(test):
    match = re.match(r"^test-(\d+)$", test.name)
    if match:
        return (0, int(match.group(1)), test.name)
    return (1, 0, test.name)


def ordered_tests():
    return sorted(TESTS, key=test_order_key)


def is_rfc_check(check):
    return check.startswith("rfc: ")


def format_check(check):
    if is_rfc_check(check):
        return check[len("rfc: ") :]
    return check


def profile_checks(test, profile):
    if profile == "rfc":
        return [format_check(check) for check in test.checks]
    return [format_check(check) for check in test.checks if not is_rfc_check(check)]


def test_case(name, title, summary, checks):
    def register(func):
        if name in TEST_BY_NAME:
            raise RuntimeError("duplicate test case: %s" % name)
        if not checks:
            raise RuntimeError("test case %s must list at least one check" % name)
        test = TestDefinition(name, title, summary, checks, func)
        TESTS.append(test)
        TEST_BY_NAME[name] = test
        return func

    return register


@test_case(
    "test-1",
    "argument checks",
    "Invalid CLI arguments must fail quickly before the server starts.",
    [
        "missing arguments",
        "non-numeric port",
        "numeric port with trailing text",
        "out-of-range port",
        "rfc: empty password",
    ],
)
def test_argument_validation(args):
    run_argument_case([args.binary], "missing argument validation")
    run_argument_case(
        [args.binary, "not-a-port", PASSWORD],
        "non-numeric port validation",
    )
    run_argument_case(
        [args.binary, "4242a", PASSWORD],
        "numeric port with trailing text validation",
    )
    run_argument_case([args.binary, "70000", PASSWORD], "out-of-range port validation")
    if is_rfc_profile(args):
        run_argument_case([args.binary, "6667", ""], "empty password validation")


@test_case(
    "transport-1",
    "transport-only socket lifecycle",
    "Socket accept, raw reads, mixed closes, and reconnect work without IRC replies.",
    [
        "multiple clients can connect without registration replies",
        "raw coalesced and fragmented input does not crash the server",
        "large unfinished input does not block reconnect",
        "mixed normal and abortive closes leave the server accepting clients",
    ],
)
def test_transport_only_socket_lifecycle(args):
    with run_server(args) as server:
        clients = []
        try:
            for index in range(12):
                clients.append(connect_client(server, "raw%d" % index))

            clients[0].send_raw("PASS %s\r\nNICK raw0\r\nUSER raw0 0 * :raw0\r\n" % PASSWORD)
            clients[1].send_raw("PASS %s\r" % PASSWORD)
            time.sleep(0.05)
            clients[1].send_raw("\nNICK raw1\r\nUSER raw1 0 * :raw1\r\n")
            clients[2].send_raw("X" * 4096, should_log_input=False)

            for index in range(3, len(clients)):
                clients[index].send_raw(
                    "PASS %s\r\nNICK raw%d\r\nUSER raw%d 0 * :raw%d\r\n"
                    % (PASSWORD, index, index, index)
                )

            time.sleep(0.2)
            require_server_alive(server, "raw transport traffic")

            for index, client in enumerate(clients):
                if index % 2 == 0:
                    client.close()
                else:
                    client.abortive_close()
            clients = []
            time.sleep(0.2)
            require_server_alive(server, "mixed transport closes")

            newcomer = connect_client(server, "raw-reconnect")
            newcomer.send_raw("PASS %s\r\n" % PASSWORD)
            newcomer.close()
        finally:
            for client in clients:
                client.close()


@test_case(
    "transport-2",
    "transport-only bind failure",
    "Starting on an occupied port exits quickly with non-zero status.",
    [
        "occupied listen port does not enter the event loop",
        "bind/listen failure is reported by process exit status",
    ],
)
def test_transport_only_bind_failure(args):
    run_occupied_port_case(args)


@test_case(
    "test-2",
    "registration and non-blocking",
    "PASS/NICK/USER registration and basic non-blocking behavior.",
    [
        "rfc: unregistered command rejection",
        "rfc: wrong password and missing parameter errors",
        "successful registration numeric 001",
        "rfc: duplicate nickname rejection",
        "idle client does not block an active client",
    ],
)
def test_registration_and_nonblocking(args):
    with run_server(args) as server:
        idle = connect_client(server, "idle")
        active = connect_client(server, "active")

        if is_rfc_profile(args):
            active.send_cmd("JOIN #blocked")
            active.wait_for_code("451")

            active.send_cmd("PASS")
            active.wait_for_code("461")

            active.send_cmd("PASS wrong")
            active.wait_for_code("464")

        nick = unique_name("reg")
        active.send_cmd("PASS %s" % PASSWORD)
        active.send_cmd("NICK %s" % nick)
        active.send_cmd("USER %s 0 * :Registered User" % nick)
        active.wait_for_code("001")

        if is_rfc_profile(args):
            active.send_cmd("PASS %s" % PASSWORD)
            active.wait_for_code("462")

        duplicate = connect_client(server, "duplicate")
        duplicate.send_cmd("PASS %s" % PASSWORD)
        duplicate.send_cmd("NICK %s" % nick)
        if is_rfc_profile(args):
            duplicate.wait_for_code("433")
        else:
            duplicate.drain()

        idle.close()
        active.close()
        duplicate.close()


@test_case(
    "test-3",
    "fragmented registration",
    "TCP-fragmented PASS/NICK/USER input is buffered and reassembled.",
    [
        "partial command bytes are not dispatched early",
        "split registration commands still produce numeric 001",
    ],
)
def test_fragmented_registration(args):
    with run_server(args) as server:
        nick = unique_name("frag")
        client = connect_client(server, nick)
        client.send_raw("PA")
        time.sleep(0.1)
        client.send_raw("SS %s\r\nNI" % PASSWORD)
        time.sleep(0.1)
        client.send_raw("CK %s\r\nUSER %s 0 * :Fragmented User\r\n" % (nick, nick))
        client.wait_for_code("001")
        client.close()


@test_case(
    "test-6",
    "HUP and disconnect cleanup",
    "Client disconnect and HUP cleanup keeps the server usable.",
    [
        "normal close",
        "abortive close",
        "partial-command abortive close",
        "partial-command normal close",
        "rfc: channel membership cleanup after disconnect",
        "new clients can still connect and exchange messages",
    ],
)
def test_hup_disconnect_cleanup(args):
    with run_server(args) as server:
        for index in range(5):
            idle = connect_client(server, "idle%d" % index)
            if index % 2 == 0:
                idle.abortive_close()
            else:
                idle.close()
            time.sleep(0.05)

        survivor = register_client(server, unique_name("survivor"))
        survivor.close()

        partial = connect_client(server, "partial")
        partial.send_raw("PASS pa")
        partial.abortive_close()
        time.sleep(0.1)

        partial_fin = connect_client(server, "partial-fin")
        partial_fin.send_raw("PASS pa")
        partial_fin.close()
        time.sleep(0.1)
        require_server_alive(server, "partial command normal close")

        if not is_rfc_profile(args):
            newcomer = register_client(server, unique_name("newcomer"))
            newcomer.close()
            return

        channel = "#hup%d" % random.randint(10000, 99999)
        watcher = register_client(server, unique_name("watcher"))
        quitter = register_client(server, unique_name("quitter"))
        watcher.send_cmd("JOIN %s" % channel)
        expect_join(watcher, channel)
        quitter.send_cmd("JOIN %s" % channel)
        expect_join(quitter, channel)
        watcher.wait_for_regex(r"\bJOIN\b\s+:?%s([\r\n]+|$)" % re.escape(channel))

        quitter.abortive_close()
        time.sleep(0.2)

        newcomer = register_client(server, unique_name("newcomer"))
        newcomer.send_cmd("JOIN %s" % channel)
        expect_join(newcomer, channel)

        message = "server survived hup cleanup"
        watcher.send_cmd("PRIVMSG %s :%s" % (channel, message))
        expect_privmsg(newcomer, channel, message)

        watcher.close()
        newcomer.close()


@test_case(
    "test-4",
    "empty and malformed input",
    "Empty lines, whitespace-only lines, and malformed commands are safe.",
    [
        "blank input does not crash",
        "rfc: whitespace-only input does not emit unexpected data",
        "rfc: missing PASS/NICK/USER/JOIN/PRIVMSG parameters return errors",
        "rfc: empty PRIVMSG body returns an error",
    ],
)
def test_empty_and_malformed_inputs(args):
    with run_server(args) as server:
        blank = connect_client(server, "blank")
        blank.drain()
        blank.send_raw("\r\n\r\n   \r\n")
        if is_rfc_profile(args):
            blank.expect_no_new_data()
        else:
            blank.drain()
        blank.send_cmd("PASS %s" % PASSWORD)
        blank.send_cmd("NICK %s" % unique_name("blank"))
        blank.send_cmd("USER blank 0 * :Blank Line User")
        blank.wait_for_code("001")
        blank.close()

        if not is_rfc_profile(args):
            return

        malformed = connect_client(server, "malformed")
        malformed.send_cmd("PASS :")
        malformed.wait_for_code("464")
        malformed.send_cmd("NICK :")
        malformed.wait_for_code("431")
        malformed.send_cmd("USER :")
        malformed.wait_for_code("461")
        malformed.close()

        nick = unique_name("edge")
        registered = register_client(server, nick)
        registered.send_cmd("JOIN")
        registered.wait_for_code("461")
        registered.send_cmd("PRIVMSG")
        registered.wait_for_any_code(["411", "461"])
        registered.send_cmd("PRIVMSG :hello")
        registered.wait_for_any_code(["411", "461"])

        channel = "#empty%d" % random.randint(10000, 99999)
        registered.send_cmd("JOIN %s" % channel)
        expect_join(registered, channel)
        registered.send_cmd("PRIVMSG %s :" % channel)
        registered.wait_for_code("412")
        registered.close()


@test_case(
    "test-5",
    "protocol edge cases",
    "Parser and protocol edge cases around IRC command syntax.",
    [
        "rfc: lowercase commands",
        "rfc: multiple spaces between parameters",
        "rfc: unknown command",
        "rfc: invalid channel name",
        "rfc: unknown target nickname",
        "trailing message text preserves spaces",
    ],
)
def test_protocol_edge_cases(args):
    with run_server(args) as server:
        nick = unique_name("edge")
        client = connect_client(server, nick)
        if is_rfc_profile(args):
            client.send_cmd("pass    %s" % PASSWORD)
            client.send_cmd("nick   %s" % nick)
            client.send_cmd("user   %s 0 * :Edge Case User" % nick)
        else:
            client.send_cmd("PASS %s" % PASSWORD)
            client.send_cmd("NICK %s" % nick)
            client.send_cmd("USER %s 0 * :Edge Case User" % nick)
        client.wait_for_code("001")
        client.drain()

        if is_rfc_profile(args):
            client.send_cmd("DOESNOTEXIST")
            client.wait_for_code("421")

            client.send_cmd("JOIN room")
            client.wait_for_any_code(["403", "476"])

            client.send_cmd("PRIVMSG ghost%d :hello" % random.randint(10000, 99999))
            client.wait_for_code("401")

        channel = "#edge%d" % random.randint(10000, 99999)
        receiver = register_client(server, unique_name("recv"))
        join_channel(client, channel)
        join_channel(receiver, channel)

        message = "hello   with   preserved   spaces"
        send_channel_message(client, receiver, channel, message)

        close_clients(client, receiver)


@test_case(
    "test-9",
    "JOIN and PRIVMSG",
    "JOIN and PRIVMSG delivery for channels and direct messages.",
    [
        "channel JOIN visibility",
        "channel message reaches other members",
        "direct message reaches the target nickname",
        "direct message is not broadcast to unrelated clients",
        "rfc: non-member channel send is rejected",
    ],
)
def test_join_and_privmsg(args):
    with run_server(args) as server:
        channel = "#room%d" % random.randint(10000, 99999)
        alice = register_client(server, unique_name("alice"))
        bob = register_client(server, unique_name("bob"))
        observer = register_client(server, unique_name("observer"))

        join_channel(alice, channel)

        join_channel(bob, channel)
        alice.wait_for_regex(r"\bJOIN\b\s+:?%s([\r\n]+|$)" % re.escape(channel))

        message = "hello channel"
        send_channel_message(alice, bob, channel, message)

        direct = "hello user"
        alice.send_cmd("PRIVMSG %s :%s" % (bob.label, direct))
        expect_privmsg(bob, bob.label, direct)
        expect_no_privmsg_text(observer, direct)

        if is_rfc_profile(args):
            charlie = register_client(server, unique_name("charlie"))
            charlie.send_cmd("PRIVMSG %s :not a member" % channel)
            charlie.wait_for_any_code(["404", "442"])
            close_clients(charlie)

        close_clients(alice, bob, observer)


@test_case(
    "test-10",
    "two-client roundtrip",
    "Two registered clients exchange messages in both directions.",
    [
        "Alice to Bob through channel",
        "Bob to Alice through channel",
        "Alice to Bob direct message",
        "Bob to Alice direct message",
    ],
)
def test_two_client_roundtrip(args):
    with run_server(args) as server:
        channel = "#round%d" % random.randint(10000, 99999)
        alice = register_client(server, unique_name("alice"))
        bob = register_client(server, unique_name("bob"))

        join_channel(alice, channel)
        join_channel(bob, channel)
        alice.wait_for_regex(r"\bJOIN\b\s+:?%s([\r\n]+|$)" % re.escape(channel))

        alice_to_bob_channel = "alice to bob through channel"
        send_channel_message(alice, bob, channel, alice_to_bob_channel)

        bob_to_alice_channel = "bob to alice through channel"
        send_channel_message(bob, alice, channel, bob_to_alice_channel)

        alice_to_bob_direct = "alice direct to bob"
        alice.send_cmd("PRIVMSG %s :%s" % (bob.label, alice_to_bob_direct))
        expect_privmsg(bob, bob.label, alice_to_bob_direct)

        bob_to_alice_direct = "bob direct to alice"
        bob.send_cmd("PRIVMSG %s :%s" % (alice.label, bob_to_alice_direct))
        expect_privmsg(alice, alice.label, bob_to_alice_direct)

        close_clients(alice, bob)


@test_case(
    "test-11",
    "operator commands and modes",
    "Mandatory operator commands and channel modes.",
    [
        "KICK operator permission",
        "fragmented KICK command is buffered and mutates membership",
        "INVITE with +i",
        "TOPIC with +t",
        "MODE +k, +l, +o",
        "rfc: mode removal and exact error enforcement after state changes",
    ],
)
def test_operator_commands_and_modes(args):
    with run_server(args) as server:
        channel = "#ops%d" % random.randint(10000, 99999)
        op = register_client(server, unique_name("op"))
        user = register_client(server, unique_name("user"))
        guest = register_client(server, unique_name("guest"))

        join_channel(op, channel)
        join_channel(user, channel)

        if is_rfc_profile(args):
            user.send_cmd("KICK %s %s :nope" % (channel, op.label))
            user.wait_for_code("482")

        kick_line = "KICK %s %s :bye" % (channel, user.label)
        log_case("%s fragmented KICK command" % op.actor())
        log_client_command(op.actor(), "<fragmented KICK command>")
        op.send_raw(kick_line[:2], should_log_input=False)
        time.sleep(0.05)
        op.send_raw(kick_line[2:] + "\r", should_log_input=False)
        time.sleep(0.05)
        op.send_raw("\n", should_log_input=False)
        user.wait_for_regex(
            r"\bKICK\b\s+%s\s+%s\b"
            % (re.escape(channel), re.escape(user.label))
        )

        if is_rfc_profile(args):
            user.send_cmd("PRIVMSG %s :after kick" % channel)
            user.wait_for_any_code(["404", "442"])

        op.send_cmd("MODE %s +i" % channel)
        if is_rfc_profile(args):
            guest.send_cmd("JOIN %s" % channel)
            guest.wait_for_code("473")

        op.send_cmd("INVITE %s %s" % (guest.label, channel))
        guest.wait_for_regex(
            r"\bINVITE\b\s+%s\s+%s([\r\n]+|$)"
            % (re.escape(guest.label), re.escape(channel)),
            expectation="receive INVITE for %s" % channel,
            match_summary="INVITE %s %s" % (guest.label, channel),
            success_summary="matched INVITE for %s" % channel,
            failure_summary="before missing INVITE for %s" % channel,
        )
        join_channel(guest, channel)

        op.send_cmd("MODE %s +t" % channel)
        if is_rfc_profile(args):
            guest.send_cmd("TOPIC %s :blocked topic" % channel)
            guest.wait_for_code("482")

        topic = "topic by operator"
        op.send_cmd("TOPIC %s :%s" % (channel, topic))
        op.send_cmd("TOPIC %s" % channel)
        op.wait_for_text(topic)

        op.send_cmd("MODE %s -i" % channel)
        op.send_cmd("MODE %s +k secret" % channel)

        keyless = None
        if is_rfc_profile(args):
            keyless = register_client(server, unique_name("keyless"))
            keyless.send_cmd("JOIN %s" % channel)
            keyless.wait_for_code("475")
            keyless.send_cmd("JOIN %s wrong" % channel)
            keyless.wait_for_code("475")

        keyed = register_client(server, unique_name("keyed"))
        join_channel(keyed, channel, "secret")

        op.send_cmd("MODE %s +l 3" % channel)
        full = register_client(server, unique_name("full"))
        full.send_cmd("JOIN %s secret" % channel)
        if is_rfc_profile(args):
            full.wait_for_code("471")
        else:
            expect_no_join(full, channel)

        op.send_cmd("MODE %s +o %s" % (channel, guest.label))
        sync_message = "mode op sync %d" % random.randint(10000, 99999)
        send_channel_message(op, guest, channel, sync_message)
        guest.send_cmd("KICK %s %s :promoted op" % (channel, keyed.label))
        keyed.wait_for_regex(
            r"\bKICK\b\s+%s\s+%s\b"
            % (re.escape(channel), re.escape(keyed.label))
        )

        if is_rfc_profile(args):
            op.send_cmd("MODE %s -o %s" % (channel, guest.label))
            guest.send_cmd("KICK %s %s :deopped" % (channel, op.label))
            guest.wait_for_code("482")

        op.send_cmd("MODE %s -l" % channel)
        op.send_cmd("MODE %s -k secret" % channel)
        if is_rfc_profile(args) and full is not None:
            join_channel(full, channel)

        close_clients(op, user, guest, keyless, keyed, full)


@test_case(
    "test-12",
    "mass message stability",
    "A large PRIVMSG burst is delivered without crashing or wedging the server.",
    [
        "many channel messages are accepted in one burst",
        "receiver gets every message in order",
        "server remains alive after the burst",
        "new clients can still register after the burst",
    ],
)
def test_mass_message_stability(args):
    with run_server(args) as server:
        channel = "#mass%d" % random.randint(10000, 99999)
        sender = register_client(server, unique_name("sender"))
        receiver = register_client(server, unique_name("receiver"))
        after = None
        messages = []

        try:
            join_channel(sender, channel)
            join_channel(receiver, channel)
            sender.wait_for_regex(r"\bJOIN\b\s+:?%s([\r\n]+|$)" % re.escape(channel))
            receiver.drain()

            for index in range(300):
                messages.append(
                    "mass-%03d-%d-%s"
                    % (index, random.randint(10000, 99999), "x" * 64)
                )

            payload = "".join(
                "PRIVMSG %s :%s\r\n" % (channel, message)
                for message in messages
            )
            log_case("%s mass PRIVMSG burst" % sender.actor())
            log_client_command(
                sender.actor(),
                "<%d PRIVMSG commands, %d bytes>"
                % (len(messages), len(payload)),
            )
            sender.send_raw(payload, should_log_input=False)
            wait_for_buffer_text(
                receiver,
                messages[-1],
                timeout=max(DEFAULT_TIMEOUT, 5.0),
                description="receive final mass message",
            )

            position = 0
            for index, message in enumerate(messages):
                expected = "PRIVMSG %s :%s" % (channel, message)
                found = receiver.buffer.find(expected, position)
                if found == -1:
                    fail("mass message %d missing or out of order: %s" % (index, expected))
                position = found + len(expected)
            log_result("receiver saw %d mass messages in order" % len(messages))

            require_server_alive(server, "after mass message burst")
            after = register_client(server, unique_name("aftermass"))
            join_channel(after, channel)
        finally:
            close_clients(sender, receiver, after)


@test_case(
    "test-7",
    "transport hardening",
    "Connection bursts, fd reuse, and bind failures keep the transport reliable.",
    [
        "burst clients can connect before sending registration",
        "all burst clients receive numeric 001",
        "mixed normal and abortive closes still allow reconnect",
        "rfc: occupied port exits quickly with non-zero status",
    ],
)
def test_transport_hardening(args):
    if is_rfc_profile(args):
        run_occupied_port_case(args)

    with run_server(args) as server:
        clients = []
        try:
            for index in range(20):
                clients.append(connect_client(server, "burst%d" % index))

            for client in clients:
                nick = unique_name("burst")
                client.send_cmd("PASS %s" % PASSWORD)
                client.send_cmd("NICK %s" % nick)
                client.send_cmd("USER %s 0 * :%s" % (nick, nick))

            for client in clients:
                client.wait_for_code("001")
                client.drain()

            for index, client in enumerate(clients):
                if index % 2 == 0:
                    client.close()
                else:
                    client.abortive_close()
            clients = []
            time.sleep(0.2)

            newcomer = register_client(server, unique_name("reconnect"))
            newcomer.close()
        finally:
            for client in clients:
                client.close()


@test_case(
    "test-8",
    "command burst and buffering",
    "Coalesced commands, CRLF boundaries, and large unfinished input stay safe.",
    [
        "coalesced PASS/NICK/USER registration receives numeric 001",
        "multiple commands in one TCP read are all dispatched",
        "CRLF split across packets still completes commands",
        "partial command completes when the remaining bytes arrive",
        "partial PRIVMSG disconnect does not deliver a message",
        "long fragmented command is buffered until CRLF",
        "server remains usable after a very long command",
        "large unfinished input does not block another client",
    ],
)
def test_command_burst_and_buffering(args):
    with run_server(args) as server:
        channel = "#buf%d" % random.randint(10000, 99999)
        receiver = register_client(server, unique_name("recv"))
        coalesced = connect_client(server, "coalesced")
        split = None
        noisy = None
        active = None
        closer = None
        try:
            receiver.send_cmd("JOIN %s" % channel)
            expect_join(receiver, channel)

            coalesced_nick = unique_name("coal")
            registration_payload = (
                "PASS %s\r\nNICK %s\r\nUSER %s 0 * :%s\r\n"
                % (PASSWORD, coalesced_nick, coalesced_nick, coalesced_nick)
            )
            log_case("%s coalesced registration" % coalesced.actor())
            log_client_command(
                coalesced.actor(),
                "<PASS/NICK/USER in one write, %d bytes>"
                % len(registration_payload),
            )
            coalesced.send_raw(registration_payload, should_log_input=False)
            coalesced.wait_for_code("001")
            coalesced.drain()

            message_lines = ["JOIN %s" % channel]
            last_message = ""
            for index in range(10):
                last_message = "coalesced burst %02d %d" % (
                    index,
                    random.randint(10000, 99999),
                )
                message_lines.append("PRIVMSG %s :%s" % (channel, last_message))
            burst_payload = "\r\n".join(message_lines) + "\r\n"
            log_case("%s coalesced channel burst" % coalesced.actor())
            log_client_command(
                coalesced.actor(),
                "<JOIN + 10 PRIVMSG commands in one write, %d bytes>"
                % len(burst_payload),
            )
            coalesced.send_raw(burst_payload, should_log_input=False)
            receiver.wait_for_text(last_message)
            log_result("all 10 coalesced PRIVMSG commands reached the receiver")
            receiver.drain()

            partial_tail = "partial-complete-%d" % random.randint(10000, 99999)
            log_case("%s partial PRIVMSG completion" % coalesced.actor())
            partial_head = "PRIVMSG %s :waiting for" % channel
            partial_tail_payload = " %s\r\n" % partial_tail
            log_client_command(
                coalesced.actor(),
                "<partial PRIVMSG head %d bytes, then tail+CRLF %d bytes>"
                % (len(partial_head), len(partial_tail_payload)),
            )
            log_expect("message must not be dispatched before CRLF")
            coalesced.send_raw(
                partial_head,
                should_log_input=False,
            )
            expect_no_privmsg_text(receiver, partial_tail)
            log_expect("same message should dispatch after tail+CRLF arrives")
            coalesced.send_raw(partial_tail_payload, should_log_input=False)
            wait_for_buffer_text(
                receiver,
                partial_tail,
                description="receive completed partial PRIVMSG",
            )
            receiver.drain()

            closer = register_client(server, unique_name("closer"))
            join_channel(closer, channel)
            receiver.drain()
            dropped_tail = "dropped-partial-%d" % random.randint(10000, 99999)
            log_case("%s partial PRIVMSG close" % closer.actor())
            dropped_payload = "PRIVMSG %s :%s" % (channel, dropped_tail)
            log_client_command(
                closer.actor(),
                "<unfinished PRIVMSG %d bytes, then close>" % len(dropped_payload),
            )
            log_expect("unfinished PRIVMSG must be discarded on close")
            closer.send_raw(dropped_payload, should_log_input=False)
            closer.close()
            closer = None
            expect_no_privmsg_text(receiver, dropped_tail)
            log_result("unfinished PRIVMSG close did not deliver stale text")

            long_tail = "long-fragment-done-%d" % random.randint(10000, 99999)
            long_message = "long-fragment-%d-%s-%s" % (
                random.randint(10000, 99999),
                "x" * 4600,
                long_tail,
            )
            long_command = "PRIVMSG %s :%s" % (channel, long_message)
            log_case("%s long fragmented PRIVMSG" % coalesced.actor())
            log_client_command(
                coalesced.actor(),
                "<fragmented PRIVMSG, %d bytes before CRLF>" % len(long_command),
            )
            coalesced.send_raw(long_command[:17], should_log_input=False)
            time.sleep(0.05)
            coalesced.send_raw(long_command[17:4097], should_log_input=False)
            time.sleep(0.05)
            coalesced.send_raw(long_command[4097:], should_log_input=False)
            log_result(
                "sent long PRIVMSG chunks: %d, %d, %d bytes without CRLF"
                % (17, 4097 - 17, len(long_command) - 4097)
            )
            log_expect("long PRIVMSG must stay buffered until CRLF")
            expect_no_privmsg_text(receiver, long_tail)
            coalesced.send_raw("\r\n", should_log_input=False)
            log_expect("long PRIVMSG should dispatch after final CRLF")
            wait_for_buffer_text(
                receiver,
                long_tail,
                timeout=max(DEFAULT_TIMEOUT, 5.0),
                description="receive completed long fragmented PRIVMSG",
            )
            drain_without_printing_payload(
                receiver,
                "drained completed long PRIVMSG payload",
            )

            after_long = "after-long-%d" % random.randint(10000, 99999)
            very_long_message = "very-long-%s-%s" % ("y" * 12000, after_long)
            log_case("%s very long PRIVMSG followed by normal command" % coalesced.actor())
            log_client_command(
                coalesced.actor(),
                "<very long PRIVMSG %d bytes plus follow-up marker %r>"
                % (len(very_long_message), after_long),
            )
            log_expect(
                "server may reject or drop the very long line, but must process "
                "the following PRIVMSG"
            )
            coalesced.send_raw(
                "PRIVMSG %s :%s\r\nPRIVMSG %s :%s\r\n"
                % (channel, very_long_message, channel, after_long),
                should_log_input=False,
            )
            wait_for_buffer_text(
                receiver,
                after_long,
                timeout=max(DEFAULT_TIMEOUT, 5.0),
                description="receive follow-up after very long PRIVMSG",
            )
            log_result("follow-up PRIVMSG after very long line was processed")

            split = connect_client(server, "splitcrlf")
            split_nick = unique_name("split")
            log_case("%s split CRLF registration" % split.actor())
            log_client_command(
                split.actor(),
                "<PASS\\r then \\nNICK\\r then \\nUSER...\\r\\n>",
            )
            split.send_raw("PASS %s\r" % PASSWORD, should_log_input=False)
            time.sleep(0.05)
            split.send_raw("\nNICK %s\r" % split_nick, should_log_input=False)
            time.sleep(0.05)
            split.send_raw(
                "\nUSER %s 0 * :%s\r\n" % (split_nick, split_nick),
                should_log_input=False,
            )
            split.wait_for_code("001")
            log_result("split CRLF registration received numeric 001")

            noisy = connect_client(server, "unfinished")
            log_case("%s large unfinished input" % noisy.actor())
            log_client_command(noisy.actor(), "<4096 bytes without newline>")
            log_expect("unfinished input may remain buffered or be closed")
            try:
                noisy.send_raw("X" * 4096, should_log_input=False)
                log_result("%s sent large unfinished input" % noisy.actor())
            except socket.error:
                log_result("%s closed during large unfinished input" % noisy.actor())

            log_expect("another client should still register after unfinished input")
            active = register_client(server, unique_name("afterbuf"))
            log_result("server accepted a new client after unfinished input")
        finally:
            for client in [receiver, coalesced, split, noisy, active, closer]:
                if client is not None:
                    client.close()


@test_case(
    "test-13",
    "review regression coverage",
    "Reviewer-reported edge cases around MODE, INVITE, TOPIC, and NICK stay fixed.",
    [
        "malformed MODE returns one error and does not echo stale commands",
        "invite-only channel invites are consumed after one successful JOIN",
        "TOPIC set and query round-trips after mode changes",
        "NICK changes keep channel membership and routing usable",
    ],
)
def test_review_regression_coverage(args):
    with run_server(args) as server:
        channel = "#review%d" % random.randint(10000, 99999)
        op = register_client(server, unique_name("op"))
        guest = register_client(server, unique_name("guest"))
        observer = register_client(server, unique_name("observer"))
        keyless = None

        try:
            join_channel(op, channel)
            op.drain()

            malformed_start = len(op.buffer)
            op.send_cmd("MODE %s :+o" % channel)
            op.wait_for_code("461")
            op.drain()
            malformed_data = op.buffer[malformed_start:]
            if re.search(r"\bMODE\b\s+%s\s+\+o\b" % re.escape(channel), malformed_data):
                fail("malformed MODE was echoed as a successful MODE change")

            invalid_start = len(op.buffer)
            op.send_cmd("MODE %s =o" % channel)
            op.wait_for_code("472")
            op.drain()
            invalid_data = op.buffer[invalid_start:]
            numeric_count = len(
                re.findall(
                    r"(^|[\r\n]+):?[^ \r\n]*\s+(?:472|461|482)\s+",
                    invalid_data,
                )
            )
            if numeric_count != 1:
                fail("invalid MODE produced %d error numerics" % numeric_count)

            op.send_cmd("MODE %s +i" % channel)
            op.send_cmd("INVITE %s %s" % (guest.label, channel))
            guest.wait_for_regex(
                r"\bINVITE\b\s+%s\s+%s([\r\n]+|$)"
                % (re.escape(guest.label), re.escape(channel)),
                expectation="receive INVITE for %s" % channel,
                match_summary="INVITE %s %s" % (guest.label, channel),
                success_summary="matched INVITE for %s" % channel,
                failure_summary="before missing INVITE for %s" % channel,
            )
            join_channel(guest, channel)

            guest.send_cmd("PART %s :consume invite" % channel)
            guest.drain()
            guest.send_cmd("JOIN %s" % channel)
            expect_no_join(guest, channel)

            op.send_cmd("INVITE %s %s" % (guest.label, channel))
            guest.wait_for_regex(
                r"\bINVITE\b\s+%s\s+%s([\r\n]+|$)"
                % (re.escape(guest.label), re.escape(channel)),
                expectation="receive second INVITE for %s" % channel,
                match_summary="INVITE %s %s" % (guest.label, channel),
                success_summary="matched second INVITE for %s" % channel,
                failure_summary="before missing second INVITE for %s" % channel,
            )
            join_channel(guest, channel)

            topic = "review topic %d" % random.randint(10000, 99999)
            op.send_cmd("MODE %s +t" % channel)
            op.send_cmd("TOPIC %s :%s" % (channel, topic))
            op.send_cmd("TOPIC %s" % channel)
            op.wait_for_text(topic)

            op.send_cmd("MODE %s -i" % channel)
            op.send_cmd("MODE %s +k reviewkey" % channel)
            op.send_cmd("MODE %s -k reviewkey" % channel)
            keyless = register_client(server, unique_name("keyless"))
            join_channel(keyless, channel)

            new_guest = unique_name("renamed")
            guest.send_cmd("NICK %s" % new_guest)
            guest.drain()
            guest.label = new_guest
            observer.send_cmd("JOIN %s" % channel)
            expect_join(observer, channel)
            observer.drain()
            nick_message = "nick-change-channel-%d" % random.randint(10000, 99999)
            guest.send_cmd("PRIVMSG %s :%s" % (channel, nick_message))
            expect_privmsg(observer, channel, nick_message)
        finally:
            close_clients(op, guest, observer, keyless)


def test_label(name):
    return TEST_BY_NAME[name].label()


def run_command_for_test(test):
    if re.match(r"^test-\d+$", test.name):
        return "make -f tests/Makefile %s" % test.name
    if test.name == "transport-1":
        return "make -f tests/Makefile transport-smoke"
    if test.name == "transport-2":
        return "make -f tests/Makefile transport-strict"
    return (
        "python3 tests/scripts/irc_integration_test.py "
        "--binary ./ircserv --case %s" % test.name
    )


def print_case_run_and_summary(test, checks, run_command=""):
    focused_command = run_command_for_test(test)

    LOGGER.info("      Run")
    if run_command:
        log_wrapped(run_command, "        ")
        if run_command != focused_command:
            log_wrapped("case only: %s" % focused_command, "        ")
    else:
        log_wrapped(focused_command, "        ")
    LOGGER.info("      What")
    log_wrapped(
        "%d checks | %s" % (len(checks), test.summary),
        "        ",
        "        ",
    )


def print_case_plan(test, profile, run_command=""):
    checks = profile_checks(test, profile)
    LOGGER.info("%s %s", tag("RUN", "test"), test.label())
    print_case_run_and_summary(test, checks, run_command)
    if VERBOSE:
        LOGGER.info("      Checks")
        for index, check in enumerate(checks, 1):
            log_wrapped(
                "%d. %s" % (index, check),
                "        ",
                "           ",
            )


def print_test_list(profile):
    for test in ordered_tests():
        checks = profile_checks(test, profile)
        LOGGER.info("%s %s", tag("CASE", "test"), test.label())
        LOGGER.info("      Profile: %s", profile)
        print_case_run_and_summary(test, checks)
        LOGGER.info("      Checks")
        for index, check in enumerate(checks, 1):
            log_wrapped(
                "%d. %s" % (index, check),
                "        ",
                "           ",
            )
        LOGGER.info("")


def parse_args(argv):
    parser = argparse.ArgumentParser(
        description="Run ft_irc black-box integration tests"
    )
    parser.add_argument("--binary", default="./ircserv", help="path to ircserv binary")
    parser.add_argument(
        "--server-wrapper",
        default="",
        help="command prefix for running the server, for example a valgrind command",
    )
    parser.add_argument(
        "--require-valgrind-clean",
        action="store_true",
        help="fail server-backed cases when valgrind reports leaks or memory errors",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=DEFAULT_TIMEOUT,
        help="socket/read timeout in seconds; increase when using valgrind",
    )
    parser.add_argument(
        "--case",
        action="append",
        choices=[test.name for test in ordered_tests()],
        help="test case to run; may be specified multiple times",
    )
    parser.add_argument(
        "--profile",
        choices=["mandatory", "rfc"],
        default=os.environ.get("FT_IRC_TEST_PROFILE", "mandatory"),
        help="test profile: mandatory for subject-scope checks, rfc for stricter reply/error checks",
    )
    parser.add_argument(
        "--list",
        action="store_true",
        help="list available test cases with descriptions",
    )
    parser.add_argument(
        "--color",
        choices=["auto", "always", "never"],
        default=os.environ.get("FT_IRC_TEST_COLOR", "auto"),
        help="colorize status tags: auto, always, or never",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        default=os.environ.get("FT_IRC_TEST_VERBOSE", "") not in ("", "0", "false"),
        help="print command, expectation, client, and payload details as they happen",
    )
    parser.add_argument(
        "--failure-detail-lines",
        type=int,
        default=int(os.environ.get("FT_IRC_TEST_FAILURE_DETAIL_LINES", "32")),
        help="number of buffered detail lines printed after each failure",
    )
    parser.add_argument(
        "--run-command",
        default="",
        help="command used to invoke this run; shown in case summaries",
    )
    return parser.parse_args(argv)


def main(argv):
    global DEFAULT_TIMEOUT, FAILURE_DETAIL_LINES, VERBOSE

    configure_logging()
    args = parse_args(argv)
    set_color_mode(args.color)
    VERBOSE = args.verbose
    FAILURE_DETAIL_LINES = max(0, args.failure_detail_lines)
    DEFAULT_TIMEOUT = args.timeout
    args.server_wrapper = shlex.split(args.server_wrapper)
    if args.list:
        print_test_list(args.profile)
        return 0
    if not os.path.exists(args.binary):
        LOGGER.error("%s binary not found: %s", tag("FAIL", "fail"), args.binary)
        return 1

    selected_names = args.case or [test.name for test in ordered_tests()]
    selected = set(selected_names)
    LOGGER.info("%s binary: %s", tag("INFO", "info"), args.binary)
    LOGGER.info("%s timeout: %.2fs", tag("INFO", "info"), args.timeout)
    LOGGER.info("%s profile: %s", tag("INFO", "info"), args.profile)
    LOGGER.info("%s selected: %d test(s)", tag("INFO", "info"), len(selected_names))
    log_wrapped(
        ", ".join(test_label(name) for name in selected_names),
        "       ",
        "       ",
    )
    if args.server_wrapper:
        LOGGER.info(
            "%s server wrapper: %s",
            tag("INFO", "info"),
            format_command(args.server_wrapper),
        )

    failures = 0
    passed = 0
    total = 0
    first_test = True
    for test in ordered_tests():
        if test.name not in selected:
            continue
        if first_test:
            LOGGER.info("")
            first_test = False
        else:
            LOGGER.info("")
        total += 1
        started = time.time()
        try:
            start_test_log(test.name)
            print_case_plan(test, args.profile, args.run_command)
            test.func(args)
            passed += 1
            LOGGER.info(
                "%s %s (%.2fs)",
                tag("PASS", "ok"),
                test.label(),
                time.time() - started,
            )
        except Exception as err:
            failures += 1
            LOGGER.error(
                "%s %s: %s",
                tag("FAIL", "fail"),
                test.label(),
                err,
            )
            if not command_lines_have_ko():
                log_result("test failed: %s" % err, ok=False)
            flush_commands()
            flush_details()
    if total:
        LOGGER.info("")
    LOGGER.info(
        "%s %d/%d passed, %d failed",
        tag("SUMMARY", "info"),
        passed,
        total,
        failures,
    )
    if failures:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
