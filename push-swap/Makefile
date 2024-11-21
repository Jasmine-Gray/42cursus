NAME = push_swap

CC = cc
#FLAGS = -Wall -Wextra -Werror 
FLAGS = -Wall -Wextra -Werror -g -fsanitize=address

SRCS_DIR = srcs
INCLUDES_DIR = includes
LIBFT_DIR = libft

LIBFT = libft/libft.a

LISTS_SRCS = 		srcs/lists/double_list_add_back.c\
					srcs/lists/double_list_add_front.c\
					srcs/lists/find_last_node.c\
					srcs/lists/node_new.c\
					srcs/lists/detach_node.c\
					srcs/lists/count_node.c\
					srcs/main.c\

COMMANDS_SRCS = 	srcs/commands/push.c\
					srcs/commands/rotate.c\
					srcs/commands/reverse_rotate.c\
					srcs/commands/swap.c\

UTILS_SRCS  =	 	srcs/utils/build_stack.c\
					srcs/utils/check_args.c\
					srcs/utils/push_swap_main.c\
					
SORT_SRCS  = 		srcs/sort/insert_pivot.c\
					srcs/sort/sort.c\
					srcs/sort/sort_utils.c\
					srcs/sort/swap_left_pivot.c\
					srcs/sort/swap_left_right.c\
					srcs/sort/is_sort.c\
					
SRCS = $(LISTS_SRCS) $(COMMANDS_SRCS) $(UTILS_SRCS) $(SORT_SRCS)

INCLUDES = -I $(INCLUDES_DIR)


OBJS = $(SRCS:.c=.o)

all: $(LIBFT) $(NAME)

$(LIBFT): 
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS)
	@echo "$(OBJS)"
	$(CC) $(FLAGS) $(OBJS) -o $@ $(LIBFT)

$(SRCS_DIR)/%.o:$(SRCS_DIR)/%.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean: 
	$(RM) $(OBJS)
	$(RM) $(LIBFT)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME)

re: fclean all 

.PHONY: all clean fclean re

