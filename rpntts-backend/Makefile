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

INCLUDES=-I/usr/local/nxprdlib/types -I/usr/local/nxprdlib/intfs -I /usr/local/nxprdlib/comps/phacDiscLoop/src/Sw -I/usr/include/mysql -I/usr/include/espeak -I/usr/include/glib-2.0 -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include 
LINKPATHS=-L/usr/local/lib -L/usr/local/nxprdlib 
LINKLIBS=-lwiringPi -lwiringPiDev -lnxprdlib -lmysqlclient -lz -lespeak -lglib-2.0

##
## ----------------------------------------------------------------- rules --
##
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
##
## --------------------------------------------------------------- targets --
##
all:	rpntts	

rpntts:	rpntts-common.o rpntts-nfc.o rpntts-mysql.o rpntts-lcd.o rpntts.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LINKPATHS) $(LINKLIBS)

#rpntts-test: rpntts-common.o rpntts-nfc.o rpntts-test.o
#	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LINKPATHS) $(LINKLIBS)

clean:
	rm -f *.o rpntts rpntts-test
