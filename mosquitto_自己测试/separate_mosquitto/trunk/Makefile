SHELL = /bin/sh 
EXEC = cloudStack
objects = ./src/main.o 

CFLAGS          := -I./include
LDFLAGS         := -L./lib
LDLIBS          := -ltopsdk -lcscommon -lcurl -lcslog -lmosquitto -lpthread -lm
CC=gcc

.PHONY : clean distclean

$(EXEC) : $(objects)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(objects) : ./src/main.c

clean :
	-rm -rf cloudStack $(objects)

distclean :
	-rm -rf cloudStack $(objects)
	-rm ./lib/*.a
	-rm ./build/obj/*
