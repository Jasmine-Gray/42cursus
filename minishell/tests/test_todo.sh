#!/bin/bash

# ===================================================
# TODO TESTS - Known issues to be fixed
# ===================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
MAGENTA='\033[0;35m'
NC='\033[0m'

TOTAL=0
PASSED=0
FAILED=0

MINISHELL="./minishell"

test_case() {
    local test_name="$1"
    local input="$2"
    local issue_number="$3"  # GitHub issue number
    
    TOTAL=$((TOTAL + 1))
    echo -e "${BLUE}Test $TOTAL: $test_name${NC}"
    if [ -n "$issue_number" ]; then
        echo -e "  ${MAGENTA}Issue: #$issue_number${NC}"
    fi
    echo "  Command: '$input'"
    
    # Run bash
    bash_out=$(mktemp)
    bash_err=$(mktemp)
    echo "$input" | bash > "$bash_out" 2> "$bash_err"
    bash_exit=$?
    bash_output=$(cat "$bash_out" | sed '/^$/d')
    bash_stderr=$(cat "$bash_err")
    rm -f "$bash_out" "$bash_err"
    
    # Run minishell
    minishell_out=$(mktemp)
    minishell_err=$(mktemp)
    echo "$input" | $MINISHELL > "$minishell_out" 2> "$minishell_err"
    minishell_exit=$?
    minishell_output=$(cat "$minishell_out" | grep -v "^minishell\$" | grep -v "^exit$" | sed '/^$/d')
    minishell_stderr=$(cat "$minishell_err")
    rm -f "$minishell_out" "$minishell_err"
    
    # Compare outputs
    if [ "$minishell_output" = "$bash_output" ]; then
        echo -e "  Output: ${GREEN}PASS${NC}"
        output_pass=1
    else
        echo -e "  Output: ${RED}FAIL (KNOWN ISSUE)${NC}"
        echo "    Expected:  '$bash_output'"
        echo "    Got:       '$minishell_output'"
        output_pass=0
    fi
    
    # Compare exit codes
    if [ "$minishell_exit" = "$bash_exit" ]; then
        echo -e "  Exit code: ${GREEN}PASS${NC} ($bash_exit)"
        exit_pass=1
    else
        echo -e "  Exit code: ${RED}FAIL (KNOWN ISSUE)${NC}"
        echo "    Expected:  $bash_exit"
        echo "    Got:       $minishell_exit"
        exit_pass=0
    fi
    
    # Show stderr if different
    if [ "$bash_stderr" != "$minishell_stderr" ] && [ -n "$bash_stderr" ]; then
        echo -e "  ${YELLOW}Stderr differs:${NC}"
        echo "    Expected:  '$bash_stderr'"
        echo "    Got:       '$minishell_stderr'"
    fi
    
    if [ "$output_pass" = "1" ] && [ "$exit_pass" = "1" ]; then
        PASSED=$((PASSED + 1))
        echo -e "  ${GREEN}✓ This issue has been fixed!${NC}"
    else
        FAILED=$((FAILED + 1))
    fi
    echo ""
}

echo "========================================="
echo "   TODO TESTS - Known Issues to Fix"
echo "========================================="
echo ""

# === Priority 1: CRITICAL - Shell Crashes ===
echo -e "${RED}=== CRITICAL: Shell Crashes ===${NC}"
echo "These issues cause minishell to terminate unexpectedly"
echo ""

# Manual tests - cannot be automated
echo -e "${YELLOW}MANUAL TEST REQUIRED:${NC}"
echo "1. Issue #31: cat << EOF then Ctrl+C (should not exit minishell)"
echo "2. Issue #32: cat then Ctrl+C or Ctrl+\\ (should not exit minishell)"
echo ""

# === Priority 2: HIGH - Incorrect Behavior ===
echo -e "${YELLOW}=== HIGH: Incorrect Behavior ===${NC}"

test_case "Empty double quotes as command" '""' "26"
test_case "Empty single quotes as command" "''" "26"
test_case "Invalid pipe chain output" "a | b | c" "33"
test_case "Pipe syntax error status" "| > |" "35"
test_case "PWD not working in some contexts" "cd / | pwd" ""
test_case "CD chain not working" "cd /tmp | cd / | pwd" ""

# === Priority 3: MEDIUM - Validation Issues ===
echo -e "${YELLOW}=== MEDIUM: Validation Issues ===${NC}"

test_case "Unset empty double quotes" 'unset ""' "27"
test_case "Unset empty single quotes" "unset ''" "27"
test_case "Export numeric identifier" "export 123=test" "30"
test_case "Export invalid char @" "export @var=test" "30"
test_case "Export with dash" "export test-var=value" "30"
test_case "Exit huge number" "exit 99999999999999999999999999999999999" "34"
test_case "Exit negative number" "exit -42" ""
test_case "Exit with string" "exit abc" ""

# === Priority 4: Variable & Redirect Issues ===
echo -e "${YELLOW}=== Variable & Redirect Issues ===${NC}"

test_case "Redirect to undefined var" "> \$NONEXISTENT" "36"
test_case "Redirect to empty var" "echo test > \$EMPTY_VAR" "40"
test_case "Redirect var with spaces" 'export A="a b"; > $A' "38"
test_case "Multiple dollar signs" 'echo $$$$$$$USER' "37"
test_case "Variable as command" 'export A="echo hello"; $A' "39"
test_case "Dollar quote syntax" 'echo $"aaa"' "29"
test_case "Double dollar" 'echo $$' ""

# === Priority 5: Heredoc Issues ===
echo -e "${YELLOW}=== Heredoc Issues ===${NC}"

test_case "Heredoc quoted delimiter double" 'cat << "EOF"
$USER
EOF' "28"
test_case "Heredoc quoted delimiter single" "cat << 'EOF'
\$USER
EOF" "28"
test_case "Simple heredoc" "cat << EOF
hello world
EOF" ""
test_case "Heredoc with expansion" "cat << EOF
\$HOME
EOF" ""

# === Priority 6: Command Execution ===
echo -e "${YELLOW}=== Command Execution ===${NC}"

test_case "Env with argument" "env nonexistentcmd" "42"
test_case "Env with single char" "env a" "42"
test_case "Builtin in pipe" "export | head -1" "41"
test_case "CD in pipe" "cd /tmp | pwd" "41"

# === Priority 7: Advanced Features (Not Implemented) ===
echo -e "${YELLOW}=== Not Implemented Features ===${NC}"
echo "These are explicitly not supported:"
echo ""

test_case "AND operator" "echo hello && echo world" ""
test_case "OR operator" "echo hello || echo world" ""
test_case "Semicolon separator" "echo hello; echo world" ""
test_case "Background process" "echo hello &" ""
test_case "Command substitution" '$(echo hello)' ""
test_case "Backticks" '`echo hello`' ""
test_case "Arithmetic expansion" 'echo $((2+2))' ""
test_case "Brace expansion" 'echo {1..5}' ""
test_case "Glob expansion" "echo *.txt" ""
test_case "Process substitution" "cat <(echo hello)" ""

# === Summary ===
echo "========================================="
echo "TODO Test Summary:"
echo -e "Total:  $TOTAL"
echo -e "Fixed:  ${GREEN}$PASSED${NC}"
echo -e "Todo:   ${RED}$FAILED${NC}"

if [ $TOTAL -gt 0 ]; then
    echo -e "Fix Rate: $(echo "scale=2; $PASSED * 100 / $TOTAL" | bc)%"
fi

echo ""
echo -e "${YELLOW}Note:${NC}"
echo "- CRITICAL issues should be fixed first (crashes)"
echo "- Run 'bash test_working.sh' for tests that should pass"
echo "- Check GitHub issues for detailed descriptions"
echo ""

if [ $PASSED -gt 0 ]; then
    echo -e "${GREEN}Good news: $PASSED issues have been fixed!${NC}"
fi