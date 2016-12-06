include ../../lib/config.mk


.PHONY: all clean

FLAGS				:= -I../../lib/lib -I../../include
LDFLAGS				:= -L../../lib/lib
LDLIBS				:= -lmosquitto -lcares -lpthread 


all : mqtt_client

mqtt_client : mqtt_client.o  sub_client.o pub_client.o client_shared.o
	${CROSS_COMPILE}${CC} ${FLAGS} ${LDFLAGS} $^ -o $@ ${LDLIBS}

#mosquitto_pub : pub_client.o client_shared.o
#	 ${CROSS_COMPILE}${CC} ${FLAGS} ${LDFLAGS} $^ -o $@ ${LDLIBS}
#	${CROSS_COMPILE}${CC} $^ -o $@ ${CLIENT_LDFLAGS}

#mosquitto_sub : sub_client.o client_shared.o
#	 ${CROSS_COMPILE}${CC} ${FLAGS} ${LDFLAGS} $^ -o $@ ${LDLIBS}
#${CROSS_COMPILE}${CC} $^ -o $@ ${CLIENT_LDFLAGS}


mqtt_client.o : mqtt_client.c pub_client.o sub_client.o
	${CROSS_COMPILE}${CC} ${FLAGS} -c $< -o $@ ${LDLIBS}

pub_client.o : ../../lib/src/pub_client.c
	${CROSS_COMPILE}${CC} ${FLAGS} -c $^ -o $@ ${LDFLAGS} ${CLIENT_CFLAGS}

sub_client.o : ../../lib/src/sub_client.c
	${CROSS_COMPILE}${CC} ${FLAGS} -c $^ -o $@ ${LDFLAGS} ${CLIENT_CFLAGS}

client_shared.o : ../../lib/src/client_shared.c ../../lib/src/client_shared.h
	${CROSS_COMPILE}${CC} ${FLAGS} -c $< -o $@ ${CLIENT_CFLAGS}


clean : 
	-rm -f *.o mqtt_client
