CC=gcc
OBJDIR = ../../build/obj/
LIBDIR = ../../lib/
CFLAGS = -I../../include/common \
	-I../../include/curl \
	-I../../include/tbtools
OBJECTS_PRE = curlResponse.o SingleMap.o globalConfig.o \
	TaobaoClient.o TopRequest.o TopResponse.o WebUtils.o randomGeneration.o
OBJECTS_BAC = $(OBJDIR)curlResponse.o $(OBJDIR)SingleMap.o $(OBJDIR)globalConfig.o \
	$(OBJDIR)TaobaoClient.o $(OBJDIR)TopRequest.o $(OBJDIR)TopResponse.o $(OBJDIR)WebUtils.o $(OBJDIR)randomGeneration.o

all: libtopsdk.a

libtopsdk.a: $(OBJECTS_PRE)
	ar rcs -o $(LIBDIR)$@ $(OBJECTS_BAC)

.c.o:
	$(CC) $(CFLAGS) $< -c -o $(OBJDIR)$@

clean:
	rm -rf $(LIBDIR)libtopsdk.a $(OBJECTS_BAC)

.PHONY: all
.SUFFIXES: .c .o .h
