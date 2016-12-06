CC=gcc
OBJDIR = ../../build/obj/lwan/
LIBDIR = ../../lib/
CFLAGS = -I ../../include/lwan/ -I ../../include/lua/
LDLIBS = -lpthread -L ../../lib/ -llua

C_DEFINES = -DHAS_ACCEPT4 -DHAS_CLOCK_GETTIME -DHAS_MEMPCPY -DHAS_MEMRCHR -DHAS_PIPE2 -DHAS_PTHREADBARRIER -DHAS_RAWMEMCHR \
	-DHAS_TIMEDJOIN -DHAVE_BUILTIN_CLZLL -DHAVE_BUILTIN_CPU_INIT -DHAVE_LUA -DHAVE_STATIC_ASSERT -D_FILE_OFFSET_BITS=64 \
	-Dlwan_shared_EXPORTS

CFLAGS += $(C_DEFINES)
OBJECTS_PRE = base64.o hash.o int-to-str.o \
			list.o lwan-cache.o lwan-config.o \
			lwan-coro.o lwan-http-authorize.o \
			lwan-io-wrappers.o lwan-job.o \
			lwan-lua.o lwan-redirect.o lwan-request.o \
			lwan-response.o lwan-rewrite.o lwan-serve-files.o \
			lwan-socket.o lwan-status.o lwan-straitjacket.o \
			lwan-tables.o lwan-template.o lwan-thread.o \
			lwan-trie.o lwan.o missing.o \
			murmur3.o patterns.o reallocarray.o realpathat.o sd-daemon.o strbuf.o 
OBJECTS_BAC = $(OBJDIR)base64.o $(OBJDIR)hash.o $(OBJDIR)int-to-str.o $(OBJDIR)list.o $(OBJDIR)lwan-cache.o \
	$(OBJDIR)lwan-config.o $(OBJDIR)lwan-coro.o $(OBJDIR)lwan-http-authorize.o \
	$(OBJDIR)lwan-io-wrappers.o $(OBJDIR)lwan-job.o $(OBJDIR)lwan-lua.o $(OBJDIR)lwan-redirect.o $(OBJDIR)lwan-request.o \
	$(OBJDIR)lwan-response.o $(OBJDIR)lwan-rewrite.o $(OBJDIR)lwan-serve-files.o \
	$(OBJDIR)lwan-socket.o $(OBJDIR)lwan-status.o $(OBJDIR)lwan-straitjacket.o $(OBJDIR)lwan-tables.o $(OBJDIR)lwan-template.o \
	$(OBJDIR)lwan-thread.o $(OBJDIR)lwan-trie.o $(OBJDIR)lwan.o $(OBJDIR)missing.o \
	$(OBJDIR)murmur3.o $(OBJDIR)patterns.o $(OBJDIR)reallocarray.o $(OBJDIR)realpathat.o $(OBJDIR)sd-daemon.o $(OBJDIR)strbuf.o 

all: liblwan.a

liblwan.a: $(OBJECTS_PRE)
	ar rcs -o $(LIBDIR)$@ $(OBJECTS_BAC)

.c.o:
	$(CC) -std=gnu99 $(CFLAGS) $(LDLIBS) $< -c -o $(OBJDIR)$@

clean:
	rm -rf $(LIBDIR)liblwan.a $(OBJECTS_BAC)

.PHONY: all
.SUFFIXES: .c .o .h

