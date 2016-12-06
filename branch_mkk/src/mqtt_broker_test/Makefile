EXEC 		= moquitto_borker
CC			= gcc

CFLAGS 		:= -I../../include
LDFLAGS		:= -L../../lib
LDLIBS		:= -lmosquitto -lm -lpthread


.PHONY : clean


$(EXEC) : main.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LDLIBS)

main.o : main.c

clean :
	-rm -rf $(EXEC) *.o
