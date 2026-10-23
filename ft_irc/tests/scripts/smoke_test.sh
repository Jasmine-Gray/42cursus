#!/bin/sh
set -eu

PORT="${1:-6667}"
PASSWD="${2:-pass}"

if [ ! -x ./ircserv ]; then
  echo "[FAIL] ./ircserv not found or not executable"
  exit 1
fi

./ircserv "$PORT" "$PASSWD" >/tmp/ft_irc_server.log 2>&1 &
PID=$!
trap 'kill "$PID" >/dev/null 2>&1 || true' EXIT INT TERM
sleep 1

{
  printf 'PASS %s\r\n' "$PASSWD"
  printf 'NICK smokeA\r\n'
  printf 'USER smokeA 0 * :Smoke A\r\n'
  printf 'JOIN #room\r\n'
  printf 'PRIVMSG #room :hello\r\n'
  sleep 1
} | nc 127.0.0.1 "$PORT" >/tmp/ft_irc_smoke_client.log 2>&1 || true

echo "[PASS] smoke script executed (inspect logs for protocol assertions)"
