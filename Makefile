NAME64 = ./lib/x86_64/libft_malloc_
NAME32 = ./lib/x86/libft_malloc_

SRC    = $(wildcard src/*.c) $(wildcard src/*/*.c)
OBJ64  = $(addprefix build/x86_64/, $(SRC:.c=.o))
OBJ32  = $(addprefix build/x86/, $(SRC:.c=.o))

CFLAGS = -Wall -Wextra -Wno-format -g3
CC64   = gcc
CC32   = gcc -m32

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

all: $(NAME64)$(HOSTTYPE).so $(NAME32)$(HOSTTYPE).so

test: all
	mkdir -p ./bin/x86_64
	mkdir -p ./bin/x86
	$(CC64) -I./include/x86_64 ./test/main.c $(NAME64)$(HOSTTYPE).so -o ./bin/x86_64/out
	$(CC32) -I./include/x86    ./test/main.c $(NAME32)$(HOSTTYPE).so -o ./bin/x86/out

$(NAME64)$(HOSTTYPE).so: $(OBJ64)
		mkdir -p $(shell dirname $(NAME64))
		$(CC64) $(OBJ64) $(CFLAGS) -shared -o $(NAME64)$(HOSTTYPE).so
		 # TODO:
		 # ln -s $(NAME64)$(HOSTTYPE).so $(NAME64).so
		mkdir -p include/x86_64/malloc
		cp ./src/malloc.h ./include/x86_64/malloc

$(NAME32)$(HOSTTYPE).so: $(OBJ32)
		mkdir -p $(shell dirname $(NAME32))
		$(CC32) $(OBJ32) $(CFLAGS) -shared -o $(NAME32)$(HOSTTYPE).so
		 # ln -s $(NAME32)$(HOSTTYPE).so $(NAME32).so
		mkdir -p include/x86/malloc
		cp ./src/malloc.h ./include/x86/malloc

build/x86_64/%.o: %.c
		mkdir -p $(shell dirname $(@))
		$(CC64) -fPIC -c $(<) -o $(@) $(CFLAGS)
build/x86/%.o: %.c
		mkdir -p $(shell dirname $(@))
		$(CC32) -fPIC -c $(<) -o $(@) $(CFLAGS)

clean:
		rm -rf ./build/*
fclean: clean
		rm -rf ./lib/*
		rm -rf ./bin/*

re : fclean all

.phony: all clean fclean re
