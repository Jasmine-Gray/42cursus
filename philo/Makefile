# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/28 14:42:29 by mishimod          #+#    #+#              #
#    Updated: 2025/11/27 21:41:23 by mishimod         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= philo

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
SRCS = src/check_args.c\
	   src/assign_args.c\
	   src/init_table.c\
	   src/utils/error.c\
	   src/utils/libft.c\
	   main.c\

# Object files for Mandatory sources
MANDATORY_OBJS = $(SRCS:.c=.o)
MANDATORY_OBJS := $(addprefix $(OBJ_DIR)/, $(MANDATORY_OBJS))

# Detect OS
UNAME_S := $(shell uname -s)

# Readline configuration
ifeq ($(UNAME_S),Darwin)
	# macOS with Homebrew
	INC      = -I$(INCS_DIR) 
else
	# Ubuntu/Linux
	INC      = -I$(INCS_DIR)
endif

all: $(NAME)

$(NAME): $(MANDATORY_OBJS) 
	@echo "\n\nBuilding $(NAME)..."
	$(CC) $(CFLAGS) $(MANDATORY_OBJS) -o $(NAME)
	@echo "\n$(NAME) is ready!"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@) 
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	@echo "Cleaning dir..."
	$(RM) -r $(OBJ_DIR)

fclean: clean
	@echo "Cleaning $(NAME)"
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
