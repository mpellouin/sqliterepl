SRC = src/main.c \
	  src/execute.c \
	  src/ibuffer.c \
	  src/table.c

OBJ = $(SRC:.c=.o)

INCLUDE = ./inc

CC = gcc

NAME = sqliterepl

CFLAGS = -Wall -Wextra -Werror -I ./inc

TEST_CMD = rspec

SHELL = /bin/bash

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(SRC) $(CFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

test: re
	$(SHELL) -c "$(TEST_CMD)"

