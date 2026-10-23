*This project has been created as part of the 42 curriculum by skomatsu, tkusama, mishimod*

---

## Description

**ft_irc** is a project designed to create a fully functional, IRC (Internet Relay Chat) server from scratch using **C++98**.
The primary objective is to understand network programming, multiplexing, and standard internet protocols by implementing a text-based communication server that complies with the IRC protocol.

The server handles multiple clients simultaneously without hanging or forking, managing all input/output operations in a **non-blocking** manner via a single `poll()` (or equivalent multiplexing system mechanism).
It allows users to authenticate, set identities, join channels, and communicate in real-time.

### Key Features
* **Multiplexed Network I/O**: Single-threaded, non-blocking I/O using `poll()` to handle connection acceptance, reading, and writing efficiently without resource exhaustion.
* **User Authentication & Management**: Registration via password, nickname, and username validation.
* **Channel Operations**: Channel creation, message broadcasting to all channel members, and specific operator privileges.
* **Operator Commands**: Complete implementation of channel management commands including `KICK`, `INVITE`, `TOPIC`, and `MODE` (with flags `i`, `t`, `k`, `o`, `l`).
* **Bonus Part 1 - Direct Client-to-Client File Transfer (DCC SEND)**: Facilitates secure binary and text file transport parameters by detecting, validating, and safely routing CTCP metadata (`\x01DCC SEND ...\x01`) directly between attached peers without blocking the core event loop.
* **Bonus Part 2 - Hit & Blow IRC Bot**: Provides an external IRC bot client for playing a 3-digit Hit & Blow game through normal channel or direct `PRIVMSG` traffic.
---

## Instructions

### Prerequisites
* A Unix-like operating system (Linux or macOS).
* A C++ compiler (for example, `c++` or `g++`) supporting C++98.
* Optional manual clients: `irssi` and `nc`.

### Build

Build everything with a single command:

```bash
make
```

The build uses `-Wall -Wextra -Werror -std=c++98 -pedantic` and creates both
the server `ircserv` (including the bonus DCC file-transfer relay) and the
bonus bot `ircbot`.

Useful Makefile rules:

```bash
make         # build ircserv and ircbot
make clean   # remove object files (keeps the executables)
make fclean  # remove object files and the executables
make re      # fclean + build
```

### Mandatory Server

Start the server with a port and password:

```bash
./ircserv 6667 pass
```

Keep this terminal open while testing clients from other terminals.

### Mandatory Operation With Irssi

This walkthrough uses three terminals: one for the server and one for each client. Follow the steps in order.

**Step 1. Terminal 1 — start the server** and leave it running:

```bash
./ircserv 6667 pass
```

**Step 2. Terminal 2 — connect as `alice`:**

```bash
irssi -c localhost -p 6667 -w pass -n alice
```

**Step 3. Terminal 3 — connect as `bob`:**

```bash
irssi -c localhost -p 6667 -w pass -n bob
```

Here `-c` is the server host, `-p` the port, `-w` the connection password, and `-n` the nickname.

**Step 4. In alice's window**, create the channel and send messages:

```text
/join #manual
/msg #manual hello everyone
/msg bob hello directly
```

**Step 5. In bob's window**, join the same channel and reply:

```text
/join #manual
/msg #manual hi alice
```

**Step 6. From either window**, leave the channel and disconnect when finished:

```text
/part #manual bye
/quit
```

The first user to join a channel becomes the channel operator (here, `alice`). Operator status is required for `KICK`, `INVITE`, `TOPIC` when `+t` is set, and channel `MODE` changes.

### Nickname and Username

`NICK` and `USER` are separate fields and do not have to match:

* `NICK <nickname>` — the visible name used for everything (messages, joins, kicks, invites).
* `USER <username> 0 * :<realname>` — registration metadata only. It is not the password and is not used to address the client.

So this is valid, and the client is still addressed as `alice`:

```text
PASS pass
NICK alice
USER guest 0 * :Guest User
```

### Operator Commands and Channel Modes

Run this with two clients connected: `alice`, who created `#manual` and is therefore its operator (`@alice`), and `bob` in a second window. Each block is a short scenario you can type to see the mode take effect. The `[alice]` / `[bob]` prefix shows which window to type in, and `;` marks the expected result.

Invite-only — `+i` / `-i`:

```text
[alice] /mode #manual +i      ; turn invite-only on
[bob]   /part #manual
[bob]   /join #manual          ; refused: bob is not invited
[alice] /invite bob #manual
[bob]   /join #manual          ; now bob gets in
[alice] /mode #manual -i      ; open the channel again
```

Topic lock — `+t` / `-t`:

```text
[alice] /mode #manual +t      ; only operators may set the topic
[bob]   /topic #manual hijack  ; refused
[alice] /topic #manual Official topic   ; works
[alice] /mode #manual -t      ; any member may set the topic again
```

Channel key (password) — `+k` / `-k`:

```text
[alice] /mode #manual +k secret
[bob]   /part #manual
[bob]   /join #manual          ; refused: key required
[bob]   /join #manual secret   ; works
[alice] /mode #manual -k      ; remove the key
```

Operator privilege — `+o` / `-o`:

```text
[alice] /mode #manual +t       ; lock the topic to operators
[alice] /mode #manual +o bob   ; promote bob to operator (@bob)
[bob]   /topic #manual set by bob   ; allowed: bob is now an operator
[alice] /mode #manual -o bob   ; demote bob
[bob]   /topic #manual nope    ; refused: bob is no longer an operator
[alice] /mode #manual -t       ; unlock the topic
```

User limit — `+l` / `-l`:

```text
[alice] /mode #manual +l 2     ; with alice and bob already in, a third join is refused
[alice] /mode #manual -l       ; remove the limit
```

Other operator commands:

```text
[alice] /mode #manual           ; show the channel's active modes
[alice] /topic #manual New topic text   ; set the topic
[alice] /kick #manual bob bye   ; remove bob from the channel
```

### Mandatory Operation With nc

`nc` (netcat) lets you type raw IRC protocol lines by hand, which is useful for grading and debugging the server directly.

**Step 1. Terminal 1 — start the server** if it is not already running:

```bash
./ircserv 6667 pass
```

**Step 2. Terminal 2 — open a raw connection:**

```bash
nc -C localhost 6667
```

The `-C` flag makes `nc` send CRLF (`\r\n`) line endings, which the IRC protocol requires.

**Step 3. Register as `alice`, then join and send.** Type one command per line:

```text
PASS pass
NICK alice
USER alice 0 * :Alice
JOIN #manual
PRIVMSG #manual :hello from nc
```

The first three lines (`PASS`, `NICK`, `USER`) must come first, in that order, to complete registration. After that you can join a channel and send messages. Every supported command is listed in the quick reference below.

**Step 4. Terminal 3 — connect a second client as `bob`** to verify that the server relays channel traffic to every member:

```text
PASS pass
NICK bob
USER bob 0 * :Bob
JOIN #manual
PRIVMSG #manual :hi alice
PRIVMSG alice :direct hello
```

Once both are in `#manual`, each client should see the other's `JOIN` and channel messages, and `alice` should receive the direct `PRIVMSG`. Close either session with `QUIT :bye` when finished.

The raw `USER alice 0 * :Alice` line is just a simple example. `USER guest 0 * :Guest User` would also register successfully after `NICK alice`; the server still addresses the client by the `NICK` value.

Raw command quick reference:

* `PASS <password>` sends the server password before registration is completed.
* `NICK <nickname>` sets or changes the visible nickname used as the sender name and message target.
* `USER <username> 0 * :<realname>` sends registration metadata. The username and realname do not have to match the nickname.
* `JOIN <channel> [key]` enters a channel. The optional key is required only when the channel has mode `+k`.
* `PRIVMSG <target> :<message>` sends a message to a channel or nickname. The target can be `#channel` or another user's nickname.
* `PING :<token>` checks that the server is responsive. The server replies with `PONG :<token>`.
* `MODE <channel>` shows the current channel modes.
* `MODE <channel> <flags> [argument]` changes channel modes. Supported flags are `+i/-i`, `+t/-t`, `+k/-k`, `+o/-o`, and `+l/-l`.
* `PART <channel> [:message]` leaves a channel, optionally with a part message.
* `NAMES [channel]` lists visible nicknames for a channel using `353` and `366`.
* `WHO [target]` acknowledges a WHO query with `315`, enough for common IRC clients that send it automatically.
* `QUIT [:message]` closes the IRC session and notifies users sharing a channel.
* `KICK <channel> <nickname> [:reason]` removes a user from a channel. The sender must be a channel operator.
* `INVITE <nickname> <channel>` invites a user into an invite-only channel. The sender must be a channel operator.
* `TOPIC <channel> [:topic]` reads or sets the channel topic. If mode `+t` is set, only channel operators can set it.

If your `nc` does not support `-C`, use `nc localhost 6667` and make sure your input sends CRLF line endings when possible.

### Bonus Features

`make` already builds the bonus pieces alongside the mandatory server:

* `ircserv` includes the CTCP DCC file-transfer control-message relay over direct `PRIVMSG`.
* `ircbot` is an external Hit & Blow IRC bot client.

Start the server first:

```bash
./ircserv 6667 pass
```

### Bonus Operation With Irssi

This continues from the mandatory setup: the bonus `ircserv` is running in Terminal 1, and you are already connected with Irssi as `alice` in Terminal 2. Only the bot needs to be added.

**Step 1. Terminal 3 — start the bot** and point it at the channel:

```bash
./ircbot localhost 6667 pass Bot '#manual'
```

The arguments are host, port, password, bot nickname, and the channel to join. Leave it running.

**Step 2. In alice's Irssi window**, join the same channel and play:

```text
/join #manual
/msg #manual !help
/msg #manual !hb start
/msg #manual !guess 123
/msg #manual !hb status
/msg #manual !hb giveup
```

Each channel, or each direct message conversation, has its own Hit & Blow game state. The answer is a 3-digit number with unique digits; leading zero is allowed.

**DCC file transfer (Irssi):** with `alice` and `bob` both connected to the server (no shared channel is required), send a file directly between them:

1. Sender — in alice's window, start the transfer:

   ```text
   /dcc send bob /path/to/file.txt
   ```

2. Receiver — in bob's window, accept it:

   ```text
   /dcc get alice
   ```

The server only relays the CTCP DCC control message, which tells the receiver the sender's address, port, and file size. The file bytes are then transferred directly between the two clients, never through the server. HexChat and LimeChat work the same way, usually with a click-to-accept prompt.

### Bonus Operation With nc

This assumes the bonus `ircserv` is already running in Terminal 1 (from the Bonus Build step above). Add the bot and two raw `nc` clients to verify it.

**Step 1. Terminal 2 — start the bot** and leave it running:

```bash
./ircbot localhost 6667 pass Bot '#manual'
```

**Step 2. Terminal 3 — connect as `alice` and play:**

```bash
nc -C localhost 6667
```

```text
PASS pass
NICK alice
USER alice 0 * :Alice
JOIN #manual
PRIVMSG #manual :!hb start
PRIVMSG #manual :!guess 123
PRIVMSG Bot :!hb status
```

**Step 3. Terminal 4 — connect a second client as `bob`** to confirm channel messages and bot replies reach every member:

```bash
nc -C localhost 6667
```

```text
PASS pass
NICK bob
USER bob 0 * :Bob
JOIN #manual
PRIVMSG #manual :!guess 456
```

After `bob` joins, both clients should see each other's messages and the bot's replies in `#manual`.

**DCC control message (raw shape):**

```text
PRIVMSG bob :<SOH>DCC SEND file.txt 2130706433 9000 1234<SOH>
```

`<SOH>` represents byte `0x01`.

## Resources
* **[RFC 1459 (Internet Relay Chat Protocol)](https://datatracker.ietf.org/doc/html/rfc1459)**: Internet Relay Chat Protocol foundational network parameters.
* **[RFC 2812 (Internet Relay Chat: Client Protocol)](https://datatracker.ietf.org/doc/html/rfc2812)**: Specifications for replies, command formats, and numeric codes.
* **System Manuals**: `man 2 poll`, `man 2 socket`, `man 2 recv`, `man 2 send`, `man 2 fcntl`.


### AI Usage Description

* **Code Review**: AI was used as a peer-review tool to check our C++98 syntax constraints and find potential bugs, memory leaks, or array boundary errors before testing.
* **Documentation Research & Summarization**: AI helped summarize complex network protocols and manual pages, including RFC 1459, RFC 2812, `poll()`, and `fcntl()`. This allowed the team to understand the requirements quickly.
* **Test Case Creation**: AI helped design various test scenarios. This included preparing input data to check for malformed commands, empty lines, and fragmented packets to ensure the server never crashes.
