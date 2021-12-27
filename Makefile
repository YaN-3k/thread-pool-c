build: main.c
	$(CC) -D_DEFAULT_SOURCE -ansi -pedantic -Wextra -Wall -g -pthread main.c

.PHONY: build
