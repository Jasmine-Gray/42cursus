NAME = inception
DATA_DIR = /home/mishimod/data

all: up

init_dir:
	@mkdir -p $(DATA_DIR)/mariadb
	@mkdir -p $(DATA_DIR)/wordpress

up: init_dir
	docker compose -f srcs/docker-compose.yml up -d --build

down:
	docker compose -f srcs/docker-compose.yml down

clean: down
	docker system prune -a --volumes -f

fclean: clean
	@sudo rm -rf $(DATA_DIR)

re: fclean all

.PHONY: all up down clean fclean re init_dir
