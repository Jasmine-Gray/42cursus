NAME = minishell
cc = CC
FLAGS = -Wall -Wextra -Werror

INCLUDE_DIR = ./includes
INCLUDES = -I$(INCLUDE_DIR) -I$(LIBFT_DIR) -I$(FT_PRINTF_DIR)

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
FT_PRINTF_DIR = $(LIBFT_DIR)/ft_printf
FT_PRINTF = $(FT_PRINTF_DIR)/libftprintf.a

SRCS= 	builtin/pwd.c\
	 	builtin/echo.c\
		builtin/env.c\

OBJS= 	$(SRCS:.c=.o)

all: $(LIBFT) $(FT_PRINTF) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)
$(FT_PRINTF):
	$(MAKE) -C $(FT_PRINTF_DIR)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(FT_PRINTF) $(LIBFT) -o $@ 
	
%.o:%.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS) 
	$(RM) -r $(LIBFT) $(FT_PRINTF)
	$(MAKE) -C $(LIBFT_DIR) clean
	$(MAKE) -C $(FT_PRINTF_DIR) clean

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all re clean fclean 

