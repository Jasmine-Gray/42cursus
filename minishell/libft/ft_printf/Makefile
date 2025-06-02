CC = cc

CFLAGS = -Wall -Wextra -Werror

NAME = libftprintf.a

SRCS = ft_printf.c\
			 ft_identify_output.c\
			 ft_digit.c\
			 ft_print_csp.c\
			 ft_print_diu.c\
			 ft_print_xpercent.c\
			 ft_put_fd.c

OBJS=$(SRCS:.c=.o)

$(NAME):$(OBJS)
	ar rcs $(NAME) $(OBJS)
#$(NAME):$(OBJS) 
#	ar rcs $(NAME) $(OBJS) $(LIBFT)

LIBFT= ../libft.a

all:$(NAME)

.c.o: $(OBJS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)
	
re: fclean all

.PHONY: all re clean fclean
