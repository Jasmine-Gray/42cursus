# ft_irc Test Suite

This directory documents the project test space. The tests treat the 42
`ft_irc` requirements as the source of truth.

## Testing Procedure

Use root `make` or `make re` when you only need the submitted server binary.
Use `tests/Makefile` for the Docker integration suite, focused cases, and
stricter validation.

### 1. Build the Submitted Server

```bash
make
```

Expected:
- the mandatory `ircserv` binary is built at the repository root
- the build uses the submitted `Makefile`, `includes/`, and `srcs/`

To rebuild from clean objects:

```bash
make re
```

### 2. Run the Current CI Gate in Ubuntu

```bash
make -f tests/Makefile ci
```

Expected:
- this currently runs `CI_TESTS=test-1 ... test-13`
- GitHub Actions uses `ubuntu-24.04`
- the repository is mounted into the Ubuntu container before the case runs
- the mounted test container uses the `ubuntu:24.04` image
- GitHub Actions runs the same staged gate through `tests/Makefile`

### 3. Confirm the Ubuntu Mount

```bash
make -f tests/Makefile check
```

Expected:
- the Ubuntu image builds
- the repository is mounted at `/workspace`
- the container prints `pwd` and `ls -la`
- `ircserv` is rebuilt inside the container under `/tmp/ft_irc_ubuntu`
- the available integration cases are listed

### 4. List Docker Test Cases

To print detailed runner cases with their purpose and checks through Docker:

```bash
make -f tests/Makefile list
```

### 5. Run Focused Docker Cases

Each focused case builds the Ubuntu image, mounts the repository, rebuilds
`ircserv` inside the container, and then runs the requested numbered test.

```bash
make -f tests/Makefile list
make -f tests/Makefile test-1
make -f tests/Makefile test-2
make -f tests/Makefile test-5
make -f tests/Makefile test-8
```

Expected:
- each target starts Docker and mounts the repository at `/workspace`
- each target rebuilds `ircserv` inside Ubuntu under `/tmp/ft_irc_ubuntu`
- server-backed cases start `ircserv` on a temporary local port in the container
- clients are created by the Python runner
- compact output is the default: each selected test prints `[RUN]`, a one-line
  check summary, then `[PASS]` or `[FAIL]`
- passed and failed tests print a `commands:` block with shell-style `$` commands,
  expected result, observed output, process exit code when applicable, and a
  colored `[OK]`/`[KO]` result
- a final `[SUMMARY]` prints passed and failed counts
- runner output uses Python's standard `logging` library
- the runner prints `[INFO]` for binary, timeout, profile, and selected cases
- failed tests also print a bounded `details:` tail; tune it with
  `FAILURE_DETAIL_LINES=N`
- trace labels use a fixed-width, right-aligned bracket style such as
  `[case-01]:`, `[    cmd]:`, `[ expect]:`, `[ output]:`, `[   exit]:`,
  and `[ result]:` so output stays aligned
- trace labels and `[OK]`/`[KO]` results are colored when color output is enabled
- client interactions are tagged as `client-A(actualNick)`, `client-B(...)`,
  and so on; each client keeps the same color within a test
- after a `NICK` command, the client tag uses the attempted nickname so command
  and received output are easier to connect
- client socket sends are displayed as `client-A(nick): $ 'PASS pass\r\n'`;
  raw fragmented sends use `client-A(nick): $ <raw-bytes>` followed by
  `<input>`
- process stdout/stderr are grouped under `[ output]:` as `<stdout>` and
  `<stderr>` sections
- IRC payloads are also grouped under `[ output]:` as `<input>`, `<match>`,
  or `<client-A(nick) received>` sections
- server stdout/stderr for server-backed cases is grouped under `[ output]:`
  as `<server stdout+stderr>`
- `VERBOSE=1` prints the full trace as it happens, including command, expected
  result, grouped output payloads, result, `[SERVER]`, and `[CLIENT]` logs
- stdout/stderr and IRC payloads in command traces, verbose output, or failure
  details are rendered in `cat -e` style
- empty stdout/stderr or IRC output is left blank under that label, matching
  empty `cat -e` output
- color is automatic for terminals; use `COLOR=always` for colored CI or piped
  output, and `COLOR=never` for plain text
- `PROFILE=mandatory` is the default and keeps checks inside subject scope;
  `PROFILE=rfc` adds stricter IRC numeric/error and parser hardening checks

### 6. Run the Full Suite When Ready

```bash
make -f tests/Makefile test
```

Expected:
- all mandatory-profile integration cases are attempted
- this may fail while the server implementation is still incomplete
- failures identify the next protocol or lifecycle behavior to implement

To run the stricter RFC/error-reply profile:

```bash
make -f tests/Makefile test PROFILE=rfc
```

### 7. Run Strict Leak and FD Checks on Linux

```bash
make -f tests/Makefile strict
make -f tests/Makefile strict-test-6
make -f tests/Makefile strict-test-11
make -f tests/Makefile strict-test-12
```

Expected:
- Valgrind reports no definite, indirect, or possible leaks
- `ERROR SUMMARY: 0 errors`
- no non-stdio file descriptors remain open at exit

## Recommended Docker Flow

Run the full mandatory suite through root `make`. Run focused tests through the
dedicated test `Makefile`.

```bash
make
make -f tests/Makefile list
make -f tests/Makefile check
make -f tests/Makefile test-1
make -f tests/Makefile test-2
make -f tests/Makefile test-3
make -f tests/Makefile test-4
make -f tests/Makefile test-5
make -f tests/Makefile test-6
make -f tests/Makefile test-7
make -f tests/Makefile test-8
make -f tests/Makefile test-9
make -f tests/Makefile test-10
make -f tests/Makefile test-11
make -f tests/Makefile test-12
make -f tests/Makefile test-13
make -f tests/Makefile transport-smoke
```

Each `test-1` ... `test-13` target:
- builds the Ubuntu test image
- mounts this repository at `/workspace`
- prints `pwd` and `ls -la`
- rebuilds `ircserv` inside the container
- runs one focused integration scenario

Ubuntu build outputs go to `/tmp/ft_irc_ubuntu` inside the container, so Linux
artifacts do not overwrite host-side `ircserv` or `objs/`.

## Test Cases

| Target | Test | Purpose |
| --- | --- | --- |
| `test-1` | Argument checks | Invalid CLI arguments must fail quickly. |
| `test-2` | Registration and non-blocking | PASS/NICK/USER flow and idle client not blocking active client; `rfc` also checks exact registration error numerics. |
| `test-3` | Fragmented registration | TCP packet fragmentation: split PASS/NICK/USER must be reassembled. |
| `test-4` | Empty and malformed input | Empty and spaces-only input must not crash or corrupt registration; `rfc` also checks exact malformed-input replies. |
| `test-5` | Protocol edge cases | Mandatory checks trailing message preservation; `rfc` also checks lowercase commands, unknown command, invalid channel name, and missing target numerics. |
| `test-6` | HUP and disconnect cleanup | Empty close, abortive close, partial-command abortive/normal close, and continued service; `rfc` also checks channel member disconnect cleanup. |
| `test-7` | Transport hardening | Burst connection registration and reconnect after mixed closes; `rfc` also checks occupied-port startup failure. |
| `test-8` | Command burst and buffering | Coalesced commands, split CRLF boundaries, long fragmented command delivery, and large unfinished input safety. |
| `test-9` | JOIN and PRIVMSG | JOIN and PRIVMSG delivery for channels and direct messages, including direct-message non-broadcast; `rfc` also checks non-member send errors. |
| `test-10` | Two-client roundtrip | Two clients exchange channel and direct messages in both directions. |
| `test-11` | Operator commands and modes | Fragmented KICK, INVITE/TOPIC, and MODE `i/t/k/o/l` happy paths, including `+l` join blocking; `rfc` also checks exact permission/mode error numerics. |
| `test-12` | Mass message stability | Sends 300 channel messages in one burst, checks every message is delivered in order, and confirms the server still accepts a new client afterward. |
| `test-13` | Review regression coverage | Malformed MODE, one-shot INVITE, TOPIC, and NICK-change channel routing regressions. |

## Transport-Only Candidate Checks

Use these cases when reviewing Member A transport work before it is wired into
the full protocol dispatcher. They do not require `001`, JOIN replies, channel
state, or numeric errors.

```bash
python3 tests/scripts/irc_integration_test.py --binary ./ircserv --case transport-1
python3 tests/scripts/irc_integration_test.py --binary ./ircserv \
  --case transport-1 --case transport-2
```

For a standalone transport candidate outside the submission tree, build that
binary first and pass its path:

```bash
python3 tests/scripts/irc_integration_test.py --binary /tmp/transport_candidate \
  --case transport-1
```

Available transport-only cases:

| Case | Purpose |
| --- | --- |
| `transport-1` | Accept raw clients, receive coalesced/fragmented input, survive large unfinished input, close clients, and reconnect. |
| `transport-2` | Fail quickly with non-zero status when the listen port is already occupied. |

## Adding Test Cases

Add new runner cases in `tests/scripts/irc_integration_test.py` with the `@test_case`
decorator directly above the test function. The decorator is the single source
for the case id, title, purpose, and checklist used by `--list`, compact output,
and case selection.

```python
@test_case(
    "test-12",
    "new behavior",
    "Short purpose sentence.",
    [
        "first expected behavior",
        "second expected behavior",
    ],
)
def test_new_behavior(args):
    with run_server(args) as server:
        client = register_client(server, unique_name("new"))
        client.send_cmd("PING :token")
        client.wait_for_text("token")
        client.close()
```

Cases named `test-N` are displayed and executed in numeric order, regardless of
where the function appears in the file. Add the corresponding focused
`tests/Makefile` target when the case should be runnable through `make`.

Strict fd/leak checks are tied to focused cases:

```bash
make -f tests/Makefile strict-test-6
make -f tests/Makefile strict-test-11
make -f tests/Makefile strict-test-12
```

## Completion Order and Viewpoints

Use this order when expanding `CI_TESTS`. The numeric target names follow the
shape of a server becoming complete.

| Order | Target | Viewpoint | Why it comes here |
| --- | --- | --- | --- |
| 1 | `test-1` | Process contract | Invalid arguments must fail before a long-running server starts. This keeps CI failures cheap and clear. |
| 2 | `test-2` | First usable IRC session | PASS/NICK/USER, `001`, and idle-client behavior prove the server can accept real clients; `rfc` adds wrong password and duplicate nick numerics. |
| 3 | `test-3` | TCP stream buffering | Split input must be reassembled before parsing. Without this, later command tests can fail for transport reasons. |
| 4 | `test-4` | Parser safety | Empty and whitespace-only input must not crash or corrupt registration; `rfc` adds exact malformed-input replies. |
| 5 | `test-5` | Protocol tolerance | Trailing text preservation checks core parsing; `rfc` adds case-insensitive commands, repeated spaces, unknown commands, and invalid targets. |
| 6 | `test-6` | Lifecycle cleanup | Close, abortive close, partial-command abortive/normal disconnect, and continued service catch hang/fd/poll issues; `rfc` adds channel cleanup after disconnect. |
| 7 | `test-7` | Transport hardening | Burst accept/registration and reconnect after fd churn catch event-loop issues that smaller happy paths can miss. |
| 8 | `test-8` | Command burst and buffering | Dense read-buffer tests catch one-command-per-recv assumptions, missing long-line buffers, and unfinished-line blocking before CI is widened. |
| 9 | `test-9` | Basic messaging | JOIN and PRIVMSG must deliver to the right clients, and direct messages must not leak to unrelated clients; `rfc` also checks non-member channel-send rejection. |
| 10 | `test-10` | Two-client roundtrip | Bidirectional channel and direct messages prove the common human workflow works end to end. |
| 11 | `test-11` | Operator feature completion | Fragmented KICK, INVITE, TOPIC, and MODE `i/t/k/o/l` check the most stateful mandatory commands, including limit enforcement. |
| 12 | `test-12` | Mass message stability | Large message bursts catch crash, wedge, output-buffer, and event-loop starvation issues after the main command set works. |
| 13 | `test-13` | Review regression pass | Locks down reviewer-reported regressions after core behavior is in place. |
| 14 | `strict-test-6` | Leak and fd hardening | Run after behavior is stable; Valgrind turns HUP cleanup into a strict leak/fd gate. |
| 15 | `strict-test-11` | Operator leak gate | Runs KICK/operator state changes under Valgrind to check leaks, memory errors, and fd leaks. |
| 16 | `strict-test-12` | Mass-message leak gate | Runs the mass message scenario under Valgrind to check leaks, memory errors, fd leaks, crashes, and hangs. |

Suggested CI expansion uses one `CI_TESTS` line at a time:

| Stage | `CI_TESTS` |
| --- | --- |
| 1 | `test-1` |
| 2 | `test-1 test-2` |
| 3 | `test-1 test-2 test-3` |
| 4 | `test-1 test-2 test-3 test-4` |
| 5 | `test-1 test-2 test-3 test-4 test-5` |
| 6 | `test-1 test-2 test-3 test-4 test-5 test-6` |
| 7 | `test-1 test-2 test-3 test-4 test-5 test-6 test-7` |
| 8 | `test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8` |
| 9 | `test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8 test-9` |
| 10 | `test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8 test-9 test-10` |
| 11 | `test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8 test-9 test-10 test-11` |
| 12 | `test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8 test-9 test-10 test-11 test-12` |
| 13 | `test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8 test-9 test-10 test-11 test-12 test-13` |

Use the list target when you want the command, summary, and checks for every
case:

```bash
make -f tests/Makefile list
```

Focused Docker targets:

| Target | Command | What it checks |
| --- | --- | --- |
| `test-1` | `make -f tests/Makefile test-1` | CLI argument validation. |
| `test-2` | `make -f tests/Makefile test-2` | Registration and idle-client non-blocking behavior. |
| `test-3` | `make -f tests/Makefile test-3` | Fragmented `PASS`/`NICK`/`USER` buffering. |
| `test-4` | `make -f tests/Makefile test-4` | Blank and malformed input safety. |
| `test-5` | `make -f tests/Makefile test-5` | Parser tolerance and trailing text preservation. |
| `test-6` | `make -f tests/Makefile test-6` | Normal close, abortive close, and post-close service. |
| `test-7` | `make -f tests/Makefile test-7` | Connection bursts, fd reuse, and reconnect behavior. |
| `test-8` | `make -f tests/Makefile test-8` | Coalesced commands, split CRLF, partial input, and long input. |
| `test-9` | `make -f tests/Makefile test-9` | Channel and direct `PRIVMSG` routing. |
| `test-10` | `make -f tests/Makefile test-10` | Two-client channel and direct-message roundtrip. |
| `test-11` | `make -f tests/Makefile test-11` | `KICK`, `INVITE`, `TOPIC`, and channel `MODE`. |
| `test-12` | `make -f tests/Makefile test-12` | Mass-message stability and event-loop progress. |
| `test-13` | `make -f tests/Makefile test-13` | Reviewer-reported regression coverage. |
| `transport-smoke` | `make -f tests/Makefile transport-smoke` | Raw socket lifecycle before IRC registration. |
| `transport-strict` | `make -f tests/Makefile transport-strict` | Raw transport hardening, including unfinished input. |
| `strict-test-6` | `make -f tests/Makefile strict-test-6` | `test-6` under Valgrind leak/fd checks. |
| `strict-test-11` | `make -f tests/Makefile strict-test-11` | `test-11` under Valgrind leak/fd checks. |
| `strict-test-12` | `make -f tests/Makefile strict-test-12` | `test-12` under Valgrind leak/fd checks. |

## What These Tests Catch

### Invalid Arguments

Examples:

```bash
./ircserv
./ircserv not-a-port pass
./ircserv 70000 pass
./ircserv 6667 ""
```

Expected:
- missing, non-numeric, and out-of-range arguments exit non-zero
- invalid arguments do not start a long-running server
- `rfc` also treats an empty password as invalid

Common failures:
- non-numeric port starts the server
- out-of-range port starts the server or hangs
- `rfc`: empty password is accepted

### Registration

Examples:

```text
PASS pass
NICK alice
USER alice 0 * :Alice User
```

Expected:
- registered clients receive `001`
- idle clients do not block active registration
- `rfc`: wrong password returns `464`
- `rfc`: missing parameters return `461` or a command-specific numeric
- `rfc`: duplicate nickname returns `433`
- `rfc`: unregistered JOIN/PRIVMSG returns `451`

Common failures:
- `001` is never sent
- idle clients block active clients
- `rfc`: duplicate nick overwrites the existing client

### Fragmented Input

TCP does not preserve IRC command boundaries. This input must still register:

```bash
{
  printf 'PA'
  sleep 1
  printf 'SS pass\r\nNI'
  sleep 1
  printf 'CK frag\r\nUSER frag 0 * :Fragment User\r\n'
} | nc -C 127.0.0.1 6667
```

Expected:
- the server buffers partial bytes
- complete IRC lines are dispatched only after CRLF/newline arrives
- the client receives `001`

Common failures:
- `PA` is parsed as an unknown command
- split `NICK` is dropped
- partial input corrupts the next command

### Empty and Malformed Input

Examples:

```text
<empty line>
<spaces only>
PASS
NICK
USER onlyone
JOIN
PRIVMSG
PRIVMSG #room
PRIVMSG #room :
```

Expected:
- empty or spaces-only lines do not crash the server
- registration still succeeds after blank input
- `rfc`: missing parameters return a numeric error
- `rfc`: empty PRIVMSG body returns `412`

Common failures:
- empty lines break registration
- parser treats spaces-only input as a command
- `rfc`: empty messages are broadcast

### Protocol Edge Cases

Examples:

```text
pass    pass
nick   alice
user   alice 0 * :Alice User
DOESNOTEXIST
JOIN room
PRIVMSG ghost :hello
PRIVMSG #room :hello   with   spaces
```

Expected:
- trailing message text preserves spaces after `:`
- `rfc`: commands are case-insensitive
- `rfc`: repeated spaces between parameters are tolerated
- `rfc`: unknown command returns `421`
- `rfc`: invalid channel name is rejected
- `rfc`: unknown nick returns `401`

### HUP and Disconnect Cleanup

The suite closes sockets in several ways:
- connect then close immediately
- send half of a command then abortively close
- send half of a command then normally close, like leaving `nc` with Ctrl+C
- join a channel then disconnect
- disconnect one client and continue messaging with another

Expected:
- server keeps running
- closed fd is removed from `poll`
- no send/recv occurs on a closed fd
- new clients can still connect after HUP
- `rfc`: disconnected clients are removed from channel membership

Common failures:
- server exits after a client disconnects
- bad file descriptor errors after `POLLHUP`
- kicked or disconnected clients remain in channels
- file descriptors remain open at exit

### Transport Hardening

The suite opens a burst of clients before any of them sends registration data,
then registers all of them and closes them with a mix of normal and abortive
closes.

Expected:
- queued TCP connections are accepted and registered
- fd churn does not leave stale client state behind
- a new client can still register after the burst disconnects
- `rfc`: starting on an already occupied port exits non-zero quickly

Common failures:
- only the first connection in a burst is serviced
- stale fd state corrupts a reconnect
- bind/listen failures are ignored and the server keeps running

### Command Burst and Buffering

The suite sends registration commands in one TCP write, sends a JOIN plus
multiple PRIVMSG commands in one TCP write, splits CRLF across packets, and
delivers a long fragmented PRIVMSG only after CRLF. It also keeps one client
holding a large unfinished line while another registers.

Expected:
- all complete lines in one read are dispatched in order
- CRLF split across packets still completes commands
- a long command spanning multiple `recv` calls is buffered until CRLF
- large unfinished input from one client does not block other clients

Common failures:
- only one command is processed per `recv`
- `\r` and `\n` split across packets corrupt the next command
- a command longer than the local read buffer is truncated or parsed early
- a client with no newline causes unrelated clients to hang

### Messaging

Examples:

```text
# Alice and Bob joined #room
PRIVMSG #room :hello through channel
PRIVMSG bob :hello directly
```

Expected:
- channel messages go to other channel members
- direct messages reach the target nickname
- direct messages do not leak to unrelated clients
- `rfc`: non-members cannot send to the channel

Common failures:
- direct message is silently ignored
- direct message is broadcast to unrelated clients
- `rfc`: non-member messages are broadcast to the channel

### KICK and Membership

Examples:

```text
KICK #room bob :bye

# Bob after being kicked
PRIVMSG #room :after kick
```

Expected:
- operators can kick members
- fragmented KICK input is buffered until the final newline
- `rfc`: non-operators receive `482`
- `rfc`: kicked users stop receiving and sending channel messages

Common failures:
- KICK works in one write but fails when split across TCP packets
- kicked users remain in channel membership
- KICK only notifies clients but does not mutate state

### Channel Modes

Examples:

```text
MODE #room +i
MODE #room +k secret
MODE #room +l 2
MODE #room +t
MODE #room +o bob
MODE #room -o bob
```

Expected:
- `+i` allows invited users to join
- `+k` allows users with the correct key to join
- `+l` prevents users above the limit from joining
- `+t` allows operators to change topic
- `+o` grants operator privileges
- `rfc`: `+i` rejects uninvited users with `473`
- `rfc`: `+k` rejects missing/wrong key with `475`
- `rfc`: `+l` rejects users above the limit with `471`
- `rfc`: `+t` prevents non-operators from changing topic
- `rfc`: `-o` removes operator privileges

Common failures:
- modes are stored but not enforced
- `+o` changes display only, not permissions
- `rfc`: removing modes does not restore normal behavior

### Valgrind Memory and FD Checks

`strict-test-6`, `strict-test-11`, and `strict-test-12` run focused scenarios
under:

```text
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
  --errors-for-leak-kinds=definite,indirect,possible --error-exitcode=99
```

Expected:
- no definite, indirect, or possible leaks
- `ERROR SUMMARY: 0 errors`
- no non-stdio file descriptors left open at exit

Notes:
- `still reachable` is not a hard failure because the server is terminated by
  the test harness.
- non-stdio fd leaks are a hard failure.
