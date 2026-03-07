#!/bin/bash

CUB3D="./cub3D"
PASS=0
FAIL=0

run_test() {
	local name="$1"
	local expect_exit="$2"
	shift 2
	local args=("$@")

	"$CUB3D" "${args[@]}" > /tmp/cub3d_stdout 2> /tmp/cub3d_stderr
	local actual_exit=$?

	if [ "$actual_exit" -eq "$expect_exit" ]; then
		printf "  \033[32mPASS\033[0m %s\n" "$name"
		PASS=$((PASS + 1))
	else
		printf "  \033[31mFAIL\033[0m %s (expected exit %d, got %d)\n" \
			"$name" "$expect_exit" "$actual_exit"
		if [ -s /tmp/cub3d_stderr ]; then
			printf "       stderr: %s\n" "$(cat /tmp/cub3d_stderr)"
		fi
		FAIL=$((FAIL + 1))
	fi
}

run_test_stderr() {
	local name="$1"
	local expect_exit="$2"
	local expect_str="$3"
	shift 3
	local args=("$@")

	"$CUB3D" "${args[@]}" > /tmp/cub3d_stdout 2> /tmp/cub3d_stderr
	local actual_exit=$?
	local stderr_content
	stderr_content=$(cat /tmp/cub3d_stderr)

	if [ "$actual_exit" -eq "$expect_exit" ] && echo "$stderr_content" | grep -q "$expect_str"; then
		printf "  \033[32mPASS\033[0m %s\n" "$name"
		PASS=$((PASS + 1))
	else
		printf "  \033[31mFAIL\033[0m %s (exit: expected %d got %d, stderr: \"%s\")\n" \
			"$name" "$expect_exit" "$actual_exit" "$stderr_content"
		FAIL=$((FAIL + 1))
	fi
}

echo "=== cub3D Parser Tests ==="
echo ""

echo "[argument errors]"
run_test_stderr "no arguments"         1 "Usage"
run_test_stderr "too many arguments"   1 "Usage"   maps/valid.cub extra_arg

echo ""
echo "[extension errors]"
run_test_stderr "wrong extension .txt" 1 "extension" tests/maps/wrong_ext.txt
run_test_stderr "no extension"         1 "extension" Makefile

echo ""
echo "[file errors]"
run_test_stderr "file not found"       1 "open"     nonexistent.cub
run_test_stderr "directory as arg"     1 ""         srcs/.cub

echo ""
echo "[config errors]"
run_test_stderr "duplicate texture"    1 "Duplicate"   tests/maps/dup_texture.cub
run_test_stderr "missing texture"      1 "Missing"     tests/maps/missing_texture.cub
run_test_stderr "color out of range"   1 "range"       tests/maps/bad_color.cub
run_test_stderr "bad color format"     1 "format"      tests/maps/bad_color_format.cub

echo ""
echo "[valid files]"
run_test "valid.cub parses OK"         0               maps/valid.cub

echo ""
echo "=========================="
printf "Results: \033[32m%d passed\033[0m, \033[31m%d failed\033[0m\n" "$PASS" "$FAIL"

rm -f /tmp/cub3d_stdout /tmp/cub3d_stderr
exit $FAIL
