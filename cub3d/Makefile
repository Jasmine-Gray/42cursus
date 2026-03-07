# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/18 00:00:00 by tkusama           #+#    #+#              #
#    Updated: 2026/02/28 21:20:35 by mishimod         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= cub3D

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
CFLAGS_DEBUG = -Wall -Wextra -Werror -g -fsanitize=address

# Directories
LIBFT_DIR	= libft
INCS_DIR	= includes
MAND_DIR	= srcs/mandatory
BONUS_DIR	= srcs/bonus
OBJ_DIR		= obj
OBJ_BONUS_DIR = obj_bonus

# Mandatory source files
SRCS		= $(MAND_DIR)/main.c \
			  $(MAND_DIR)/input.c \
			  $(MAND_DIR)/$(CLEANUP_SRC) \
			  $(MAND_DIR)/game_loop.c \
			  $(MAND_DIR)/parser/parser.c \
			  $(MAND_DIR)/parser/parser_read.c \
			  $(MAND_DIR)/parser/parse_config.c \
			  $(MAND_DIR)/parser/parse_color.c \
			  $(MAND_DIR)/parser/parse_map.c \
			  $(MAND_DIR)/parser/validate_map.c \
			  $(MAND_DIR)/parser/parser_utils.c \
			  $(MAND_DIR)/parser/parse_map_utils.c \
			  $(MAND_DIR)/render/draw_utils.c \
			  $(MAND_DIR)/render/raycast.c

# Bonus source files
SRCS_BONUS	= $(BONUS_DIR)/main_bonus.c \
			  $(BONUS_DIR)/input_bonus.c \
			  $(BONUS_DIR)/$(CLEANUP_BONUS_SRC) \
			  $(BONUS_DIR)/$(MOUSE_BONUS_SRC) \
			  $(BONUS_DIR)/game_loop_bonus.c \
			  $(BONUS_DIR)/textures_bonus.c \
			  $(BONUS_DIR)/parser/parser_bonus.c \
			  $(BONUS_DIR)/parser/parser_read_bonus.c \
			  $(BONUS_DIR)/parser/parse_config_bonus.c \
			  $(BONUS_DIR)/parser/parse_color_bonus.c \
			  $(BONUS_DIR)/parser/parse_map_bonus.c \
			  $(BONUS_DIR)/parser/validate_map_bonus.c \
			  $(BONUS_DIR)/parser/parser_utils_bonus.c \
			  $(BONUS_DIR)/parser/parse_map_utils_bonus.c \
			  $(BONUS_DIR)/render/draw_utils_bonus.c \
			  $(BONUS_DIR)/render/minimap_bonus.c \
			  $(BONUS_DIR)/render/raycast_bonus.c \
			  $(BONUS_DIR)/render/sprite_bonus.c

# Object files
OBJS		= $(SRCS:$(MAND_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS_BONUS	= $(SRCS_BONUS:$(BONUS_DIR)/%.c=$(OBJ_BONUS_DIR)/%.o)

# Library
LIBFT		= $(LIBFT_DIR)/libft.a

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    # macOS (OpenGL version)
    MLX_TAR		= minilibx/minilibx_macos_opengl.tgz
    MLX_DIR		= minilibx/minilibx_opengl_20191021
    MLX_FLAGS	= -L$(MLX_DIR) -lmlx -framework OpenGL -framework AppKit
    MLX_LIB		= $(MLX_DIR)/libmlx.a
    CLEANUP_SRC	= cleanup_macos.c
    CLEANUP_BONUS_SRC = cleanup_macos_bonus.c
    MOUSE_BONUS_SRC = mouse_macos_bonus.c
    INC_MAC		= -Iincludes/mac
else
    # Linux
    MLX_TAR		= minilibx/minilibx-linux.tgz
    MLX_DIR		= minilibx/minilibx-linux
    MLX_FLAGS	= -L$(MLX_DIR) -lmlx -lX11 -lXext -lm
    MLX_LIB		= $(MLX_DIR)/libmlx.a $(MLX_DIR)/libmlx_Linux.a
    CLEANUP_SRC	= cleanup_linux.c
    CLEANUP_BONUS_SRC = cleanup_linux_bonus.c
    MOUSE_BONUS_SRC = mouse_linux_bonus.c
    INC_MAC		=
endif

# Include paths (macOS: includes/mac/ overrides includes/)
INC			= -I$(MLX_DIR) $(INC_MAC) -I$(INCS_DIR) -I$(LIBFT_DIR)

all: $(MLX_LIB) $(LIBFT) $(NAME)

$(NAME): $(OBJS)
	@echo "\n[cub3D] Linking..."
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(MLX_FLAGS) -o $(NAME)
	@echo "[cub3D] Build complete!"

$(OBJ_DIR)/%.o: $(MAND_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Bonus rules
bonus: $(MLX_LIB) $(LIBFT) .bonus

.bonus: $(OBJS_BONUS)
	@echo "\n[cub3D bonus] Linking..."
	$(CC) $(CFLAGS) $(OBJS_BONUS) $(LIBFT) $(MLX_FLAGS) -o $(NAME)
	@echo "[cub3D bonus] Build complete!"
	@touch .bonus

$(OBJ_BONUS_DIR)/%.o: $(BONUS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(LIBFT):
	@echo "[libft] Building..."
	@$(MAKE) -C $(LIBFT_DIR)
	@echo "[libft] Done.\n"

$(MLX_LIB): $(MLX_TAR)
	@if [ ! -d "$(MLX_DIR)" ]; then \
		echo "[minilibx] Extracting..."; \
		tar xzf $(MLX_TAR) -C minilibx; \
	fi
	@echo "[minilibx] Building..."
	@$(MAKE) -C $(MLX_DIR) 2>/dev/null || true
	@echo "[minilibx] Done.\n"

clean:
	@echo "[clean] Removing object files..."
	@rm -rf $(OBJ_DIR)
	@rm -rf $(OBJ_BONUS_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "[fclean] Removing executables and libraries..."
	@rm -f $(NAME)
	@rm -f .bonus
	@rm -f $(LIBFT)
	@rm -rf $(MLX_DIR)

re: fclean all

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: re

info:
	@echo "OS: $(UNAME_S)"
	@echo "MLX_DIR: $(MLX_DIR)"
	@echo "MLX_FLAGS: $(MLX_FLAGS)"

.PHONY: all clean fclean re bonus debug info
