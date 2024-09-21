NAME = push_swap

CC = cc
FLAGS = -Wall -Wextra -Werror

SRCS_DIR = srcs
INCLUDES_DIR = includes
LIBFT_DIR = libft

_SRCS = 
_SRCS = 
_SRCS = 
SRCS = $(_SRCS) $(_SRCS) $(_SRCS)

OBJS = $(SRCS:.c=.o)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $< -o $@ 
	
.c.o: $(OBJS)
	$(CC) $(FLAGS) -c $< -o $@

$(LIBFT): 
	@$(MAKE) -C (LIBFT)

all: $(LIBFT) $(NAME)

clean: 
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all 

.PHONY: all clean fclean re








CC = cc
CCFLAGS = -Wall -Wextra -Werror
DEBUG = -g -fsanitize=address
DEBUG2 = -g -fsanitize=thread

SRCDIR = srcs/
OBJDIR = objs

SRCS = $(SRCDIR)main.c $(SRCDIR)ft_atoi.c $(SRCDIR)ft_utils.c srcs/ft_time.c srcs/utils.c
OBJS = $(SRCS:$(SRCDIR)%.c=$(OBJDIR)/%.o)

TARGET = philo
TARGET1 = debug
TARGET2 = thread

all: $(TARGET)
    @echo $(BRIGHT_RED)$(BOLD)╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    @echo ╱╱╱╱╭━━━╮╱╭╮╱╱╱╭╮╱╭╮╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╭╮╱╱╱╱╱╱╱╱╱╱╱╱╭━━━╮╱╱╱╱╱
    @echo ╱╱╱╱┃╭━╮┃╱┃┃╱╱╱╰╯╱┃┃╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱┃┃╱╱╱╱╱╱╱╱╱╱╱╱┃╭━╮┃╱╱╱╱╱
    @echo ╱╱╱╱┃╰━╯┃╱┃╰━╮╱╭╮╱┃┃╱╱╭━━╮╱╭━━╮╱╭━━╮╱╭━━╮╱┃╰━╮╱╭━━╮╱╭━╮╱┃╰━━╮╱╱╱╱╱
    @echo ╱╱╱╱┃╭━━╯╱┃╭╮┃╱┃┃╱┃┃╱╱┃╭╮┃╱┃━━┫╱┃╭╮┃╱┃╭╮┃╱┃╭╮┃╱┃┃━┫╱┃╭╯╱╰━━╮┃╱╱╱╱╱
    @echo ╱╱╱╱┃┃╱╱╱╱┃┃┃┃╱┃┃╱┃╰╮╱┃╰╯┃╱┣━━┃╱┃╰╯┃╱┃╰╯┃╱┃┃┃┃╱┃┃━┫╱┃┃╱╱┃╰━╯┃╱╱╱╱╱
    @echo ╱╱╱╱╰╯╱╱╱╱╰╯╰╯╱╰╯╱╰━╯╱╰━━╯╱╰━━╯╱╰━━╯╱┃╭━╯╱╰╯╰╯╱╰━━╯╱╰╯╱╱╰━━━╯╱╱╱╱╱
    @echo ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╰╯╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    @echo ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱

debug: $(TARGET1)

thread: $(TARGET2)

$(TARGET): $(OBJS)
    $(CC) $(CCFLAGS) -o $@ $^
    @echo

$(TARGET1): $(OBJS)
    @echo $(BRIGHT_YELLOW)
    $(CC) $(CCFLAGS) $(DEBUG) -o $@ $^
    @echo
    @echo FSANITIZE=ADDRESS DONE

$(TARGET2): $(OBJS)
    @echo $(BRIGHT_YELLOW)
    $(CC) $(CCFLAGS) $(DEBUG2) -o $@ $^
    @echo
    @echo FSANITIZE=THREAD DONE

$(OBJDIR):
    @echo $(GREEN)
    @mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)%.c | $(OBJDIR)
    $(CC) $(CCFLAGS) -o $@ -c $<

clean:
    @rm -rf $(OBJDIR)
    @echo $(BRIGHT_MAGENTA)
    @echo ╭━╮╭╮╱╭━╮╭━━╮╭━┳╮╭━┳╮╭━━╮╭━┳╮╭━━╮
    @echo ┃╭╯┃┃╱┃┳╯┃╭╮┃┃┃┃┃┃┃┃┃╰┃┃╯┃┃┃┃┃╭━┫
    @echo ┃╰╮┃╰╮┃┻╮┃┣┫┃┃┃┃┃┃┃┃┃╭┃┃╮┃┃┃┃┃╰╮┃╭╮╭╮╭╮
    @echo ╰━╯╰━╯╰━╯╰╯╰╯╰┻━╯╰┻━╯╰━━╯╰┻━╯╰━━╯╰╯╰╯╰╯
