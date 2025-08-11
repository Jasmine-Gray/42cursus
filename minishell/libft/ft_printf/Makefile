CC = cc

CFLAGS = -Wall -Wextra -Werror

NAME = libftprintf.a

SRCS = ft_printf.c\
			ft_dprintf.c\
			ft_vdprintf.c\
			ft_itoa_unsigned.c\
			print_num.c\
			print_str.c\
			print_utils.c\
			print_utils_fd.c\
			print_str_fd.c\
			print_num_fd.c\
			ctx_flush.c\
			ctx_append_mem.c\
			ctx_append_char.c\
			process_format.c\

# Create obj directory
OBJDIR = obj
OBJS = $(SRCS:.c=.o)
OBJ_FILES = $(addprefix $(OBJDIR)/, $(OBJS))

all: $(NAME)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(NAME): $(OBJ_FILES)
	@echo "Building ft_printf..."
	ar rcs $(NAME) $(OBJ_FILES)

clean:
	@echo "Cleaning ft_printf object files..."
	$(RM) -r $(OBJDIR)

fclean: clean
	@echo "Cleaning ft_printf library..."
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
