include ../config.mk

.PHONY: all install uninstall clean

all : mosquitto_pub

mosquitto_pub : pub_client.o client_shared.o
	${CROSS_COMPILE}${CC} $^ -o $@ ${CLIENT_LDFLAGS}
	$(MAKE) -C ../lib install

pub_client.o : pub_client.c ../lib/libmosquitto.so.${SOVERSION}
	${CROSS_COMPILE}${CC} -c $< -o $@ ${CLIENT_CFLAGS}

client_shared.o : client_shared.c client_shared.h
	${CROSS_COMPILE}${CC} -c $< -o $@ ${CLIENT_CFLAGS}

../lib/libmosquitto.so.${SOVERSION} :
	$(MAKE) -C ../lib

install : all
	$(INSTALL) -d ${DESTDIR}$(prefix)/sbin
	$(INSTALL) -s --strip-program=${CROSS_COMPILE}${STRIP} mosquitto_pub ${DESTDIR}${prefix}/sbin/mosquitto_pub

uninstall :
	-rm -f ${DESTDIR}${prefix}/sbin/mosquitto_pub

clean : 
	-rm -f *.o mosquitto_pub
	$(MAKE) -C ../lib  clean
	$(MAKE) -C ../lib  uninstall
	$(MAKE) -C ./ uninstall
	-rm -f /usr/local/lib/libmosquitto*
