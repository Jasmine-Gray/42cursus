NAME = minitalk
NAME_SERVER = server
NAME_CLIENT = client
NAME_BONUS_SERVER = server_bonus
NAME_BONUS_CLIENT = client_bonus

cc = CC
FLAGS = -Wall -Wextra -Werror

INCLUDE_DIR = includes
PRINTF_DIR = printf
PRINTF = printf/libftprintf.a

SRCS_DIR = srcs
SRC_SERVER = 	$(SRCS_DIR)/server.c
SRC_CLIENT = 	$(SRCS_DIR)/client.c
SRC_BONUS_SERVER = 	$(SRCS_DIR)/server_bonus.c
SRC_BONUS_CLIENT = 	$(SRCS_DIR)/client_bonus.c

INCLUDES = -I$(INCLUDE_DIR)

OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
OBJ_BONUS_SERVER = $(SRC_BONUS_SERVER:.c=.o)
OBJ_BONUS_CLIENT = $(SRC_BONUS_CLIENT:.c=.o)

all: $(PRINTF) $(NAME)

bonus: $(PRINTF) $(NAME_BONUS_SERVER) $(NAME_BONUS_CLIENT)

$(PRINTF):
	@$(MAKE) -C $(PRINTF_DIR)

$(NAME): $(NAME_SERVER) $(NAME_CLIENT)

$(NAME_SERVER): $(OBJ_SERVER)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJ_SERVER) -o $@ $(PRINTF)
	
$(NAME_CLIENT): $(OBJ_CLIENT)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJ_CLIENT) -o $@ $(PRINTF)

$(NAME_BONUS_SERVER): $(OBJ_BONUS_SERVER)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJ_BONUS_SERVER) -o $@ $(PRINTF)

$(NAME_BONUS_CLIENT): $(OBJ_BONUS_CLIENT)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJ_BONUS_CLIENT) -o $@ $(PRINTF)

$(SRCS_DIR)/%.o:$(SRCS_DIR)/%.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJ_SERVER) $(OBJ_CLIENT) $(OBJ_BONUS_SERVER) $(OBJ_BONUS_CLIENT)
	$(RM) $(PRINTF)
	@$(MAKE) -C $(PRINTF_DIR) clean

fclean: clean
	$(RM) $(NAME_SERVER) $(NAME_CLIENT) $(NAME_BONUS_SERVER) $(NAME_BONUS_CLIENT)
	@$(MAKE) -C $(PRINTF_DIR) clean


re: fclean all

.PHONY: all re clean fclean 

