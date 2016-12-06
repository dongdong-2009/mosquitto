SHELL = /bin/sh

CC = gcc -std=gnu99                                      
OBJDIR := ../../build/obj/lua/
LIBDIR := ../../lib/
CFLAGS := ../../include/lua/
CCFLAGS = -O2 -Wall -DLUA_USE_LINUX -Wl,-E -ldl -lreadline -lhistory -lncurses

LIB := -lm -ldl

OBJS =  lapi.o \
		lauxlib.o \
		lbaselib.o \
		lcode.o \
		ldblib.o \
		ldebug.o \
		ldo.o \
		ldump.o \
		lfunc.o \
		lgc.o \
		linit.o \
		liolib.o \
		llex.o \
		lmathlib.o \
		lmem.o \
		loadlib.o \
		lobject.o \
		lopcodes.o \
		loslib.o \
		lparser.o \
		lstate.o \
		lstring.o \
		lstrlib.o \
		ltable.o \
		ltablib.o \
		ltm.o \
		lundump.o \
		lvm.o \
		lzio.o \
		print.o

		
OBJS_BAC = $(OBJDIR)/lapi.o \
		$(OBJDIR)/lauxlib.o \
		$(OBJDIR)/lbaselib.o \
		$(OBJDIR)/lcode.o \
		$(OBJDIR)/ldblib.o \
		$(OBJDIR)/ldebug.o \
		$(OBJDIR)/ldo.o \
		$(OBJDIR)/ldump.o \
		$(OBJDIR)/lfunc.o \
		$(OBJDIR)/lgc.o \
		$(OBJDIR)/linit.o \
		$(OBJDIR)/liolib.o \
		$(OBJDIR)/llex.o \
		$(OBJDIR)/lmathlib.o \
		$(OBJDIR)/lmem.o \
		$(OBJDIR)/loadlib.o \
		$(OBJDIR)/lobject.o \
		$(OBJDIR)/lopcodes.o \
		$(OBJDIR)/loslib.o \
		$(OBJDIR)/lparser.o \
		$(OBJDIR)/lstate.o \
		$(OBJDIR)/lstring.o \
		$(OBJDIR)/lstrlib.o \
		$(OBJDIR)/ltable.o \
		$(OBJDIR)/ltablib.o \
		$(OBJDIR)/ltm.o \
		$(OBJDIR)/lundump.o \
		$(OBJDIR)/lvm.o \
		$(OBJDIR)/lzio.o \
		$(OBJDIR)/print.o

all: liblua.a

liblua.a: $(OBJS)
	ar rcs -o $(LIBDIR)$@ $(OBJS_BAC)

.c.o:
	$(CC) -std=gnu99 -I $(CFLAGS) $(LIB) $< -c -o $(OBJDIR)$@

clean:
	rm -rf $(LIBDIR)liblua.a $(OBJS_BAC)

.PHONY: all
.SUFFIXES: .c .o .h 


