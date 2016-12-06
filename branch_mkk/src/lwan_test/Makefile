SHELL = /bin/sh 

CC=gcc

EXEC = lwan
objects = ./main.o 

SRCDIR = ./
OBJ = $(SRCDIR)main.c

CFLAGS  := -Wall -Wextra -Wshadow -Wconversion -std=gnu99 -g \
		-I./include -I../../include/lwan -I../../include/lua
LDFLAGS := -L../../lib/

LDLIBS  := -rdynamic -Wl,-whole-archive -llwan -llua \
		-Wl,-no-whole-archive -ldl -lm -lpthread -lz
C_DEFINES = -DHAS_CLOCK_GETTIME -DHAS_PTHREADBARRIER \
		-DHAVE_BUILTIN_CLZLL -DHAVE_BUILTIN_CPU_INIT \
		-DHAVE_STATIC_ASSERT -D_FILE_OFFSET_BITS=64
CFLAGS += $(C_DEFINES)

lwan : $(objects)
	$(CC) $(LDFLAGS) $(CFLAGS) -o lwan $(OBJ) $(LDLIBS)

all:$(EXEC)

main.o : ./main.c

.PHONY : clean
clean :
	rm -rf lwan $(objects)
