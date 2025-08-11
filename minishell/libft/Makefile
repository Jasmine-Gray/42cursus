# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/11 05:40:45 by tkusama           #+#    #+#              #
#    Updated: 2025/08/11 05:40:48 by tkusama          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc

CFLAGS = -Wall -Wextra -Werror

NAME = libft.a

SRCS = ft_isalpha.c \
	ft_isdigit.c \
	ft_isalnum.c \
	ft_isascii.c \
	ft_isprint.c \
	ft_strlen.c \
	ft_memset.c \
	ft_bzero.c \
	ft_memcpy.c \
	ft_memmove.c \
	ft_strlcpy.c \
	ft_strlcat.c \
	ft_toupper.c \
	ft_tolower.c \
	ft_strchr.c \
	ft_strrchr.c \
	ft_strncmp.c \
	ft_memchr.c \
	ft_memcmp.c \
	ft_strnstr.c \
	ft_atoi.c \
	ft_calloc.c \
	ft_strdup.c \
	ft_substr.c \
	ft_strjoin.c \
	ft_split.c \
	ft_strtrim.c \
	ft_itoa.c \
	ft_strmapi.c \
	ft_striteri.c \
	ft_putchar_fd.c \
	ft_putstr_fd.c \
	ft_putendl_fd.c \
	ft_putnbr_fd.c \

# SRCSB = ft_lstnew.c \
# 				ft_lstadd_front.c \
# 				ft_lstlast.c \
# 				ft_lstsize.c \
# 				ft_lstadd_back.c \
# 				ft_lstdelone.c \
# 				ft_lstclear.c \
# 				ft_lstiter.c \
# 				ft_lstmap.c

OBJS = $(SRCS:.c=.o)
# OBJSB = $(SRCSB:.c=.o)

# Create obj directory
OBJDIR = obj
OBJ_FILES = $(addprefix $(OBJDIR)/, $(OBJS))
# OBJ_FILESB = $(addprefix $(OBJDIR)/, $(OBJSB))

$(NAME): $(OBJ_FILES)
	@echo "Building libft..."
	ar rcs $(NAME) $(OBJ_FILES)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

clean:
	@echo "Cleaning libft object files..."
	$(RM) -r $(OBJDIR)
#	@echo "Cleaning bonus files..."
#	$(RM) $(OBJSB) bonus

fclean: clean
	@echo "Cleaning libft.a..."
	$(RM) $(NAME)

re: fclean all

#bonus: $(OBJ_FILESB)
#	@echo "Building libft bonus..."
#	ar rcs $(NAME) $(OBJ_FILESB)
#	@touch bonus

.PHONY: all re clean fclean bonus
