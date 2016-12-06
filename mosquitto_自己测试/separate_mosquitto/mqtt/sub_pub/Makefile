include ../config.mk

.PHONY: all clean

CC = gcc
CFLAGS = ../../../include/mqtt
OBJDIR = ../../../build/obj
OBJECTS_PRE = pub_client.o sub_client.o
OBJECTS_BAC = $(OBJDIR)/pub_client.o $(OBJDIR)/sub_client.o
LIBDIR = ../../../lib
MOQUITTO_LIB = moquitto.a


all : $(MOQUITTO_LIB)

$(MOQUITTO_LIB):$(OBJECTS_PRE)
	ar rcs -o $(LIBDIR)/$@ $(OBJECTS_BAC)


#all : mosquitto_sub mosquitto_pub\
mosquitto_pub : pub_client.o client_shared.o\
	${CROSS_COMPILE}${CC} $^ -o $@ ${CLIENT_LDFLAGS}\
\
mosquitto_sub : sub_client.o client_shared.o\
	${CROSS_COMPILE}${CC} $^ -o $@ ${CLIENT_LDFLAGS}\
\
pub_client.o : pub_client.c ../lib/libmosquitto.so.${SOVERSION}\
	${CROSS_COMPILE}${CC} -c $< -o $@ ${CLIENT_CFLAGS}\
\
sub_client.o : sub_client.c ../lib/libmosquitto.so.${SOVERSION}\
	${CROSS_COMPILE}${CC} -c $< -o $@ ${CLIENT_CFLAGS}\
\
client_shared.o : client_shared.c client_shared.h\
	${CROSS_COMPILE}${CC} -c $< -o $@ ${CLIENT_CFLAGS}\
\
../lib/libmosquitto.so.${SOVERSION} :\
	$(MAKE) -C ../lib\
\
install : all\
	$(INSTALL) -d ${DESTDIR}$(prefix)/bin\
	$(INSTALL) -s --strip-program=${CROSS_COMPILE}${STRIP} mosquitto_pub ${DESTDIR}${prefix}/bin/mosquitto_pub\
	$(INSTALL) -s --strip-program=${CROSS_COMPILE}${STRIP} mosquitto_sub ${DESTDIR}${prefix}/bin/mosquitto_sub\
\
uninstall :\
	-rm -f ${DESTDIR}${prefix}/bin/mosquitto_pub\
	-rm -f ${DESTDIR}${prefix}/bin/mosquitto_sub\
\
reallyclean : clean\
\
clean : \
	-rm -f *.o mosquitto_pub mosquitto_sub
