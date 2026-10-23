TEST_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT_DIR := $(abspath $(TEST_DIR)/..)
TIMEOUT ?= 3
STRICT_TIMEOUT ?= 8
CI_TESTS ?= test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8 \
	test-9 test-10 test-11 test-12 test-13
COLOR ?= auto
VERBOSE ?= 0
PROFILE ?= mandatory
FAILURE_DETAIL_LINES ?= 32
RUNNER_FLAGS = --color $(COLOR) --profile $(PROFILE) \
	--failure-detail-lines $(FAILURE_DETAIL_LINES)
ifeq ($(VERBOSE),1)
RUNNER_FLAGS += --verbose
endif
CASE_test-1 = test-1
CASE_test-2 = test-2
CASE_test-3 = test-3
CASE_test-4 = test-4
CASE_test-5 = test-5
CASE_test-6 = test-6
CASE_test-7 = test-7
CASE_test-8 = test-8
CASE_test-9 = test-9
CASE_test-10 = test-10
CASE_test-11 = test-11
CASE_test-12 = test-12
CASE_test-13 = test-13
CASE_transport-1 = transport-1
CASE_transport-2 = transport-2
CASE_ARGS = $(foreach test,$(1),--case $(CASE_$(test)))
CI_CASE_ARGS = $(call CASE_ARGS,$(CI_TESTS))
VALGRIND ?= valgrind --leak-check=full --show-leak-kinds=all \
	--track-fds=yes --errors-for-leak-kinds=definite,indirect,possible \
	--error-exitcode=99
DOCKER_COMPOSE ?= docker compose -f $(TEST_DIR)/compose.yaml
DOCKER_SERVICE ?= ft_irc
DOCKER_BINARY ?= /tmp/ft_irc_ubuntu/ircserv
DOCKER_OBJ_DIR ?= /tmp/ft_irc_ubuntu/objs
DOCKER_BUILD_IRC = rm -rf /tmp/ft_irc_ubuntu; \
	mkdir -p /tmp/ft_irc_ubuntu; \
	make -s re NAME=$(DOCKER_BINARY) OBJ_DIR=$(DOCKER_OBJ_DIR)
DOCKER_RUN = $(DOCKER_COMPOSE) run --rm -T $(DOCKER_SERVICE) sh -lc
DOCKER_CASE_CMD = set -eu; printf "\\n== %s ==\\n" "[$@] $(TITLE)"; \
	$(DOCKER_BUILD_IRC); python3 tests/scripts/irc_integration_test.py $(ARGS) \
		--run-command "make -f tests/Makefile $@"

.PHONY: all ci image shell check list test test-all strict strict-test-6 \
	strict-test-11 strict-test-12 \
	test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8 test-9 \
	test-10 test-11 test-12 test-13 transport-smoke transport-strict

all: test

image:
	@$(DOCKER_COMPOSE) build

shell: image
	@$(DOCKER_COMPOSE) run --rm $(DOCKER_SERVICE)

check list: TITLE = case list
check list: ARGS = $(RUNNER_FLAGS) --list

ci: TITLE = staged CI gate
ci: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) --timeout $(TIMEOUT) \
	$(CI_CASE_ARGS)

test: TITLE = full suite
test: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) --timeout $(TIMEOUT)

strict: TITLE = strict full suite
strict: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(STRICT_TIMEOUT) --server-wrapper "$(VALGRIND)" \
	--require-valgrind-clean

strict-test-6: TITLE = strict test-6 HUP and disconnect cleanup
strict-test-6: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(STRICT_TIMEOUT) --server-wrapper "$(VALGRIND)" \
	--require-valgrind-clean --case $(CASE_test-6)
strict-test-11: TITLE = strict test-11 operator commands and modes
strict-test-11: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(STRICT_TIMEOUT) --server-wrapper "$(VALGRIND)" \
	--require-valgrind-clean --case $(CASE_test-11)
strict-test-12: TITLE = strict test-12 mass message stability
strict-test-12: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(STRICT_TIMEOUT) --server-wrapper "$(VALGRIND)" \
	--require-valgrind-clean --case $(CASE_test-12)
transport-smoke: TITLE = transport-only socket lifecycle
transport-smoke: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_transport-1)
transport-strict: TITLE = transport-only strict checks
transport-strict: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_transport-1) --case $(CASE_transport-2)

test-1: TITLE = test-1 argument checks
test-1: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-1)
test-2: TITLE = test-2 registration and non-blocking
test-2: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-2)
test-3: TITLE = test-3 fragmented registration
test-3: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-3)
test-4: TITLE = test-4 empty and malformed input
test-4: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-4)
test-5: TITLE = test-5 protocol edge cases
test-5: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-5)
test-6: TITLE = test-6 HUP and disconnect cleanup
test-6: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-6)
test-7: TITLE = test-7 transport hardening
test-7: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-7)
test-8: TITLE = test-8 command burst and buffering
test-8: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-8)
test-9: TITLE = test-9 JOIN and PRIVMSG
test-9: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-9)
test-10: TITLE = test-10 two-client roundtrip
test-10: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-10)
test-11: TITLE = test-11 operator commands and modes
test-11: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-11)
test-12: TITLE = test-12 mass message stability
test-12: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-12)
test-13: TITLE = test-13 review regression coverage
test-13: ARGS = $(RUNNER_FLAGS) --binary $(DOCKER_BINARY) \
	--timeout $(TIMEOUT) --case $(CASE_test-13)

test-all: test-1 test-2 test-3 test-4 test-5 test-6 test-7 test-8 test-9 \
	test-10 test-11 test-12 test-13

ci check list test strict strict-test-6 strict-test-11 test-1 test-2 test-3 test-4 \
strict-test-12 test-5 test-6 test-7 test-8 test-9 test-10 test-11 test-12 test-13 transport-smoke \
transport-strict: image
	@$(DOCKER_RUN) '$(DOCKER_CASE_CMD)'
