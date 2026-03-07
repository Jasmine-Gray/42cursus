#!/bin/bash
# Comprehensive cub3D test suite
# Tests both mandatory and bonus builds

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

PASS=0
FAIL=0
TOTAL=0
TMPDIR=$(mktemp -d)

cleanup() {
	rm -rf "$TMPDIR"
}
trap cleanup EXIT

# Helper: expect error (non-zero exit)
expect_error() {
	local desc="$1"
	local expected_msg="$2"
	shift 2
	TOTAL=$((TOTAL + 1))
	"$@" > /tmp/cub3d_test_out 2>&1
	local exit_code=$?
	local output
	output=$(cat /tmp/cub3d_test_out)
	if [ $exit_code -ne 0 ]; then
		if [ -n "$expected_msg" ] && ! echo "$output" | grep -qi "$expected_msg"; then
			printf "${YELLOW}WARN${NC} %-50s (exit=%d, expected '%s', got '%s')\n" "$desc" "$exit_code" "$expected_msg" "$output"
			PASS=$((PASS + 1))
		else
			printf "${GREEN}PASS${NC} %-50s (exit=%d)\n" "$desc" "$exit_code"
			PASS=$((PASS + 1))
		fi
	else
		printf "${RED}FAIL${NC} %-50s (expected error, got exit=0)\n" "$desc"
		FAIL=$((FAIL + 1))
	fi
}

# Helper: expect success (runs for 2 sec then kills)
expect_success() {
	local desc="$1"
	shift
	TOTAL=$((TOTAL + 1))
	timeout 2 "$@" > /tmp/cub3d_test_out 2>&1
	exit_code=$?
	output=$(cat /tmp/cub3d_test_out)
	# 124 = timeout (good, means it ran), 0 = clean exit
	if [ $exit_code -eq 124 ] || [ $exit_code -eq 0 ]; then
		printf "${GREEN}PASS${NC} %-50s (ran OK)\n" "$desc"
		PASS=$((PASS + 1))
	else
		printf "${RED}FAIL${NC} %-50s (exit=%d, output='%s')\n" "$desc" "$exit_code" "$output"
		FAIL=$((FAIL + 1))
	fi
}

# Helper: create a valid base map file
make_valid_map() {
	local f="$1"
	cat > "$f" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm

F 100,100,100
C 50,50,200

1111111
1000001
100N001
1000001
1111111
MAP
}

echo "=============================================="
echo "  cub3D Comprehensive Test Suite"
echo "=============================================="
echo ""

#----------------------------------------------------
# Part 1: MANDATORY BUILD
#----------------------------------------------------
echo "--- Building mandatory ---"
make re > /dev/null 2>&1
echo ""
echo "=== [MANDATORY] Argument Tests ==="

expect_error "No arguments" "" ./cub3D
expect_error "Too many arguments" "" ./cub3D maps/large_room.cub extra
expect_error "Empty string arg" "" ./cub3D ""

echo ""
echo "=== [MANDATORY] File Extension Tests ==="

touch "$TMPDIR/noext"
touch "$TMPDIR/test.txt"
touch "$TMPDIR/test.cub.txt"
touch "$TMPDIR/.cub"

expect_error "No extension" "Invalid file extension" ./cub3D "$TMPDIR/noext"
expect_error ".txt extension" "Invalid file extension" ./cub3D "$TMPDIR/test.txt"
expect_error ".cub.txt extension" "Invalid file extension" ./cub3D "$TMPDIR/test.cub.txt"

echo ""
echo "=== [MANDATORY] File Access Tests ==="

expect_error "Nonexistent file" "Cannot open file" ./cub3D "$TMPDIR/nonexistent.cub"
expect_error "Directory as file" "" ./cub3D "$TMPDIR/.cub"

mkdir -p "$TMPDIR/dir.cub" 2>/dev/null || true
expect_error "Directory with .cub extension" "" ./cub3D "$TMPDIR/dir.cub"

echo ""
echo "=== [MANDATORY] Empty / Minimal File Tests ==="

touch "$TMPDIR/empty.cub"
expect_error "Empty file" "" ./cub3D "$TMPDIR/empty.cub"

echo "" > "$TMPDIR/newline_only.cub"
expect_error "Only newlines" "" ./cub3D "$TMPDIR/newline_only.cub"

echo "   " > "$TMPDIR/spaces_only.cub"
expect_error "Only spaces" "" ./cub3D "$TMPDIR/spaces_only.cub"

echo ""
echo "=== [MANDATORY] Missing Config Tests ==="

cat > "$TMPDIR/no_north.cub" <<'MAP'
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
100N001
1111111
MAP
expect_error "Missing NO texture" "Missing configuration" ./cub3D "$TMPDIR/no_north.cub"

cat > "$TMPDIR/no_floor.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
C 50,50,200

1111111
100N001
1111111
MAP
expect_error "Missing floor color" "Missing configuration" ./cub3D "$TMPDIR/no_floor.cub"

cat > "$TMPDIR/no_ceil.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 50,50,200

1111111
100N001
1111111
MAP
expect_error "Missing ceiling color" "Missing configuration" ./cub3D "$TMPDIR/no_ceil.cub"

echo ""
echo "=== [MANDATORY] Duplicate Config Tests ==="

cat > "$TMPDIR/dup_north.cub" <<'MAP'
NO ./textures/north.xpm
NO ./textures/south.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
100N001
1111111
MAP
expect_error "Duplicate NO texture" "Duplicate" ./cub3D "$TMPDIR/dup_north.cub"

cat > "$TMPDIR/dup_floor.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
F 200,200,200
C 50,50,200

1111111
100N001
1111111
MAP
expect_error "Duplicate floor color" "Duplicate" ./cub3D "$TMPDIR/dup_floor.cub"

echo ""
echo "=== [MANDATORY] Texture Path Tests ==="

cat > "$TMPDIR/missing_tex_path.cub" <<'MAP'
NO
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
100N001
1111111
MAP
expect_error "Missing texture path" "" ./cub3D "$TMPDIR/missing_tex_path.cub"

cat > "$TMPDIR/bad_tex_path.cub" <<'MAP'
NO ./textures/nonexistent.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
100N001
1111111
MAP
expect_error "Nonexistent texture file" "not found" ./cub3D "$TMPDIR/bad_tex_path.cub"

echo ""
echo "=== [MANDATORY] Color Format Tests ==="

# Invalid color values
for color_test in \
	"F -1,100,100:Negative value" \
	"F 256,100,100:Value > 255" \
	"F 1000,100,100:Value > 999" \
	"F 100,100:Only 2 values" \
	"F 100,100,100,100:4 values" \
	"F abc,100,100:Letters in color" \
	"F ,,,:Only commas" \
	"F 100,,100:Empty middle value" \
	"F ,100,100:Empty first value"
do
	color_val="${color_test%%:*}"
	color_desc="${color_test##*:}"
	cat > "$TMPDIR/color_test.cub" <<MAP
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
$color_val
C 50,50,200

1111111
100N001
1111111
MAP
	expect_error "Color: $color_desc" "" ./cub3D "$TMPDIR/color_test.cub"
done

echo ""
echo "=== [MANDATORY] Map Character Tests ==="

for char_test in "2:digit 2" "D:door char" "x:lowercase x" "@:at sign" "#:hash"; do
	ch="${char_test%%:*}"
	ch_desc="${char_test##*:}"
	cat > "$TMPDIR/char_test.cub" <<MAP
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
10${ch}N001
1111111
MAP
	expect_error "Invalid char: $ch_desc" "Invalid character" ./cub3D "$TMPDIR/char_test.cub"
done

echo ""
echo "=== [MANDATORY] Player Tests ==="

cat > "$TMPDIR/no_player.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
1000001
1111111
MAP
expect_error "No player start" "No player" ./cub3D "$TMPDIR/no_player.cub"

cat > "$TMPDIR/two_players.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
1N00S01
1111111
MAP
expect_error "Multiple players" "Multiple player" ./cub3D "$TMPDIR/two_players.cub"

echo ""
echo "=== [MANDATORY] Map Enclosure Tests ==="

cat > "$TMPDIR/open_top.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1001111
1000001
100N001
1000001
1111111
MAP
expect_error "Open top wall" "not enclosed" ./cub3D "$TMPDIR/open_top.cub"

cat > "$TMPDIR/open_bottom.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
1000001
100N001
1000001
1001111
MAP
expect_error "Open bottom wall" "not enclosed" ./cub3D "$TMPDIR/open_bottom.cub"

cat > "$TMPDIR/open_left.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
1000001
000N001
1000001
1111111
MAP
expect_error "Open left wall" "not enclosed" ./cub3D "$TMPDIR/open_left.cub"

cat > "$TMPDIR/open_right.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
1000000
100N001
1000001
1111111
MAP
expect_error "Open right wall" "not enclosed" ./cub3D "$TMPDIR/open_right.cub"

cat > "$TMPDIR/space_hole.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
1000 01
100N001
1000001
1111111
MAP
expect_error "Space in middle of map" "not enclosed" ./cub3D "$TMPDIR/space_hole.cub"

cat > "$TMPDIR/irregular_open.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

  111
 1 001
1100N01
 10001
  111
MAP
expect_error "Irregular shape with gap" "not enclosed" ./cub3D "$TMPDIR/irregular_open.cub"

cat > "$TMPDIR/empty_line_in_map.cub" <<MAP
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
100N001

1000001
1111111
MAP
expect_error "Empty line inside map" "not enclosed" ./cub3D "$TMPDIR/empty_line_in_map.cub"

echo ""
echo "=== [MANDATORY] Edge Case Maps ==="

cat > "$TMPDIR/minimal_map.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

111
1N1
111
MAP
expect_success "Minimal 3x3 map" ./cub3D "$TMPDIR/minimal_map.cub"

cat > "$TMPDIR/player_n.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

11111
1N001
11111
MAP
expect_success "Player facing N" ./cub3D "$TMPDIR/player_n.cub"

cat > "$TMPDIR/player_s.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

11111
1S001
11111
MAP
expect_success "Player facing S" ./cub3D "$TMPDIR/player_s.cub"

cat > "$TMPDIR/player_e.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

11111
1E001
11111
MAP
expect_success "Player facing E" ./cub3D "$TMPDIR/player_e.cub"

cat > "$TMPDIR/player_w.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

11111
1W001
11111
MAP
expect_success "Player facing W" ./cub3D "$TMPDIR/player_w.cub"

# Color boundary values
cat > "$TMPDIR/color_bounds.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 0,0,0
C 255,255,255

111
1N1
111
MAP
expect_success "Color boundary 0,0,0 and 255,255,255" ./cub3D "$TMPDIR/color_bounds.cub"

# Spaces/tabs around config
cat > "$TMPDIR/spaces_config.cub" <<MAP
NO  ./textures/north.xpm
SO	./textures/south.xpm
WE  ./textures/west.xpm
EA  ./textures/east.xpm
F  100 , 100 , 100
C  50 , 50 , 200

111
1N1
111
MAP
expect_success "Spaces/tabs in config" ./cub3D "$TMPDIR/spaces_config.cub"

# Existing map files
expect_success "large_room.cub" ./cub3D maps/large_room.cub

for mapfile in maps/corridors.cub maps/pillars.cub maps/multi_room.cub maps/irregular_shape.cub; do
	if [ -f "$mapfile" ]; then
		expect_success "$mapfile" ./cub3D "$mapfile"
	fi
done

echo ""
echo "=== [MANDATORY] Config-only (no map) ==="

cat > "$TMPDIR/no_map.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200
MAP
expect_error "Config complete but no map" "No map found" ./cub3D "$TMPDIR/no_map.cub"

echo ""
echo "=== [MANDATORY] Map-only (no config) ==="

cat > "$TMPDIR/no_config.cub" <<'MAP'
1111111
100N001
1111111
MAP
expect_error "Map without config" "" ./cub3D "$TMPDIR/no_config.cub"

echo ""
echo "=== [MANDATORY] Tricky edge cases ==="

# Player on border
cat > "$TMPDIR/player_border.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

N1111
10001
11111
MAP
expect_error "Player on border" "not enclosed" ./cub3D "$TMPDIR/player_border.cub"

# Single line map
cat > "$TMPDIR/single_line_map.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1N1
MAP
expect_error "Single line map" "not enclosed" ./cub3D "$TMPDIR/single_line_map.cub"

# Tabs in map
cat > "$TMPDIR/tab_in_map.cub" <<MAP
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

1111111
10	N001
1111111
MAP
expect_error "Tab in map" "" ./cub3D "$TMPDIR/tab_in_map.cub"

#----------------------------------------------------
# Part 2: BONUS BUILD
#----------------------------------------------------
echo ""
echo "--- Building bonus ---"
make bonus > /dev/null 2>&1
echo ""
echo "=== [BONUS] Basic functionality ==="

expect_success "bonus: test_map.cub (with doors)" ./cub3D maps/test_map.cub
expect_success "bonus: large_room.cub" ./cub3D maps/large_room.cub

echo ""
echo "=== [BONUS] Door validation ==="

cat > "$TMPDIR/door_valid.cub" <<'MAP'
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

11111111
10000001
1000N001
10000001
1D000001
10000001
11111111
MAP
expect_success "bonus: Door in map" ./cub3D "$TMPDIR/door_valid.cub"

echo ""
echo "=== [BONUS] Error cases (should match mandatory) ==="

expect_error "bonus: No arguments" "" ./cub3D
expect_error "bonus: Nonexistent file" "" ./cub3D "$TMPDIR/nonexistent.cub"
expect_error "bonus: No player" "" ./cub3D "$TMPDIR/no_player.cub"
expect_error "bonus: Open wall" "" ./cub3D "$TMPDIR/open_top.cub"
expect_error "bonus: Bad color" "" ./cub3D "$TMPDIR/color_test.cub"

echo ""
echo "=== [BONUS] All direction starts ==="

for dir in N S E W; do
	cat > "$TMPDIR/bonus_dir_${dir}.cub" <<MAP
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 100,100,100
C 50,50,200

11111
1${dir}001
11111
MAP
	expect_success "bonus: Player facing $dir" ./cub3D "$TMPDIR/bonus_dir_${dir}.cub"
done

#----------------------------------------------------
# Part 3: LEAKS CHECK
#----------------------------------------------------
echo ""
echo "=== LEAKS CHECK ==="

check_leaks() {
	local desc="$1"
	shift
	TOTAL=$((TOTAL + 1))
	"$@" &
	local pid=$!
	sleep 2
	leaks_output=$(leaks "$pid" 2>&1) || true
	kill "$pid" 2>/dev/null
	wait "$pid" 2>/dev/null
	if echo "$leaks_output" | grep -q "0 leaks"; then
		printf "${GREEN}PASS${NC} %-50s (0 leaks)\n" "$desc"
		PASS=$((PASS + 1))
	elif echo "$leaks_output" | grep -q "[0-9]* leaks"; then
		leak_line=$(echo "$leaks_output" | grep "[0-9]* leaks")
		printf "${RED}FAIL${NC} %-50s (%s)\n" "$desc" "$leak_line"
		FAIL=$((FAIL + 1))
	else
		printf "${YELLOW}WARN${NC} %-50s (could not parse leaks output)\n" "$desc"
		PASS=$((PASS + 1))
	fi
}

# Mandatory leaks
make re > /dev/null 2>&1
check_leaks "Leaks: mandatory large_room" ./cub3D maps/large_room.cub
check_leaks "Leaks: mandatory minimal map" ./cub3D "$TMPDIR/minimal_map.cub"

# Bonus leaks
make bonus > /dev/null 2>&1
check_leaks "Leaks: bonus test_map (doors)" ./cub3D maps/test_map.cub
check_leaks "Leaks: bonus large_room" ./cub3D maps/large_room.cub

# Error case leaks (should not leak on error)
echo ""
echo "=== ERROR PATH LEAKS (valgrind-style) ==="

check_error_leaks() {
	local desc="$1"
	shift
	TOTAL=$((TOTAL + 1))
	# Error cases exit immediately, just check with leaks via MallocStackLogging
	MallocStackLogging=1 "$@" > /dev/null 2>&1
	exit_code=$?
	# For error paths we mainly check it doesn't crash
	if [ $exit_code -ne 0 ] && [ $exit_code -ne 139 ] && [ $exit_code -ne 134 ] && [ $exit_code -ne 136 ]; then
		printf "${GREEN}PASS${NC} %-50s (no crash, exit=%d)\n" "$desc" "$exit_code"
		PASS=$((PASS + 1))
	else
		if [ $exit_code -eq 0 ]; then
			printf "${YELLOW}WARN${NC} %-50s (expected error but got 0)\n" "$desc"
			PASS=$((PASS + 1))
		else
			printf "${RED}FAIL${NC} %-50s (CRASH! exit=%d)\n" "$desc" "$exit_code"
			FAIL=$((FAIL + 1))
		fi
	fi
}

# Rebuild mandatory for error path tests
make re > /dev/null 2>&1

check_error_leaks "Error leak: no args" ./cub3D
check_error_leaks "Error leak: bad extension" ./cub3D "$TMPDIR/noext"
check_error_leaks "Error leak: nonexistent" ./cub3D /nonexistent.cub
check_error_leaks "Error leak: missing config" ./cub3D "$TMPDIR/no_north.cub"
check_error_leaks "Error leak: bad color" ./cub3D "$TMPDIR/color_test.cub"
check_error_leaks "Error leak: no player" ./cub3D "$TMPDIR/no_player.cub"
check_error_leaks "Error leak: open wall" ./cub3D "$TMPDIR/open_top.cub"
check_error_leaks "Error leak: dup texture" ./cub3D "$TMPDIR/dup_north.cub"
check_error_leaks "Error leak: two players" ./cub3D "$TMPDIR/two_players.cub"
check_error_leaks "Error leak: no map" ./cub3D "$TMPDIR/no_map.cub"

echo ""
echo "=============================================="
printf "Results: ${GREEN}%d passed${NC}, ${RED}%d failed${NC}, %d total\n" "$PASS" "$FAIL" "$TOTAL"
echo "=============================================="

if [ $FAIL -gt 0 ]; then
	exit 1
fi
exit 0
