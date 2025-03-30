# **************************************************************************** #
#                                                                              #
#                                                       ::::::::        :::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                          +:+      +:+ +:+    #
#    By: mishimod <marvin@42.fr>                        +#+       +#+  +:+     #
#                                                    +#+        +#+#+#+#+#+    #
#    Created: 2025/02/24 21:34:09 by mishimod      #+#               #+#       #
#    Updated: 2025/03/30 18:54:55 by mishimod         ###   ########.fr        #
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
MLX_URL = https://cdn.intra.42.fr/document/document/32190/minilibx-linux.tgz
MLX_LIB = minilibx-linux/libmlx.a minilibx-linux/libmlx_Linux.a
MLX_TAR = minilibx-linux.tgz
#MLX_FLAGS = -lX11 -lXext -L$(MLX_DIR) -lmlx -lm 
MLX_FLAGS = -lX11 -lXext -L$(MLX_DIR) -lm -lmlx
#MLX_FLAGS = -L/opt/X11/lib -lX11 -lXext -L$(MLX_DIR) -lm -lmlx

SRCS= 	srcs/main.c\
	 	srcs/ft_atof.c\
		srcs/mlx_practice.c\

OBJS= 	$(SRCS:.c=.o)

all: $(LIBFT) $(FT_PRINTF) mlx $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)
$(FT_PRINTF):
	$(MAKE) -C $(FT_PRINTF_DIR)

mlx:
	@if [ ! -e $(MLX_TAR) ]; then\
		curl -O $(MLX_URL);\
		tar -xvf $(MLX_TAR);\
		$(MAKE) -C $(MLX_DIR);\
	fi

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(MLX_FLAGS) $(FT_PRINTF) $(LIBFT) -o $@ 
	#$(CC) $(FLAGS) $(OBJS) $(MLX_FLAGS) $(LIBFT) $(FT_PRINTF) -o $@
	
%.o:%.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS) 
	$(RM) -r $(MLX_TAR) $(MLX_DIR) $(LIBFT) $(FT_PRINTF)
	$(MAKE) -C $(LIBFT_DIR) clean
	$(MAKE) -C $(FT_PRINTF_DIR) clean

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all re clean fclean 






# NAME		= fractol
# 
# CC		= cc
# CFLAGS		= -Wall -Wextra -Werror
# CFLAGS_DEBUG = -Wall -Wextra -Werror -g -fsanitize=address
# 
# LIBFT_DIR 	:= libft
# FT_PRINTF_DIR 	:= libft/ft_printf
# SRCS_DIR	 = ./srcs
# INCS_DIR	:= ./includes
# OBJ_DIR		 = obj
# 
# SRCS		= $(addprefix $(SRCS_DIR)/, main.c)
# # search path
# vpath %.c $(SRCS_DIR) $(SRCS_DIR)/fractol $(SRCS_DIR)/utils
# 
# OBJS		= $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))
# 
# #minilibx
# MLX_URL		:= https://cdn.intra.42.fr/document/document/32190/minilibx-linux.tgz
# MLX_TAR_GZ	:= minilibx-linux.tgz
# MLX_DIR		:= ./minilibx-linux
# 
# # library
# MLX		:= $(MLX_DIR)/libmlx_Linux.a $(MLX_DIR)/libmlx.a
# LIBFT		 = $(LIBFT_DIR)/libft.a
# 
# # compile option (Linker two mlx library file and X11 library)
# #MLX_FLAGS 	 = -lX11 -lXext -L$(MLX_DIR) -lm -lmlx 
# MLX_FLAGS = -L/opt/X11/lib -lX11 -lXext -L$(MLX_DIR) -lm -lmlx
# 
# INC		 = -I$(MLX_DIR) -I$(INCS_DIR) -I$(LIBFT_DIR) -I$(FT_PRINTF_DIR)
# 
# all: $(OBJ_DIR) $(MLX) $(LIBFT) $(NAME)
# 
# $(NAME): $(OBJS) 
# 	@echo "\n$(NAME) code is Compiling ..."
# 	$(CC) $(CFLAGS) $(OBJS) $(MLX_FLAGS) -o $(NAME) $(LIBFT)
# 	@echo "\n$(NAME) is ready!"
# 
# $(OBJ_DIR):
# 	@mkdir -p $(OBJ_DIR)
# 
# $(LIBFT):
# 	$(MAKE) -C $(LIBFT_DIR)
# 	@echo "\n"
# 
# $(MLX_TAR_GZ):
# 	curl -O $(MLX_URL)
# 
# $(MLX): $(MLX_TAR_GZ)
# 	tar xzf $(MLX_TAR_GZ)
# 	$(MAKE) -j4 -C $(MLX_DIR)
# 
# $(OBJ_DIR)/%.o: %.c
# 	@$(CC) $(CFLAGS) $(INC) -c $< -o $@
# 	@echo "$(CC) $(CFLAGS) $(INC) -c $< -o $@"
# 
# clean:
# 	@echo "Cleaning object files..."
# 	@$(MAKE) -C $(LIBFT_DIR) clean
# 	$(RM) $(LIBFT)
# 	@echo "Cleaning dir..."
# 	@rm -rf $(OBJ_DIR)
# 	@rm -rf $(MLX_DIR)
# 
# fclean: clean
# 	@echo "Cleaning executables"
# 	@rm -f $(MLX_TAR_GZ)
# 	$(RM) $(NAME)
# 
# re: fclean all
# 
# .PHONY: all clean fclean re
