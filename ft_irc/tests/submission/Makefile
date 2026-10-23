NAME = ircserv
BOT_NAME = ircbot

CXX = c++
CXXFLAGS += -Wall -Wextra -Werror -std=c++98 -pedantic
CPPFLAGS += -Iincludes
DEPFLAGS = -MMD -MP

SRC_DIR = srcs
BUILD_MODE ?= mandatory
OBJ_DIR_ORIGIN := $(origin OBJ_DIR)

ifeq ($(BUILD_MODE),bonus)
OBJ_DIR ?= objs_bonus
CPPFLAGS += -DBONUS
else
OBJ_DIR ?= objs
endif

COMMON_SRCS = main.cpp \
	client/ClientSession.cpp \
	channel/Channel.cpp \
	channel/ChannelManager.cpp \
	protocol/CommandDispatcher.cpp \
	protocol/CommandParser.cpp \
	protocol/ParsedCommand.cpp \
	protocol/ReplyBuilder.cpp \
	protocol/commands/Pass.cpp \
	protocol/commands/Nick.cpp \
	protocol/commands/User.cpp \
	protocol/commands/Cap.cpp \
	protocol/commands/Ping.cpp \
	protocol/commands/Join.cpp \
	protocol/commands/Part.cpp \
	protocol/commands/Privmsg.cpp \
	protocol/commands/Kick.cpp \
	protocol/commands/Invite.cpp \
	protocol/commands/Topic.cpp \
	protocol/commands/Mode.cpp \
	server/Server.cpp

BONUS_SRCS = bonus/FileTransfer.cpp

BOT_OBJ_DIR ?= objs_bot
BOT_SRCS = bonus/IrcBotMain.cpp \
	bonus/IrcBot.cpp
BOT_OBJS = $(BOT_SRCS:%.cpp=$(BOT_OBJ_DIR)/%.o)
BOT_DEPS = $(BOT_OBJS:.o=.d)

ifeq ($(BUILD_MODE),bonus)
SRCS = $(COMMON_SRCS) $(BONUS_SRCS)
else
SRCS = $(COMMON_SRCS)
endif

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)
MODE_STAMP = $(dir $(NAME)).$(notdir $(NAME)).mode

ifeq ($(OBJ_DIR_ORIGIN),undefined)
CLEAN_OBJ_DIRS = objs objs_bonus $(BOT_OBJ_DIR)
else
CLEAN_OBJ_DIRS = $(OBJ_DIR)
endif

.PHONY: all bonus clean fclean re FORCE

all: $(NAME)

bonus:
	$(MAKE) BUILD_MODE=bonus $(NAME) $(BOT_NAME)

$(MODE_STAMP): FORCE
	@mode=$$(cat $@ 2>/dev/null || true); \
	if [ "$$mode" != "$(BUILD_MODE)" ]; then \
		printf '%s\n' "$(BUILD_MODE)" > $@; \
	fi

$(NAME): $(MODE_STAMP) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

$(BOT_NAME): $(BOT_OBJS)
	$(CXX) $(CXXFLAGS) $(BOT_OBJS) -o $@

$(BOT_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	$(RM) -r $(CLEAN_OBJ_DIRS)

fclean: clean
	$(RM) $(NAME) $(BOT_NAME) $(MODE_STAMP)

re: fclean all

-include $(DEPS) $(BOT_DEPS)
