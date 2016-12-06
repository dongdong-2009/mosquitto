CC=gcc
OBJDIR = ../../build/obj/
LIBDIR = ../../lib/
CFLAGS = -I../../include/log
OBJECTS_PRE = buf.o category.o category_table.o conf.o event.o format.o level.o level_list.o \
		mdc.o record.o record_table.o rotater.o rule.o spec.o thread.o zc_arraylist.o \
		zc_hashtable.o zc_profile.o zc_util.o zlog.o
OBJECTS_BAC = $(OBJDIR)buf.o $(OBJDIR)category.o $(OBJDIR)category_table.o $(OBJDIR)conf.o $(OBJDIR)event.o \
	$(OBJDIR)format.o $(OBJDIR)level.o $(OBJDIR)level_list.o $(OBJDIR)mdc.o $(OBJDIR)record.o \
	$(OBJDIR)record_table.o $(OBJDIR)rotater.o $(OBJDIR)rule.o $(OBJDIR)spec.o $(OBJDIR)thread.o \
	$(OBJDIR)zc_arraylist.o $(OBJDIR)zc_hashtable.o $(OBJDIR)zc_profile.o $(OBJDIR)zc_util.o $(OBJDIR)zlog.o 

all: libcslog.a

libcslog.a: $(OBJECTS_PRE)
	ar rcs -o $(LIBDIR)$@ $(OBJECTS_BAC)

.c.o:
	$(CC) $(CFLAGS) $< -c -o $(OBJDIR)$@

clean:
	rm -rf $(LIBDIR)libcslog.a $(OBJECTS_BAC)

.PHONY: all
.SUFFIXES: .c .o .h
