#!/bin/bash

# ===================================================
# WORKING TESTS - These should currently pass
# ===================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NC='\033[0m'

TOTAL=0
PASSED=0
FAILED=0

MINISHELL="./minishell"

test_case() {
    local test_name="$1"
    local input="$2"
    
    TOTAL=$((TOTAL + 1))
    echo -e "${BLUE}Test $TOTAL: $test_name${NC}"
    echo "  Command: '$input'"
    
    # Run bash
    bash_out=$(mktemp)
    bash_err=$(mktemp)
    echo "$input" | bash > "$bash_out" 2> "$bash_err"
    bash_exit=$?
    bash_output=$(cat "$bash_out" | sed '/^$/d')
    rm -f "$bash_out" "$bash_err"
    
    # Run minishell
    minishell_out=$(mktemp)
    minishell_err=$(mktemp)
    echo "$input" | $MINISHELL > "$minishell_out" 2> "$minishell_err"
    minishell_exit=$?
    minishell_output=$(cat "$minishell_out" | grep -v "^minishell\$" | grep -v "^exit$" | sed '/^$/d')
    rm -f "$minishell_out" "$minishell_err"
    
    # Compare outputs
    if [ "$minishell_output" = "$bash_output" ]; then
        echo -e "  Output: ${GREEN}PASS${NC}"
        output_pass=1
    else
        echo -e "  Output: ${RED}FAIL${NC}"
        echo "    Bash:      '$bash_output'"
        echo "    Minishell: '$minishell_output'"
        output_pass=0
    fi
    
    # Compare exit codes
    if [ "$minishell_exit" = "$bash_exit" ]; then
        echo -e "  Exit code: ${GREEN}PASS${NC} ($bash_exit)"
        exit_pass=1
    else
        echo -e "  Exit code: ${RED}FAIL${NC}"
        echo "    Bash:      $bash_exit"
        echo "    Minishell: $minishell_exit"
        exit_pass=0
    fi
    
    if [ "$output_pass" = "1" ] && [ "$exit_pass" = "1" ]; then
        PASSED=$((PASSED + 1))
    else
        FAILED=$((FAILED + 1))
    fi
    echo ""
}

echo "========================================="
echo "   WORKING TESTS - Should Currently Pass"
echo "========================================="
echo ""

# === Basic Echo Commands ===
echo -e "${YELLOW}--- Basic Echo Commands ---${NC}"
test_case "Simple echo" "echo hello"
test_case "Echo multiple words" "echo hello world"
test_case "Echo with tabs" "echo 'hello	world'"
test_case "Echo with multiple spaces" "echo hello     world"
test_case "Echo empty" "echo"
test_case "Echo with -n flag" "echo -n test"

# === Working Quotes ===
echo -e "${YELLOW}--- Working Quotes ---${NC}"
test_case "Single quotes" "echo 'hello world'"
test_case "Double quotes" "echo \"hello world\""
test_case "Mixed quotes" "echo \"it's working\""
test_case "Quotes preserve spaces" "echo \"hello    world\""
test_case "Single quotes no expansion" "echo '\$PATH'"

# === Basic Pipes ===
echo -e "${YELLOW}--- Basic Pipes ---${NC}"
test_case "Simple pipe" "echo hello | cat"
test_case "Pipe chain 2" "echo hello | cat | cat"
test_case "Pipe chain 3" "echo test | cat | cat | cat"
test_case "Pipe with grep" "echo hello | grep hello"
test_case "Pipe with wc" "echo hello world | wc -w"

# === Basic Commands ===
echo -e "${YELLOW}--- Basic Commands ---${NC}"
test_case "PWD command" "pwd"
test_case "ls command" "ls /bin/ls"
test_case "Which command" "which ls"
test_case "Absolute path" "/bin/echo hello"
test_case "Command not found" "nonexistentcommand"

# === Basic Redirections ===
echo -e "${YELLOW}--- Basic Redirections ---${NC}"
test_case "Output redirect" "echo hello > /tmp/test_working1.txt"
test_case "Input redirect" "cat < /etc/passwd | head -1"
test_case "Append redirect" "echo hello >> /tmp/test_working2.txt"
test_case "Redirect to /dev/null" "echo hello > /dev/null"
test_case "Redirect from /dev/null" "cat < /dev/null"
test_case "Redirect to ambiguous filename" "echo hello > /tmp/test_working3.txt"
test_case "Redirect from ambiguous filename" "cat < /tmp/test_working3.txt"
test_case "Redirect only filename" "echo hello > /tmp/test_working3.txt"
test_case "Redirect only filename with quotes" "echo hello > \"/tmp/test_working3.txt\""
test_case "Redirect only infile" "cat < /tmp/test_working3.txt"
test_case "Redirect only infile with quotes" "cat < \"/tmp/test_working3.txt\""
test_case "Redirect only outfile" "> /tmp/test_working3.txt"
test_case "Redirect only outfile with quotes" "> \"/tmp/test_working3.txt\""
test_case "Redirect only infile and outfile" "cat < /tmp/test_working3.txt > /tmp/test_working4.txt"
test_case "Redirect only infile and outfile with quotes" "cat < \"/tmp/test_working3.txt\" > \"/tmp/test_working4.txt\""
test_case "Redirect only infile and outfile with quotes" "cat < \"/tmp/test_working3.txt\" > \"/tmp/test_working4.txt\""

# === Environment Variables ===
echo -e "${YELLOW}--- Environment Variables ---${NC}"
test_case "Echo PATH" "echo \$PATH"
test_case "Echo HOME" "echo \$HOME"
test_case "Echo USER" "echo \$USER"
test_case "Echo undefined var" "echo \$UNDEFINED_VAR_XYZ"
test_case "Variable in double quotes" "echo \"\$USER\""

# === Exit Command ===
echo -e "${YELLOW}--- Exit Command ---${NC}"
test_case "Exit 0" "exit 0"
test_case "Exit 1" "exit 1"
test_case "Exit 42" "exit 42"
test_case "Exit 255" "exit 255"
test_case "Exit -1" "exit -1"
test_case "Exit 123abc" "exit 123abc"
test_case "Exit pipeline" "exit | exit"
test_case "Exit pipeline with args" "exit 1 | exit 1"
test_case "Exit more than 255" "exit 256"
test_case "Exit without args" "exit"
test_case "Exit with invalid arg" "exit a"
test_case "Exit with invalid arg" "exit -a"
test_case "Exit with invalid arg" "exit 123abc"
test_case "Exit more more numeric" "exit 123456789012345678901234567890"
test_case "Exit with multiple args" "exit 1 2 3"
test_case "Exit 2" "exit 2"
test_case "Exit 258 (modulo 256 = 2)" "exit 258"
test_case "Exit 514 (modulo 256 = 2)" "exit 514"
test_case "Exit -254 (negative modulo)" "exit -254"
test_case "Exit with spaces" "exit   42   "
test_case "Exit with tabs" "exit 42"
test_case "Exit INT_MAX" "exit 2147483647"
test_case "Exit INT_MIN" "exit -2147483648"
test_case "Exit LLONG_MAX" "exit 9223372036854775807"
test_case "Exit with plus sign" "exit +42"

# === CD Command ===
echo -e "${YELLOW}--- CD Command ---${NC}"
test_case "CD to HOME" "cd"
test_case "CD to /tmp" "cd /tmp | pwd"
test_case "CD to parent" "cd .. | pwd"
test_case "CD to current" "cd . | pwd"
test_case "CD to nonexistent" "cd /nonexistent/path"
test_case "CD with multiple args" "cd /tmp /usr"
test_case "CD with spaces in path" "cd '/tmp'"

# === Export Command ===
echo -e "${YELLOW}--- Export Command ---${NC}"
test_case "Export simple variable" "export TEST=hello | echo \$TEST"
test_case "Export with equals in value" "export TEST='a=b' | echo \$TEST"
test_case "Export without value" "export TEST | echo \$TEST"
test_case "Export multiple vars" "export A=1 B=2 | echo \$A \$B"
test_case "Export with spaces" "export TEST='hello world' | echo \$TEST"
test_case "Export numeric variable" "export TEST=123 | echo \$TEST"
test_case "Export empty value" "export TEST= | echo '\$TEST is empty'"
test_case "Export special chars" "export TEST='!@#\$%^&*()' | echo \$TEST"

# === Unset Command ===
echo -e "${YELLOW}--- Unset Command ---${NC}"
test_case "Unset variable" "export TEST=hello | unset TEST | echo \$TEST"
test_case "Unset multiple vars" "export A=1 B=2 | unset A B | echo \$A \$B"
test_case "Unset nonexistent" "unset NONEXISTENT_VAR"
test_case "Unset PATH" "unset PATH | echo \$PATH"

# === ENV Command ===
echo -e "${YELLOW}--- ENV Command ---${NC}"
test_case "Env output" "env | grep -c PATH"
test_case "Env with pipe" "env | head -5 | wc -l"
test_case "Env with grep" "env | grep USER"

# === Special Characters ===
echo -e "${YELLOW}--- Special Characters ---${NC}"
test_case "Asterisk in quotes" "echo '*'"
test_case "Question mark in quotes" "echo '?'"
test_case "Dollar in quotes" "echo '\$'"
test_case "Pipe in quotes" "echo '|'"
test_case "Redirect in quotes" "echo '>'"

# === Whitespace Handling ===
echo -e "${YELLOW}--- Whitespace Handling ---${NC}"
test_case "Leading spaces" "   echo hello"
test_case "Trailing spaces" "echo hello   "
test_case "Tabs and spaces" "echo hello"
test_case "Empty command" ""
test_case "Only spaces" "   "

# === Additional Redirections ===
echo -e "${YELLOW}--- Additional Redirections ---${NC}"
test_case "Multiple output redirects" "echo hello > /tmp/test1.txt > /tmp/test2.txt"
test_case "Multiple input redirects" "cat < /etc/passwd < /etc/hosts"
test_case "Redirect with no space" "echo hello>/tmp/test.txt"
test_case "Redirect in middle of command" "echo > /tmp/mid.txt hello"
test_case "Redirect at beginning" "> /tmp/begin.txt echo hello"

# === Additional Pipes ===
echo -e "${YELLOW}--- Additional Pipes ---${NC}"
test_case "Pipe with error command" "nonexistent | echo hello"
test_case "Long pipe chain" "echo test | cat | cat | cat | cat | cat"
test_case "Pipe with builtin" "echo hello | cd /tmp"
test_case "Pipe with exit" "echo hello | exit 42"
test_case "Pipe with export" "echo TEST=value | export"

# === Additional Environment Variables ===
echo -e "${YELLOW}--- Additional Environment Variables ---${NC}"
test_case "Multiple variables" "echo \$USER \$HOME \$PATH"
test_case "Empty variable expansion" "echo \$NONEXISTENT"
test_case "Variable in single quotes" "echo '\$USER'"

# === Additional Quotes ===
echo -e "${YELLOW}--- Additional Quotes ---${NC}"
test_case "Nested quotes" "echo \"'hello'\""
test_case "Empty quotes" "echo '' \"\""
test_case "Mixed quote types" "echo 'single' \"double\" 'again'"
test_case "Quotes with variables" "echo '\$USER' \"\$USER\""

# === Edge Cases ===
echo -e "${YELLOW}--- Edge Cases ---${NC}"

# === Multiple Arguments ===
echo -e "${YELLOW}--- Multiple Arguments ---${NC}"
test_case "Many arguments" "echo a b c d e f g h i j"
test_case "Arguments with quotes" "echo 'arg1' \"arg2\" arg3"
test_case "Mixed quotes and args" "echo hello 'world' \"test\" end"

# === Complex Scenarios ===
echo -e "${YELLOW}--- Complex Scenarios ---${NC}"
test_case "Pipe chain with redirect" "cat /etc/passwd | grep root | head -1 > /tmp/root_user.txt"
test_case "Multiple redirects with pipe" "echo hello > /tmp/t1.txt | cat < /tmp/t1.txt"
test_case "Export and echo in subshell" "export MYVAR=test | echo \$MYVAR"
test_case "CD and pwd" "cd /tmp | pwd"
test_case "Complex quote mixing" "echo 'single' \"double with \$USER\" 'back to single'"
test_case "Long pipe with redirects" "cat /etc/passwd | grep -v '#' | head -5 | tail -2 > /tmp/users.txt"
test_case "Exit in pipe chain" "echo start | cat | exit 42 | echo end"
test_case "Builtin with external" "cd /tmp | ls | grep tmp"
test_case "Variable in redirect path" "echo test > /tmp/test_\$USER.txt"
test_case "Multiple exports" "export A=1 | export B=2 | export C=3"
test_case "Export then unset" "export TEST=hello | echo \$TEST | unset TEST"
test_case "CD with paths" "cd / | cd tmp | pwd"
test_case "All quote types" "echo 'single' \"double\" bare \$USER '\$USER' \"\$USER\""
test_case "Error in pipe" "ls /nonexistent 2>/dev/null | echo 'continues' | cat"
test_case "Complex redirect order" "> /tmp/out.txt cat < /etc/passwd | head -1"
test_case "Whitespace preservation" "echo '   spaces   ' \"	tabs	\""
test_case "Empty var in pipe" "echo \$NONEXISTENT | cat | wc -l"
test_case "Multiple cd" "cd / | cd usr | cd bin | pwd"
test_case "Nested pipes complex" "echo hello | cat | grep h | wc -l"
test_case "Redirect and pipe mix" "< /etc/passwd grep root | head -1 > /tmp/test.txt"
test_case "Export with spaces" "export TEST='hello world' | echo \$TEST"
test_case "Unset PATH effect" "echo \$PATH | unset PATH | ls"
test_case "Echo exit codes" "exit 5 | echo \$?"
test_case "Pipe builtin to builtin" "echo test | cd /tmp"
test_case "Complex variable expansion" "echo start\$USER:end:\$HOME"
test_case "Multiple redirects same file" "> /tmp/test.txt echo first > /tmp/test.txt"
test_case "Pipe with all builtins" "pwd | env | export TEST=value"
test_case "Mixed redirect types" "< /etc/passwd cat | grep root > /tmp/out.txt"
test_case "Exit values in pipes" "exit 1 | exit 2 | exit 3"

# === Summary ===
echo "========================================="
echo "Test Summary (WORKING TESTS):"
echo -e "Total:  $TOTAL"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"

if [ $TOTAL -gt 0 ]; then
    echo -e "Pass Rate: $(echo "scale=2; $PASSED * 100 / $TOTAL" | bc)%"
fi

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All working tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some working tests failed - these should be fixed!${NC}"
    exit 1
fi
