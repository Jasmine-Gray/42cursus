NAME := inception
COMPOSE := sudo docker compose -p $(NAME) -f srcs/docker-compose.yml

.PHONY: all up down clean re

all: up

up:
	$(COMPOSE) up -d --build

down:
	$(COMPOSE) down

clean: down

re: down
	$(MAKE) up
