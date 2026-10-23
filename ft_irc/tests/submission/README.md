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

### Mandatory Build

Build only the mandatory server:

```bash
make
```

The build uses `-Wall -Wextra -Werror -std=c++98` and creates `ircserv`.

Useful Makefile rules:

```bash
make
make clean
make fclean
make re
```

### Mandatory Server

Start the server with a port and password:

```bash
./ircserv 6667 pass
```

Keep this terminal open while testing clients from other terminals.

### Mandatory Operation With Irssi

Connect two clients:

```bash
irssi -c localhost -p 6667 -w pass -n alice
irssi -c localhost -p 6667 -w pass -n bob
```

Common commands:

```text
/join #manual
/msg #manual hello everyone
/msg bob hello directly
/part #manual bye
/quit
```

The first user to join a channel becomes the channel operator. Operator status is required for `KICK`, `INVITE`, `TOPIC` when `+t` is set, and channel `MODE` changes.

Operator commands:

```text
/kick #manual bob bye
/invite bob #manual
/topic #manual New topic text
/mode #manual
```

Mode examples:

```text
/mode #manual +i
/mode #manual -i
/mode #manual +t
/mode #manual -t
/mode #manual +k secret
/join #manual secret
/mode #manual -k
/mode #manual +o bob
/mode #manual -o bob
/mode #manual +l 2
/mode #manual -l
```

### Mandatory Operation With nc

Connect with `nc`:

```bash
nc -C localhost 6667
```

Then type raw IRC commands:

```text
PASS pass
NICK alice
USER alice 0 * :Alice
JOIN #manual
PRIVMSG #manual :hello from nc
PING :12345
MODE #manual +i
MODE #manual -i
PART #manual :bye
QUIT
```

If your `nc` does not support `-C`, use `nc localhost 6667` and make sure your input sends CRLF line endings when possible.

### Bonus Build

Build the bonus server and bot:

```bash
make bonus
```

The bonus build creates:

* `ircserv`, with CTCP DCC file-transfer control-message relay over direct `PRIVMSG`
* `ircbot`, an external Hit & Blow IRC bot client

Start the server first:

```bash
./ircserv 6667 pass
```

### Bonus Operation With Irssi

Connect a normal user:

```bash
irssi -c localhost -p 6667 -w pass -n alice
```

Start the bot in another terminal:

```bash
./ircbot localhost 6667 pass Bot '#manual'
```

In Irssi:

```text
/join #manual
/msg #manual !help
/msg #manual !hb start
/msg #manual !guess 123
/msg #manual !hb status
/msg #manual !hb giveup
```

Each channel, or each direct message conversation, has its own Hit & Blow game state. The answer is a 3-digit number with unique digits; leading zero is allowed.

For DCC file-transfer checks, connect two reference clients such as Irssi, HexChat, or LimeChat, join the same server, and start a normal DCC SEND from one client to the other. The server relays the CTCP DCC control message; the file bytes are transferred peer-to-peer by the clients.

### Bonus Operation With nc

Start the bonus server and bot:

```bash
./ircserv 6667 pass
./ircbot localhost 6667 pass Bot '#manual'
```

Connect with `nc`:

```bash
nc -C localhost 6667
```

Register and talk to the bot:

```text
PASS pass
NICK alice
USER alice 0 * :Alice
JOIN #manual
PRIVMSG #manual :!hb start
PRIVMSG #manual :!guess 123
PRIVMSG Bot :!hb status
```

Raw CTCP DCC control-message shape:

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
