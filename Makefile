# Variables to be used for implicit rules
CFLAGS= -Wall -Wextra
CC=gcc

# Phony rules
.PHONY: clean
.PHONY: all

# Default rule to build executable program for question2
all: shell

# Rules to build object files
shell: shell.c stack.c

# Rule to clean up all object and exec files
clean:
	rm -f *.o shell