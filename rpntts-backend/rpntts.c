#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include <speak_lib.h>

#include "rpntts-nfc.h"
#include "rpntts-mysql.h"

#define SLEEPUSECONDS 2000000

#define ESPEAK_BUFFER 500
#define ESPEAK_RATE 180
#define ESPEAK_TEXT_LENGTH 256

void usage(char *progname);

int main(int argc, char **argv) {

    int optopt = 0;
    int vflag = 0;
    int xflag = 0;
    char *dbhost = NULL;
    char *dbname = NULL;
    char *dbuser = NULL;
    char *dbpassword = NULL;
    uint16_t dbtcpsocket = 3306;
    const char dbhostdefault[] = "localhost";
    const char dbnamedefault[] = "rpntts";
    const char dbuserdefault[] = "rpntts";
    const char dbpassworddefault[] = "rpntts";
    char *strtolep = NULL;
    nxprdlibparams nxpparams;
    uint8_t bHalBufferTx[HALBUFSIZE];
    uint8_t bHalBufferRx[HALBUFSIZE];
    uint8_t bcardUID[MAXUIDLEN];
    uint8_t cardUIDlen = 0;
    char cardUID[(MAXUIDLEN*2)+1];
    char *ppos = NULL;
    MYSQL mysql;
    rpntts_user user;
    espeak_VOICE espeak_voice;
    espeak_POSITION_TYPE espeak_position_type;
    espeak_ERROR espeak_error;
    char espeak_text[ESPEAK_TEXT_LENGTH] = { 0 };
    int status = 0;
    unsigned int i = 0;

    while ((optopt = getopt(argc, argv, "vh:d:u:p:s:x")) != -1) {
       switch (optopt) {
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
           case 'x':
            xflag = 1;
            break;
           default:
            usage(argv[0]);
            return -1;
       }
    }

    if (dbhost == NULL) {
        dbhost = (char*) dbhostdefault;
    }
    if (dbname == NULL) {
        dbname = (char*) dbnamedefault;
    }
    if (dbuser == NULL) {
        dbuser = (char*) dbuserdefault;
    }
    if (dbpassword == NULL) {
        dbpassword = (char*) dbpassworddefault;
    }

    /* Initialize NXP Reader Library params */
    status = initNxprdlib(&nxpparams, bHalBufferTx, bHalBufferRx);
    if (status != 0) {
        fprintf(stderr, "Error initializing nxp reader library structures: %d\n", status);
        return 1;
    }

    /* Init mysql structure */
    if (mysql_init(&mysql) == NULL) {
        fprintf(stderr, "Error initializing mysql structure: %d\n", status);
        return 2;
    }

    /* Init espeak */
    memset(&espeak_voice, '\0', sizeof(espeak_VOICE));
    memset(&espeak_position_type, '\0', sizeof(espeak_POSITION_TYPE));
    espeak_voice.languages = "de";
    espeak_voice.gender = 1;
    espeak_voice.variant = 1;
    if ((espeak_error = espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, ESPEAK_BUFFER, NULL, 0)) == EE_INTERNAL_ERROR) {
        fprintf(stderr, "Error initializing espeak: %d\n", espeak_error);
        return 3;
    }
    if ((espeak_error = espeak_SetVoiceByProperties(&espeak_voice)) != EE_OK) {
        fprintf(stderr, "Error setting espeak voice: %d\n", espeak_error);
        return 4;
    }
    if ((espeak_error = espeak_SetParameter(espeakRATE, ESPEAK_RATE, 0)) != EE_OK) {
        fprintf(stderr, "Error setting espeak rate: %d\n", espeak_error);
        return 5;
    }

    strncpy(espeak_text, "rpntts initialisiert, akzeptiere Buchungen", ESPEAK_TEXT_LENGTH-1);
    espeak_Synth(espeak_text, sizeof(espeak_text), 0, espeak_position_type, 0, espeakCHARS_AUTO, NULL, NULL);
    espeak_Synchronize();

    while (1) {

        /* Search for card in field */
        status = detectCard(&nxpparams, bcardUID, &cardUIDlen);
        if (status != 0) {
            fprintf(stderr, "Error detecting card: %d\n", status);
        }

        else if (cardUIDlen > 0) {

            if (vflag) {
                fprintf(stderr, "Found card with UID: %s\n", cardUID);
            }

            /* UID to string */
            ppos = cardUID;
            for(i = 0; i < cardUIDlen; i++) {
                sprintf(ppos, "%02X", bcardUID[i]);
                ppos += 2;
            }
            ppos = NULL;

            if (xflag) {
                usleep(SLEEPUSECONDS);
                continue;
            }

            /* Connect to mysql DB */
            if (! mysql_real_connect(&mysql, dbhost, dbuser, dbpassword, dbname, dbtcpsocket, NULL, 0)) {
                fprintf(stderr, "Error connecting to mysql: %s\n", mysql_error(&mysql));
                mysql_close(&mysql);
                usleep(SLEEPUSECONDS);
                continue;
            }

            /* Lookup user corresponding to card UID */
            memset(&user, '\0', sizeof(rpntts_user));
            status = getUserByCardID(&mysql, cardUID, &user);
            if (status != 0) {
                if (vflag) {
                    fprintf(stderr, "Can not find user, status: %d\n", status);
                }
                mysql_close(&mysql);
                usleep(SLEEPUSECONDS);
                continue;
            }

            /* Do booking */
            status = doBooking(&mysql, user.pk);
            if (status != 0) {
                fprintf(stderr, "Error during booking: ");
                if (status == -1) {
                    fprintf(stderr, "booking time intervall below limit\n");
                }
                else {
                    fprintf(stderr, "%s\n", mysql_error(&mysql));
                }
                mysql_close(&mysql);
            }
            else if (vflag) {
                fprintf(stderr, "INFO: Successfully performed booking\n");
            }

            mysql_close(&mysql);

        }
        else {
            if (vflag) {
                fprintf(stderr, "Nothing found...\n");
            }
        }

        usleep(SLEEPUSECONDS);

    }

    mysql_close(&mysql);

    return EXIT_SUCCESS;

}

void usage(char *progname) {
    fprintf(stderr, "%s: usage: \n", progname);
    fprintf(stderr, "   -v verbose output\n");
    fprintf(stderr, "   -x just try to detect card, no mysql interaction\n");
    fprintf(stderr, "   -h [host] mysql host to connect to, default localhost\n");
    fprintf(stderr, "   -s [port] mysql tcp socket to connect to, default 3306\n");
    fprintf(stderr, "   -d [db name] mysql db name to connect to, default rpntts\n");
    fprintf(stderr, "   -u [db user] mysql db user, default rpntts\n");
    fprintf(stderr, "   -p [db password] mysql db password, default rpntts\n");
}
