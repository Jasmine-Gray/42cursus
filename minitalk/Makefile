NAME = minitalk

cc = CC
FLAGS = -Wall -Wextra -Werror

LIBFT_DIR = libft
LIBFT = libft/libft.a

SRC = 	client.c\
		server.c\

OBJS = $(SRCS:.c=.o)

NAME = minitalk

all: $(LIBFT) $(NAME)

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS)
	@echo "$(OBJS)"
	$(CC) $(FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(RM) $(LIBFT)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME)

re: fclean clean

.PHONY: all re clean fclean 

