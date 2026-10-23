#!/bin/sh
set -eu

pass() { printf '[PASS] %s\n' "$1"; }
fail() { printf '[FAIL] %s\n' "$1"; exit 1; }

command -v c++ >/dev/null 2>&1 || fail "c++ not found"
command -v make >/dev/null 2>&1 || fail "make not found"
command -v nc >/dev/null 2>&1 || fail "nc not found"

pass "required commands exist"

printf '%s\n' 'int main(){return 0;}' > /tmp/ft_irc_env_check.cpp
c++ -Wall -Wextra -Werror -std=c++98 -pedantic /tmp/ft_irc_env_check.cpp -o /tmp/ft_irc_env_check_bin \
  || fail "c++98 compile test failed"
pass "c++98 compile flags work"

nc -h >/tmp/ft_irc_nc_help.txt 2>&1 || true
if grep -q -- '-C' /tmp/ft_irc_nc_help.txt; then
  pass "nc supports -C"
else
  printf '[WARN] nc may not support -C; partial packet test may need adjustment\n'
fi

pass "env check completed"
