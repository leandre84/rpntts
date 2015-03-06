##
## ------------------------------------------------------------- variables --
##
CC=gcc
#CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3
CFLAGS=-DDEBUG -Wall -pedantic -Wextra -fno-common -g -O3 -std=gnu99
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

INCLUDES=-I/usr/local/nxprdlib/types -I/usr/local/nxprdlib/intfs -I/usr/include/mysql
LINKPATHS=-L/usr/local/lib -L/usr/local/nxprdlib
LINKLIBS=-lwiringPi -lnxprdlib -lmysqlclient -lz

##
## ----------------------------------------------------------------- rules --
##
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
##
## --------------------------------------------------------------- targets --
##
all:	rpntts	

rpntts:	rpntts-nfc.o rpntts-mysql.o rpntts.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LINKPATHS) $(LINKLIBS)


clean:
	rm -f *.o rpntts
