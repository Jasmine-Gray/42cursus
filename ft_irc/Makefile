NAME = ircserv
BOT_NAME = ircbot

CXX = c++
CXXFLAGS += -Wall -Wextra -Werror -std=c++98 -pedantic
CPPFLAGS += -Iincludes

SRC_DIR = srcs
OBJ_DIR ?= objs

SRCS = main.cpp \
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
	protocol/commands/Quit.cpp \
	protocol/commands/Join.cpp \
	protocol/commands/Part.cpp \
	protocol/commands/Names.cpp \
	protocol/commands/Who.cpp \
	protocol/commands/Privmsg.cpp \
	protocol/commands/Kick.cpp \
	protocol/commands/Invite.cpp \
	protocol/commands/Topic.cpp \
	protocol/commands/Mode.cpp \
	bonus/FileTransfer.cpp \
	server/Server.cpp

BOT_SRCS = bonus/IrcBotMain.cpp bonus/IrcBot.cpp

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
BOT_OBJS = $(BOT_SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME) $(BOT_NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(BOT_NAME): $(BOT_OBJS)
	$(CXX) $(CXXFLAGS) $(BOT_OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r objs $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME) $(BOT_NAME)

re: fclean all

.PHONY: all clean fclean re
