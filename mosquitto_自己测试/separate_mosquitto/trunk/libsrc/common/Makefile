CC=gcc
OBJDIR = ../../build/obj/
LIBDIR = ../../lib/
CFLAGS = -I../../include/common
OBJECTS_PRE = json.o Md5Util.o 
OBJECTS_BAC = $(OBJDIR)json.o $(OBJDIR)Md5Util.o 

all: libcscommon.a

libcscommon.a: $(OBJECTS_PRE)
	ar rcs -o $(LIBDIR)$@ $(OBJECTS_BAC)

.c.o:
	$(CC) $(CFLAGS) $< -c -o $(OBJDIR)$@

clean:
	rm -rf $(LIBDIR)libcscommon.a $(OBJECTS_BAC)

.PHONY: all
.SUFFIXES: .c .o .h
