# **************************************************************************** #
#                                                                              #
#                                                       ::::::::        :::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                          +:+      +:+ +:+    #
#    By: mishimod <marvin@42.fr>                        +#+       +#+  +:+     #
#                                                    +#+        +#+#+#+#+#+    #
#    Created: 2025/02/24 21:34:09 by mishimod      #+#               #+#       #
#    Updated: 2025/02/27 19:00:08 by mishimod         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = fract_ol
cc = CC
FLAGS = -Wall -Wextra -Werror

INCLUDE_DIR = includes
PRINTF_DIR = printf
PRINTF = printf/libftprintf.a

MLX_URL = https://cdn.intra.42.fr/document/document/31138/minilibx-linux.tgz
MLX_A = minilibx-linux/libmlx.a minilibx-linux/libmlx_Linux.a
MLX_TAR = minilibx-linux.tgz

SRCS_DIR = srcs
SRC_SERVER = 	$(SRCS_DIR)/server.c
SRC_CLIENT = 	$(SRCS_DIR)/client.c

INCLUDES = -I$(INCLUDE_DIR)

OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

all: $(PRINTF) $(MLX) $(NAME)

$(PRINTF):
	@$(MAKE) -C $(PRINTF_DIR)

$(MLX):
	curl -O $(MLX_URL)
	tar -xzvf $(MLX_TAR)

$(NAME): $(NAME_SERVER) $(NAME_CLIENT)

$(NAME_SERVER): $(OBJ_SERVER)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJ_SERVER) -o $@ $(PRINTF)
	
$(NAME_CLIENT): $(OBJ_CLIENT)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJ_CLIENT) -o $@ $(PRINTF)

$(SRCS_DIR)/%.o:$(SRCS_DIR)/%.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJ_SERVER) $(OBJ_CLIENT) 
	$(RM) $(PRINTF)
	@$(MAKE) -C $(PRINTF_DIR) clean

fclean: clean
	$(RM) $(NAME_SERVER) $(NAME_CLIENT) 
	@$(MAKE) -C $(PRINTF_DIR) clean


re: fclean all

.PHONY: all re clean fclean 

