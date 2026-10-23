# ft_irc Research & 3-Person Work Plan (C++98)

## 1. Goal and Scope (Mandatory)
Implement an IRC server executable:

```bash
./ircserv <port> <password>
```

Mandatory scope:
- non-blocking single-process server
- one `poll()`-based event loop (or equivalent)
- TCP/IP client connections
- authentication + registration flow
- channel operations and operator controls

Out of scope:
- IRC client implementation
- server-to-server network

---

## 2. Requirements Breakdown

### 2.1 Build / Language Rules
- C++98 (`-std=c++98`) compatible
- compile flags: `-Wall -Wextra -Werror -std=c++98 -pedantic`
- no Boost / external libraries
- Makefile targets: `all`, `$(NAME)`, `clean`, `fclean`, `re`

### 2.2 Network / Runtime Rules
- must not crash under normal or error conditions
- no `fork()`
- all fds non-blocking
- one poll-based multiplexing core for listen/read/write
- robust handling of partial packets and command reassembly

### 2.3 IRC Feature Minimum
- PASS / NICK / USER registration path
- JOIN channel
- PRIVMSG user↔user and user→channel forwarding
- operator vs regular user
- channel operator commands:
  - `KICK`
  - `INVITE`
  - `TOPIC`
  - `MODE` with `i`, `t`, `k`, `o`, `l`

---

## 3. Recommended Reference Client
Use one reference client consistently during development and evaluation rehearsal:
- HexChat (GUI) **or** irssi (CLI)

Recommendation for team flow:
- Dev/debug: `nc -C`, irssi (easy to inspect protocol behavior)
- Final behavior check: HexChat (common evaluator usage)

---

## 4. Suggested High-Level Architecture

### 4.1 Core Components
1. **Server**
   - socket setup, bind/listen, accept
   - owns pollfd table
   - dispatches readable/writable events

2. **ClientSession**
   - per-fd state
   - auth/registration flags
   - input buffer (for partial frames)
   - output queue (for partial send/backpressure)

3. **Command Parser**
   - line split by CRLF
   - tokenize `COMMAND params :trailing`
   - normalization (uppercase command)

4. **Command Handlers**
   - PASS/NICK/USER/JOIN/PRIVMSG
   - KICK/INVITE/TOPIC/MODE
   - numeric replies and errors

5. **Channel Model**
   - members
   - operators
   - mode flags `i,t,k,o,l`
   - topic/key/user-limit/invite-list

6. **Reply Builder**
   - consistent RFC-style output format
   - centralized numeric/error messages

### 4.2 Event Loop Pattern
- poll for events
- if listen fd readable → accept until EAGAIN
- if client fd readable → recv append buffer, extract complete lines
- for each full command line → parse + handle
- if fd writable and queue non-empty → send remaining bytes
- on disconnect/error → cleanup client and channel membership

---

## 5. 3-Person Work Split (balanced)

### Member A: Network/Event Loop Owner
**Primary responsibility**: reliable non-blocking server core.

Tasks:
- socket init/bind/listen/accept
- poll loop and fd lifecycle
- read/write buffering
- disconnect/error handling
- signal-safe shutdown basics

Deliverables:
- `Server`, `ClientSession` low-level transport
- stable command line extraction from stream

Definition of done:
- 50+ clients can connect/disconnect without hang/crash
- fragmented input reconstructed correctly

### Member B: IRC Protocol & Command Owner
**Primary responsibility**: registration and mandatory commands.

Tasks:
- parser/tokenizer (RFC-like command structure)
- PASS/NICK/USER registration finite-state behavior
- JOIN/PRIVMSG basic semantics
- operator commands: KICK/INVITE/TOPIC/MODE(i,t,k,o,l)
- numeric reply/error consistency

Deliverables:
- command dispatch table
- handler implementations + protocol tests

Definition of done:
- reference client can fully register and use all mandatory command paths
- expected errors returned for invalid order/params/permissions

### Member C: Domain Model/Quality/Docs Owner
**Primary responsibility**: channel state correctness + project quality.

Tasks:
- channel/user model classes and invariants
- mode state transitions and authorization checks
- Makefile quality and build reproducibility
- test scenarios and regression checklist
- README + docs consolidation

Deliverables:
- `Channel` and supporting model utilities
- test matrix, evaluator rehearsal scripts, final docs

Definition of done:
- all mode combinations behave predictably
- docs allow third party to build/run/test quickly

---

## 6. Shared Coding Rules (C++98)
- no C++11+ features (`auto`, range-for, nullptr, etc.)
- prefer STL/C++ headers (`<cstring>` etc.) over C headers when possible
- RAII where practical; explicit ownership semantics
- keep functions small and single-purpose
- no hidden side effects in getters
- avoid exceptions for normal control flow
- strict input validation before state mutation
- deterministic error replies

Style suggestions:
- one class per pair of `.hpp/.cpp`
- command handler naming: `handlePASS`, `handleNICK`, ...
- no magic numbers; centralize numeric reply constants
- avoid global mutable state

### 6.1 How to decide coding conventions as a team
Use the following agreement process before implementation starts:
1. **Draft proposal (Member C owner)**
   - prepare a one-page proposal covering naming, file layout, error handling, and logging.
2. **30-minute architecture review (all 3 members)**
   - each member gives one risk and one improvement point.
3. **Rule finalization by consensus**
   - only rules that all members can explain are adopted.
4. **Record in writing**
   - add final rules to `docs/coding_conventions.md` and reference them from README.
5. **Weekly review and revision lock**
   - allow updates once per week only, to avoid unstable style drift.

Mandatory convention items to finalize:
- naming (class/function/member/constant)
- include order and header guards
- max function length target
- error code + numeric reply construction policy
- ownership policy for pointers/references
- branch and commit message format

### 6.2 Suggested coding convention baseline (first version)
- Class names: `PascalCase` (`ClientSession`)
- Methods: `camelCase` (`parseCommand`)
- Private members: trailing underscore (`nickname_`)
- Constants/macros: `UPPER_SNAKE_CASE`
- Files: class-based pairing (`Channel.hpp` / `Channel.cpp`)
- One responsibility per source file block
- No duplicated parsing logic across handlers

---

## 7. Unified Validation Environment (build now)

### 7.1 Why unify environment
Many ft_irc bugs are environment-dependent (client version, shell behavior, timing, partial packet behavior). Standardizing tool versions reduces “works on my machine” issues.

### 7.2 Team standard environment profile
- OS baseline: Linux (Ubuntu LTS) for main tests
- Compiler: `c++` with `-Wall -Wextra -Werror -std=c++98 -pedantic`
- Tools:
  - `make`
  - `nc` (OpenBSD netcat preferred)
  - `irssi` or `hexchat`
  - `python3` (for lightweight test scripts)
- Locale/encoding: UTF-8
- Timezone: UTC (for deterministic logs)

### 7.3 Environment bootstrap checklist
1. Confirm compiler and stdlib availability
2. Confirm netcat variant and CRLF behavior (`nc -C`)
3. Confirm reference IRC client can connect localhost
4. Confirm no unexpected firewall blocks on test ports
5. Run smoke test suite before feature development

### 7.4 Add reproducible scripts (start now)
Create these files:
- `tests/scripts/env_check.sh`
  - check compiler version, required commands, and basic socket permissions
- `tests/scripts/smoke_test.sh`
  - boot server, run PASS/NICK/USER/JOIN/PRIVMSG happy path via scripted client input
- `tests/scripts/fragment_test.sh`
  - send split command packets and verify reconstruction

Script requirements:
- POSIX shell compatible
- exits non-zero on failure
- prints concise PASS/FAIL summary

### 7.5 CI-like local routine (shared)
Before pushing:
1. `make re`
2. run `tests/scripts/env_check.sh`
3. run `tests/scripts/smoke_test.sh`
4. run `tests/scripts/fragment_test.sh`
5. run manual check from reference client (join/channel message/topic/mode)

---

## 8. Milestone Plan

### Milestone 1 (Core online)
- server starts
- multiple TCP clients connect
- basic PASS/NICK/USER accepted
- `tests/scripts/env_check.sh` created

Member focus:
- **Member A**: implement socket setup, bind/listen, non-blocking accept, `poll()` registration, fd removal, and per-client input buffering.
- **Member B**: implement the parser entry point, command dispatch skeleton, PASS/NICK/USER registration state, and the first success/error numeric replies.
- **Member C**: keep Makefile/scripts reproducible, finalize the first coding convention draft, and create the basic environment checklist used by all members.

Integration checkpoint:
- one local server process accepts at least three clients
- each client can complete PASS/NICK/USER
- invalid port/password usage fails deterministically

### Milestone 2 (Messaging)
- JOIN/PRIVMSG channel forwarding stable
- disconnect and cleanup stable
- `tests/scripts/smoke_test.sh` created

Member focus:
- **Member A**: harden read/write queues, partial send handling, disconnect cleanup, and multi-client stability.
- **Member B**: implement JOIN and PRIVMSG semantics, including user-to-user and user-to-channel forwarding plus expected errors.
- **Member C**: implement/maintain channel membership state, ensure cleanup removes users from all channels, and turn the happy path into `tests/scripts/smoke_test.sh`.

Integration checkpoint:
- two or more registered clients can join the same channel
- channel PRIVMSG is forwarded only to the correct recipients
- abrupt client close leaves no stale channel membership

### Milestone 3 (Operator features)
- KICK/INVITE/TOPIC/MODE complete
- permissions and edge cases covered
- `tests/scripts/fragment_test.sh` created

Member focus:
- **Member A**: verify the transport layer remains stable under command bursts, queued replies, and fragmented command input.
- **Member B**: implement KICK/INVITE/TOPIC/MODE handlers, command-specific validation, and numeric reply/error consistency.
- **Member C**: own channel mode state (`i`, `t`, `k`, `o`, `l`), authorization checks, invite/key/limit edge cases, and `tests/scripts/fragment_test.sh`.

Integration checkpoint:
- channel operators and regular users are distinguished correctly
- all mandatory channel operator commands work from the reference client
- fragmented commands are reconstructed before handler execution

### Milestone 4 (Hardening)
- partial packet tests
- stress tests
- README/docs finalization
- coding convention review log updated

Member focus:
- **Member A**: run connection stress tests, verify no blocking calls remain, and fix cleanup paths found during repeated connect/disconnect cycles.
- **Member B**: audit protocol edge cases, invalid command order, missing parameters, duplicate nicknames, and reply formatting.
- **Member C**: run the final regression checklist, update README/docs, capture known limitations, and coordinate evaluator rehearsal.

Integration checkpoint:
- `make re` and all scripts pass from a clean checkout
- reference-client manual tests cover registration, messaging, operator commands, and mode combinations
- every member can explain the full request flow, not only their own module

---

## 9. Test Checklist (practical)
- fragmented command input via `nc -C`
- invalid registration order
- duplicate nick
- join protected/invite-only/key-limited channels
- mode toggling races and authority checks
- kick while target absent/present
- topic read/write with `+t`
- graceful handling of abrupt client close
- reconnect after forced disconnect
- invalid UTF-8 / long line handling policy confirmation

---

## 10. Documentation Strategy (each member writes architecture)
Each member must document **overall architecture** (not just own tasks):
- `docs/member_a_architecture.md`
- `docs/member_b_architecture.md`
- `docs/member_c_architecture.md`

Template:
1. End-to-end request flow
2. Data structures and ownership
3. Command lifecycle
4. Failure paths and recovery
5. Open risks and mitigations
6. How this design is validated in unified test environment

This ensures cross-understanding and prevents single-point dependency.
