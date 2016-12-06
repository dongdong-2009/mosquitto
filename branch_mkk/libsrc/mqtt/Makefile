include config.mk

CC=gcc
FLAGS = -I../../include/mqtt/src -I../../include/mqtt/lib -I ../../include/mqtt -I../../include/openssl -L../../lib/
LIBFLAGS = -lssl -lcrypto -ldl
OBJDIR = ../../build/obj/
LIBDIR = ../../lib/
OBJECTS_PRE = mosquitto.o bridge.o conf.o context.o database.o logging.o loop.o memory_mosq.o persist.o net.o net_mosq.o read_handle.o read_handle_client.o read_handle_server.o read_handle_shared.o security.o security_default.o send_client_mosq.o send_mosq.o send_server.o service.o subs.o sys_tree.o time_mosq.o tls_mosq.o util_mosq.o websockets.o will_mosq.o
OBJECTS_BAC =  $(OBJDIR)mosquitto.o $(OBJDIR)bridge.o $(OBJDIR)conf.o $(OBJDIR)context.o $(OBJDIR)database.o $(OBJDIR)logging.o $(OBJDIR)loop.o $(OBJDIR)memory_mosq.o $(OBJDIR)persist.o $(OBJDIR)net.o $(OBJDIR)net_mosq.o $(OBJDIR)read_handle.o $(OBJDIR)read_handle_client.o $(OBJDIR)read_handle_server.o $(OBJDIR)read_handle_shared.o $(OBJDIR)security.o $(OBJDIR)security_default.o $(OBJDIR)send_client_mosq.o $(OBJDIR)send_mosq.o $(OBJDIR)send_server.o $(OBJDIR)service.o $(OBJDIR)subs.o $(OBJDIR)sys_tree.o $(OBJDIR)time_mosq.o $(OBJDIR)tls_mosq.o $(OBJDIR)util_mosq.o $(OBJDIR)websockets.o $(OBJDIR)will_mosq.o
SRC = src/
LIB = lib/

.PHONY: all clean

all : libmosquitto.a


libmosquitto.a:$(OBJECTS_PRE)
	ar rcs -o $(LIBDIR)$@  $(OBJECTS_BAC)

mosquitto.o : $(SRC)mosquitto.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

bridge.o : $(SRC)bridge.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)
	
conf.o : $(SRC)conf.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)
	
context.o : $(SRC)context.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

database.o : $(SRC)database.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

logging.o : $(SRC)logging.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

loop.o : $(SRC)loop.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

memory_mosq.o : $(LIB)memory_mosq.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

net.o : $(SRC)net.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

net_mosq.o : $(LIB)net_mosq.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)
	
persist.o : $(SRC)persist.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)
	
read_handle.o : $(SRC)read_handle.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

read_handle_client.o : $(SRC)read_handle_client.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

read_handle_server.o : $(SRC)read_handle_server.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

read_handle_shared.o : $(LIB)read_handle_shared.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

security.o : $(SRC)security.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

security_default.o : $(SRC)security_default.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

send_client_mosq.o : $(LIB)send_client_mosq.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

send_mosq.o : $(LIB)send_mosq.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

send_server.o : $(SRC)send_server.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

service.o : $(SRC)service.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

subs.o : $(SRC)subs.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

sys_tree.o : $(SRC)sys_tree.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

time_mosq.o : $(LIB)time_mosq.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

tls_mosq.o : $(LIB)tls_mosq.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

util_mosq.o : $(LIB)util_mosq.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

websockets.o : $(SRC)websockets.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

will_mosq.o : $(LIB)will_mosq.c
	${CROSS_COMPILE}${CC} $(FLAGS) $(BROKER_CFLAGS) -c $< -o $(OBJDIR)$@ $(LIBFLAGS)

#mosquitto_passwd : mosquitto_passwd.o
#	${CROSS_COMPILE}${CC} $^ -o $@ ${LDFLAGS} $(PASSWD_LIBS)

#mosquitto_passwd.o : mosquitto_passwd.c
#	${CROSS_COMPILE}${CC} $(CFLAGS) ${CPPFLAGS} -c $< -o $@

clean : 
	-rm -rf  $(OBJECTS_BAC) $(LIBDIR)libmosquitto.a
