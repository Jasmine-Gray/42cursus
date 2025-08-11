# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/11 19:45:27 by tkusama           #+#    #+#              #
#    Updated: 2025/08/11 21:42:55 by tkusama          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= minishell

# Compiler settings
CC			= cc

ifeq ($(DEBUG),1)
	CFLAGS = -Wall -Wextra -Werror -g -O0 -fsanitize=address
else
	CFLAGS = -Wall -Wextra -Werror
endif


# Directory paths
SRCS_DIR	 = srcs
INCS_DIR	:= includes
LIBFT_DIR	      := libft
FT_PRINTF_DIR := $(LIBFT_DIR)/ft_printf
OBJ_DIR		 = obj

# static library
LIBFT 				= $(LIBFT_DIR)/libft.a
FT_PRINTF     = $(FT_PRINTF_DIR)/libftprintf.a

# Mandatory source files:
SRCS = src/builtin/builtin_cd.c\
	   src/builtin/builtin_cd_expand.c\
	   src/builtin/builtin_cd_helper.c\
	   src/builtin/builtin_cd_utils.c\
	   src/builtin/builtin_echo.c\
	   src/builtin/builtin_env.c\
	   src/builtin/builtin_exit.c\
	   src/builtin/builtin_export.c\
	   src/builtin/builtin_pwd.c\
	   src/builtin/builtin_unset.c\
	   src/env/env_map.c\
	   src/env/env_to_array.c\
	   src/env/map_get.c\
	   src/env/map_put.c\
	   src/env/map_set.c\
	   src/execute/execute_ast.c\
	   src/execute/execute_ast_utils.c\
	   src/execute/execute_builtin.c\
	   src/execute/execute_external.c\
	   src/execute/execute_external_utils.c\
	   src/execute/execute_pipe.c\
	   src/execute/execute_pipe_utils.c\
	   src/execute/handle_redirects.c\
	   src/execute/process_status.c\
	   src/execute/redirect_utils.c\
	   src/execute/search_builtin.c\
	   src/expand/expand_variable.c\
	   src/expand/get_variable_name.c\
	   src/expand/get_variable_value.c\
	   src/memory/ctx_memory.c\
	   src/memory/ctx_strjoin.c\
	   src/memory/memory_pool.c\
	   src/parse/ast_node.c\
	   src/parse/heredoc_all.c\
	   src/parse/heredoc_process.c\
	   src/parse/heredoc_utils.c\
	   src/parse/parse.c\
	   src/parse/parse_command.c\
	   src/parse/parse_command_process.c\
	   src/parse/parse_command_utils.c\
	   src/parse/parse_utils.c\
	   src/parse/redirect.c\
	   src/repl/interpret.c\
	   src/repl/run_repl.c\
	   src/signal/signal_handler.c\
	   src/signal/signal_util.c\
	   src/tokenize/add_word_to_list.c\
	   src/tokenize/extract_concatenated_word.c\
	   src/tokenize/extract_operator.c\
	   src/tokenize/extract_quoted_part.c\
	   src/tokenize/extract_unquoted_part.c\
	   src/tokenize/get_flags.c\
	   src/tokenize/get_quote.c\
	   src/tokenize/is_operator.c\
	   src/tokenize/tokenize.c\
	   src/util/fatal_error.c\
	   src/util/ft_realloc.c\
	   src/util/ft_strcmp.c\
	   src/util/ft_strndup.c\
	   src/util/ft_strtok.c\
	   src/util/is_builtin.c\
	   src/util/is_space.c\
	   src/util/identifier.c\
	   main.c\

# Object files for Mandatory sources
MANDATORY_OBJS = $(SRCS:.c=.o)
MANDATORY_OBJS := $(addprefix $(OBJ_DIR)/, $(MANDATORY_OBJS))

# Detect OS
UNAME_S := $(shell uname -s)

# Readline configuration
ifeq ($(UNAME_S),Darwin)
	# macOS with Homebrew
	RLDIR    = $(shell brew --prefix readline)
	LIB      = -L$(FT_PRINTF_DIR) -lftprintf -L$(LIBFT_DIR) -lft -L$(RLDIR)/lib -lreadline
	INC      = -I$(INCS_DIR) -I$(LIBFT_DIR) -I$(FT_PRINTF_DIR) -I$(RLDIR)/include
else
	# Ubuntu/Linux
	LIB      = -L$(FT_PRINTF_DIR) -lftprintf -L$(LIBFT_DIR) -lft -lreadline
	INC      = -I$(INCS_DIR) -I$(LIBFT_DIR) -I$(FT_PRINTF_DIR)
endif

all: $(NAME)

$(NAME): $(MANDATORY_OBJS) $(LIBFT) $(FT_PRINTF)
	@echo "\n\nBuilding $(NAME)..."
	$(CC) $(CFLAGS) $(MANDATORY_OBJS) -o $(NAME) $(LIB)
	@echo "\n$(NAME) is ready!"

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

$(FT_PRINTF):
	@$(MAKE) -C $(FT_PRINTF_DIR)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@) 
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	@echo "Cleaning object files..."
	@$(MAKE) -C $(LIBFT_DIR) clean
	@$(MAKE) -C $(FT_PRINTF_DIR) clean
	$(RM) $(LIBFT)
	$(RM) $(FT_PRINTF)
	@echo "Cleaning dir..."
	$(RM) -r $(OBJ_DIR)

fclean: clean
	@echo "Cleaning $(NAME)"
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
