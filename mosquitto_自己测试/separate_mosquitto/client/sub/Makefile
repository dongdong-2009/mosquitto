include config.mk

DIRS=lib sub

.PHONY : all mosquitto binary clean install uninstall

all : $(MAKE_ALL)

binary : mosquitto

mosquitto :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d} clean; done

install : mosquitto
	$(MAKE) -C sub/ install	
#set -e; for d in ${DIRS}; do $(MAKE) -C $${d} install; done


uninstall :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d} uninstall; done
