CC = cc

CFLAGS = -Wall -Wextra -Werror

NAME = libftprintf.a

#libftprintf.a

SRCS = ft_printf.c\
			 ft_identify_output.c\
			 ft_digit.c\
			 ft_print_csp.c\
			 ft_print_diu.c\
			 ft_print_xpercent.c\
			 ft_put_fd.c

OBJS=$(SRCS:.c=.o)

$(NAME):$(OBJS) $(LIBFT)
	@make -C $(LIBFT_PATH) all
	cp $(LIBFT_PATH)/$(LIBFT_NAME) $(NAME)
	ar rcs $(NAME) $(OBJS)
	#ar rcs libftprintf.a ft_identify_output.o ft_print_xpercent.o ft_puthex_fd.o ft_putunit_fd.o ft_putptr_fd.o
#LIBFTPATH ~/Dev/42cursus/ft_printf/Libft
LIBFT_PATH = ./libft
LIBFT_NAME = libft.a

all:$(NAME)

.c.o: $(OBJS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	@make -C $(LIBFT_PATH) clean

fclean: clean
	$(RM) $(NAME)
	$(RM) $(LIBFT_PATH)/$(LIBFT_NAME)
	
re: fclean all

.PHONY: all re clean fclean
