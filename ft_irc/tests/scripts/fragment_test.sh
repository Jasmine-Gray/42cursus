#!/bin/sh
set -eu

PORT="${1:-6667}"
PASSWD="${2:-pass}"

if [ ! -x ./ircserv ]; then
  echo "[FAIL] ./ircserv not found or not executable"
  exit 1
fi

./ircserv "$PORT" "$PASSWD" >/tmp/ft_irc_server_fragment.log 2>&1 &
PID=$!
trap 'kill "$PID" >/dev/null 2>&1 || true' EXIT INT TERM
sleep 1

{
  printf 'PA'
  sleep 1
  printf 'SS %s\r\n' "$PASSWD"
  printf 'NI'
  sleep 1
  printf 'CK fragA\r\n'
  printf 'USER fragA 0 * :Frag A\r\n'
  sleep 1
} | nc -C 127.0.0.1 "$PORT" >/tmp/ft_irc_fragment_client.log 2>&1 || true

echo "[PASS] fragment script executed (inspect logs for reassembly assertions)"
