CC = cc

CFLAGS = -Wall -Wextra -Werror

NAME = libftprintf.a
LIBFT_DIR = ../Libft
LIBFT = $(LIBFT_DIR)/libft.a

SRCS = ft_printf.c\
			 ft_identify_output.c\
			 ft_digit.c\
			 ft_print_csp.c\
			 ft_print_diu.c\
			 ft_print_xpercent.c\
			 ft_put_fd.c


OBJS=$(SRCS:.c=.o)

all: $(LIBFT) $(NAME)

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME):$(OBJS) $(LIBFT)
	cp $(LIBFT) $(NAME)
	ar rcs $(NAME) $(OBJS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(RM) $(LIBFT)
	@make -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME)
	@make -C $(LIBFT_DIR) clean
	
re: fclean all

.PHONY: all re clean fclean
