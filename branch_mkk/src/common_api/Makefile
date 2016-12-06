SHELL = /bin/sh 

CC=gcc

EXEC = lwan
objects = ./cjson.o \
		  ./db_common.o \
		  ./debug_log.o \
		  ./json_common.o \
		  ./readconf.o \
		  ./web_common.o

SRCDIR = ./
OBJ = 	$(SRCDIR)cjson.c \
		$(SRCDIR)db_common.c \
		$(SRCDIR)debug_log.c \
		$(SRCDIR)json_common.c \
		$(SRCDIR)readconf.c \
		$(SRCDIR)web_common.c

#CFLAGS  := -Wall -Wextra -Wshadow -Wconversion -std=gnu99 -g \
		-I./include -I../../include/lwan -I../../include/lua
CFLAGS  := -w -Wshadow -Wconversion -std=gnu99 -g \
		-I./include -I../../include/lwan -I../../include/lua
LDFLAGS := -L../../lib/

LDLIBS  := -rdynamic -Wl,-whole-archive -llwan -llua \
		-Wl,-no-whole-archive -ldl -lm -lpthread -lz -lmysqlclient
C_DEFINES = -DHAS_CLOCK_GETTIME -DHAS_PTHREADBARRIER \
		-DHAVE_BUILTIN_CLZLL -DHAVE_BUILTIN_CPU_INIT \
		-DHAVE_STATIC_ASSERT -D_FILE_OFFSET_BITS=64
CFLAGS += $(C_DEFINES)

lwan : $(objects)
#	$(CC) $(LDFLAGS) $(CFLAGS) -o lwan $(OBJ) $(LDLIBS)
	$(CC) -c $(LDFLAGS) $(CFLAGS) $(OBJ) $(LDLIBS)

all:$(EXEC)

#main.o : ./main.c
#%.o : %.c  
#	$(CC) $(CCFLAGS) -c $< -o $@  

.PHONY : clean
clean :
	rm -rf lwan $(objects)
