#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <my_global.h>
#include <mysql.h>

#include "rpntts-nfc.h"
#include "rpntts-mysql.h"

int main(int argc, char** argv) {

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
    int i = 0;
    
    argc = argc;
    argv = argv;

    /* Initialize NXP Reader Library params */
    status = initNxprdlib(&nxpparams, bHalBufferTx, bHalBufferRx);
    if (status != 0) {
        fprintf(stderr, "Error initializing: %d\n", status);
        return 1;
    }

    /* Init mysql structure */
    if (mysql_init(&mysql) == NULL) {
        fprintf(stderr, "Error initializing mysql structure%d\n", status);
        return 2;
    }

    /* Connect to mysql DB */
    if (! mysql_real_connect(&mysql, "localhost", "rpntts", "rpntts", "rpntts", 0, NULL, 0)) {
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
            fprintf(stdout, "Found card with UID: %s\n", cardUID);

            memset(&user, '\0', sizeof(rpntts_user));
            status = getUserByCardID(&mysql, cardUID, &user);
            if (status != 0) {
                fprintf(stderr, "Can not find user, status: %d\n", status);
                continue;
            }

            status = doBooking(&mysql, user.pk);
            if (status != 0) {
                fprintf(stderr, "Error during booking: %s\n", mysql_error(&mysql));
            }

        }
        else {
            fprintf(stdout, "Nothing found...\n");
        }

        /* sleep(1); */
        break;

    }

    mysql_close(&mysql);

    return 0;
}
