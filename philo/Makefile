# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/28 14:42:29 by mishimod          #+#    #+#              #
#    Updated: 2025/08/28 14:51:26 by mishimod         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= philosophers

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
OBJ_DIR		 = obj

# Mandatory source files:
SRCS = main.c

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
