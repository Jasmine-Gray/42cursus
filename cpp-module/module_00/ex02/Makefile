NAME        = account
CC          = c++
CFLAGS      = -Wall -Wextra -Werror -Wpedantic -std=c++98

SRCS        = Account.cpp tests.cpp
OBJS        = $(SRCS:.cpp=.o)


all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
