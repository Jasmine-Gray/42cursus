# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/28 14:42:29 by mishimod          #+#    #+#              #
#    Updated: 2025/12/21 23:03:31 by mishimod         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= philo

# Compiler settings
CC			= cc
CFLAGS = -Wall -Wextra -Werror

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0 -fsanitize=address
endif

ifeq ($(RACE),1)
	CFLAGS += -g -O0 -fsanitize=thread 
endif


# Directory paths
SRCS_DIR	 = srcs
INCS_DIR	:= includes
OBJ_DIR		 = obj

# Mandatory source files:
SRCS = src/assign_args.c\
	   src/check_args.c\
	   src/init_philo.c\
	   src/init_table.c\
	   src/monitor.c\
	   src/utils/error.c\
	   src/utils/libft.c\
	   src/utils/time.c\
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
