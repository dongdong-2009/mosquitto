include config.mk

DIRS=lib src

.PHONY : all clean

all:
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d} clean; done
	set -e; for d in ${DOCDIRS}; do $(MAKE) -C $${d} clean; done
