##
## ------------------------------------------------------------- variables --
##
CC=gcc
#CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3
CFLAGS=-DDEBUG -Wall -pedantic -Wextra -Wstrict-prototypes -fno-common -g -O3
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

INCLUDES=-I/usr/local/nxprdlib/types -I/usr/local/nxprdlib/intfs
LINKPATHS=-L/usr/local/lib -L/usr/local/nxprdlib
LINKLIBS=-lwiringPi -lnxprdlib

##
## ----------------------------------------------------------------- rules --
##
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
##
## --------------------------------------------------------------- targets --
##
all:	test

test:	test.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LINKPATHS) $(LINKLIBS)


clean:
	rm *.o test
