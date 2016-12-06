SHELL = /bin/sh 
EXEC = cloudStack
objects = ./src/main.o \
		  ./src/ideahos_xgo_server.o\
		  ./src/common_api/cjson.o \
		  ./src/common_api/db_common.o \
		  ./src/common_api/debug_log.o \
		  ./src/common_api/json_common.o \
		  ./src/common_api/readconf.o \
		  ./src/common_api/web_common.o

CFLAGS          := -I./include -I./include/openssl -I./src/common_api -I./include/mqtt/
LDFLAGS         := -L./lib 
LDLIBS          := -lmosquitto -ltopsdk -lcscommon -lcurl -lcslog -lpthread -lm -lssl -lcrypto  -llwan -llua -lmysqlclient -lz -ldl
CC=gcc

.PHONY : clean distclean

$(EXEC) : $(objects)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LDLIBS)


#$(objects) : ./src/main.c

clean :
	-rm -rf cloudStack ./src/*.o

distclean :
	-rm -rf cloudStack $(objects)
	-rm ./lib/*.a
	-rm ./build/obj/*
