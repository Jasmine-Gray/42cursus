# **************************************************************************** #
#                                                                              #
#                                                       ::::::::        :::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                          +:+      +:+ +:+    #
#    By: mishimod <marvin@42.fr>                        +#+       +#+  +:+     #
#                                                    +#+        +#+#+#+#+#+    #
#    Created: 2025/02/24 21:34:09 by mishimod      #+#               #+#       #
#    Updated: 2025/03/08 20:42:39 by mishimod         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = fractol
cc = CC
FLAGS = -Wall -Wextra -Werror

INCLUDE_DIR = includes
INCLUDES = -I$(INCLUDE_DIR) -I$(LIBFT_DIR) -I$(FT_PRINTF_DIR) -I$(MLX_DIR)

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
FT_PRINTF_DIR = $(LIBFT_DIR)/ft_printf
FT_PRINTF = $(FT_PRINTF_DIR)/libftprintf.a

MLX_DIR = minilibx-linux
MLX_URL = https://cdn.intra.42.fr/document/document/31138/minilibx-linux.tgz
MLX_LIB = minilibx-linux/libmlx.a minilibx-linux/libmlx_Linux.a
MLX_TAR = minilibx-linux.tgz
#MLX_FLAGS = -lX11 -lXext -L$(MLX_DIR) -lmlx -lm 
MLX_FLAGS = -lX11 -lXext -L$(MLX_DIR) -lmlx -lm 

SRCS= 	srcs/main.c
OBJS= 	$(SRCS:.c=.o)

all: $(LIBFT) $(FT_PRINTF) mlx $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)
$(FT_PRINTF):
	$(MAKE) -C $(FT_PRINTF_DIR)

mlx:
	curl -O $(MLX_URL)
	tar -xzvf $(MLX_TAR)
	$(MAKE) -C $(MLX_DIR)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(MLX_FLAGS) $(OBJS) $(LIBFT) $(FT_PRINTF) -o $@ 
	
%.o:%.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS) 
	$(RM) $(MLX_TAR) $(LIBFT) $(FT_PRINTF)
	$(MAKE) -C $(LIBFT_DIR) clean
	$(MAKE) -C $(FT_PRINTF_DIR) clean

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all re clean fclean 

