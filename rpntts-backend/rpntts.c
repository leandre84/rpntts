#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include <my_global.h>
#include <mysql.h>

#include "rpntts-nfc.h"
#include "rpntts-mysql.h"

#define SLEEPSECONDS 3

#define RPNTTSDEBUG

void usage(char *progname);

int main(int argc, char **argv) {

    int vflag = 0;
    char *dbhost = NULL;
    char dbhostdefault[] = "localhost";
    char *dbname = NULL;
    char dbnamedefault[] = "rpntts";
    char *dbuser = NULL;
    char dbuserdefault[] = "rpntts";
    char *dbpassword = NULL;
    char dbpassworddefault[] = "rpntts";
    unsigned int dbtcpsocket = 3306;
    char *strtolep = NULL;
    int option = 0;

    nxprdlibparams nxpparams;
    uint8_t bHalBufferTx[HALBUFSIZE];
    uint8_t bHalBufferRx[HALBUFSIZE];
    uint8_t bcardUID[MAXUIDLEN];
    uint8_t cardUIDlen = 0;
    char cardUID[(MAXUIDLEN*2)+1];
    char *ppos = NULL;
    MYSQL mysql;
    rpntts_user user;
    int status = 0;
    unsigned int i = 0;

    while ((option = getopt(argc, argv, "vh:d:u:p:s:")) != -1) {
       switch (option) {
           case 'v':
            vflag = 1;
            break;
           case 'h':
            dbhost = optarg;
            break;
           case 'd':
            dbname = optarg;
            break;
           case 'u':
            dbuser = optarg;
            break;
           case 'p':
            dbpassword = optarg;
            break;
           case 's':
            dbtcpsocket = strtoul(optarg, &strtolep, 10);
            if (strlen(strtolep) > 0) {
                fprintf(stderr, "Invalid port argument given: %s\n", optarg);
                return -2;
            }
            break;
           default:
            usage(argv[0]);
            return -1;
       }
    }

    if (dbhost == NULL) {
        dbhost = dbhostdefault;
    }
    if (dbname == NULL) {
        dbname = dbnamedefault;
    }
    if (dbuser == NULL) {
        dbuser = dbuserdefault;
    }
    if (dbpassword == NULL) {
        dbpassword = dbpassworddefault;
    }

    /* Initialize NXP Reader Library params */
    status = initNxprdlib(&nxpparams, bHalBufferTx, bHalBufferRx);
    if (status != 0) {
        fprintf(stderr, "Error initializing nxp reader library structures: %d\n", status);
        return 1;
    }

    /* Init mysql structure */
    if (mysql_init(&mysql) == NULL) {
        fprintf(stderr, "Error initializing mysql structure%d\n", status);
        return 2;
    }

    /* Connect to mysql DB */
    if (! mysql_real_connect(&mysql, dbhost, dbuser, dbpassword, dbname, dbtcpsocket, NULL, 0)) {
        fprintf(stderr, "Error connecting to mysql: %s\n", mysql_error(&mysql));
        mysql_close(&mysql);
        return 3;
    }

    while (1) {
        /* Search for card in field */
        status = detectCard(&nxpparams, bcardUID, &cardUIDlen);
        if (status != 0) {
            fprintf(stderr, "Error detecting card: %d\n", status);
        }
        else if (cardUIDlen > 0) {
            ppos = cardUID;
            for(i = 0; i < cardUIDlen; i++) {
                sprintf(ppos, "%02X", bcardUID[i]);
                ppos += 2;
            }
            ppos = NULL;

            if (vflag) {
                fprintf(stderr, "Found card with UID: %s\n", cardUID);
            }

            memset(&user, '\0', sizeof(rpntts_user));
            status = getUserByCardID(&mysql, cardUID, &user);
            if (status != 0) {
                if (vflag) {
                    fprintf(stderr, "Can not find user, status: %d\n", status);
                }
                sleep(SLEEPSECONDS);
                continue;
            }

            status = doBooking(&mysql, user.pk);
            if (status != 0) {
                fprintf(stderr, "Error during booking: %s\n", mysql_error(&mysql));
            }

        }
        else {
            if (vflag) {
                fprintf(stderr, "Nothing found...\n");
            }
        }

        sleep(SLEEPSECONDS);

    }

    mysql_close(&mysql);

    return 0;
}

void usage(char *progname) {
    fprintf(stderr, "%s: usage: \n", progname);
    fprintf(stderr, "   -v verbose output\n");
    fprintf(stderr, "   -h [host] mysql host to connect to, default localhost\n");
    fprintf(stderr, "   -s [port] mysql tcp socket to connect to, default 3306\n");
    fprintf(stderr, "   -d [db name] mysql db name to connect to, default rpntts\n");
    fprintf(stderr, "   -u [db user] mysql db user, default rpntts\n");
    fprintf(stderr, "   -p [db password] mysql db password, default rpntts\n");
}
