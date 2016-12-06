include config.mk

DIRS=lib pub

.PHONY : all mosquitto docs binary clean reallyclean test install uninstall dist sign copy

all : $(MAKE_ALL)

binary : mosquitto

mosquitto :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d} clean; done

install : mosquitto
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d} install; done


uninstall :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d} uninstall; done

