NAME		= bureaucrat

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -pedantic -std=c++98

SRCS		= main.cpp \
			  Bureaucrat.cpp \
			  AForm.cpp \
			  ShrubberyCreationForm.cpp \
			  RobotomyRequestForm.cpp \
			  PresidentialPardonForm.cpp \
			  Intern.cpp

OBJS		= $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)
	rm -f *_shrubbery 

re: fclean all

.PHONY: all clean fclean re
