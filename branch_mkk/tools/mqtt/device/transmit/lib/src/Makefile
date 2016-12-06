include ../config.mk


.PHONY: all clean

FLAGS				:= -I../lib -I../../include
LDFLAGS				:= -L../lib
LDLIBS				:= -lmosquitto -lcares -lpthread 


all : mosquitto_pub

mosquitto_pub : pub_client.o client_shared.o
	 ${CROSS_COMPILE}${CC} ${FLAGS} ${LDFLAGS} $^ -o $@ ${LDLIBS}
#	${CROSS_COMPILE}${CC} $^ -o $@ ${CLIENT_LDFLAGS}

mosquitto_sub : sub_client.o client_shared.o
	 ${CROSS_COMPILE}${CC} ${FLAGS} ${LDFLAGS} $^ -o $@ ${LDLIBS}
#${CROSS_COMPILE}${CC} $^ -o $@ ${CLIENT_LDFLAGS}

client_shared.o : client_shared.c client_shared.h
	${CROSS_COMPILE}${CC} -c $< -o $@ ${CLIENT_CFLAGS}

pub_client.o : pub_client.c
	${CROSS_COMPILE}${CC} ${FLAGS} -c $^ -o $@ ${LDFLAGS} ${CLIENT_CFLAGS} ${LDLIBS}
sub_client.o : sub_client.c
	${CROSS_COMPILE}${CC} ${FLAGS} -c $^ -o $@ ${LDFLAGS} ${CLIENT_CFLAGS}

#pub_client.o : pub_client.c ../lib/libmosquitto.so.${SOVERSION}
#	${CROSS_COMPILE}${CC} -c $< -o $@ ${CLIENT_CFLAGS}

#sub_client.o : sub_client.c ../lib/libmosquitto.so.${SOVERSION}
#	${CROSS_COMPILE}${CC} -c $< -o $@ ${CLIENT_CFLAGS}

#../lib/libmosquitto.so.${SOVERSION} :
#	$(MAKE) -C ../lib


clean : 
	-rm -f *.o mosquitto_pub mosquitto_sub
