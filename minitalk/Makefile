NAME = minitalk

cc = CC
FLAGS = -Wall -Wextra -Werror

SRC_DIR = srcs
INCLUDE_DIR = includes
PRINTF_DIR = printf
PRINTF = printf/libftprintf.a

SRCS = 	srcs/server.c\

INCLUDES = -I $(INCLUDE_DIR)

OBJS = $(SRCS:.c=.o)


all: $(PRINTF) $(NAME)

$(PRINTF):
	@$(MAKE) -C $(PRINTF_DIR)

$(NAME): $(OBJS)
	@echo "$(OBJS)"
	$(CC) $(FLAGS) $(OBJS) -o $@ $(PRINTF)

$(SRC_DIR)/%.o:$(SRC_DIR)/%.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@


clean:
	$(RM) $(OBJS)
	$(RM) $(PRINTF)
	@$(MAKE) -C $(PRINTF_DIR) clean

fclean: clean
	$(RM) $(NAME)
	@$(MAKE) -C $(PRINTF_DIR) clean

re: fclean clean

.PHONY: all re clean fclean 

