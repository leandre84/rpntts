#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <alloca.h>

#include <speak_lib.h>

#include "rpntts-common.h"
#include "rpntts-nfc.h"
#include "rpntts-mysql.h"

#define RPNTTS_DEFAULT_DB_HOST "localhost"
#define RPNTTS_DEFAULT_DB_NAME "rpntts"
#define RPNTTS_DEFAULT_DB_USER "rpntts"
#define RPNTTS_DEFAULT_DB_PASSWORD "rpntts"
#define RPNTTS_DEFAULT_DB_PORT 3306 


#define SLEEPUSECONDS 200000

#define ESPEAK_BUFFER 500
#define ESPEAK_RATE 180
#define ESPEAK_TEXT_LENGTH 256

rpnttsOptions options;

void usage(char *progname);

int main(int argc, char **argv) {
    int optopt = 0;
    char *strtolep = NULL;
    nxprdlibParams nxp_params;
    uint8_t bcard_uid[MAXUIDLEN];
    uint8_t card_uid_len = 0;
    char card_uid[(MAXUIDLEN*2)+1];
    MYSQL mysql;
    rpntts_user user;
    espeak_VOICE espeak_voice;
    espeak_POSITION_TYPE espeak_position_type;
    espeak_ERROR espeak_error;
    char espeak_text[ESPEAK_TEXT_LENGTH] = { 0 };
    int status = 0;

    memset(&options, '\0', sizeof(rpnttsOptions));
    options.progname = argv[0];
    options.db_port = RPNTTS_DEFAULT_DB_PORT;

    while ((optopt = getopt(argc, argv, "vh:d:u:p:s:xq1")) != -1) {
       switch (optopt) {
           case 'v':
            options.verbose = 1;
            break;
           case 'h':
            if ((options.db_host = alloca(strlen(optarg)+1)) == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for argument \"%c\"\n", options.progname, 'h');
                return -2;
            }
            strcpy(options.db_host, optarg);
            break;
           case 'd':
            if ((options.db_name = alloca(strlen(optarg)+1)) == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for argument \"%c\"\n", options.progname, 'd');
                return -2;
            }
            strcpy(options.db_name, optarg);
            break;
           case 'u':
            if ((options.db_user = alloca(strlen(optarg)+1)) == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for argument \"%c\"\n", options.progname, 'u');
                return -2;
            }
            strcpy(options.db_user, optarg);
            break;
           case 'p':
            if ((options.db_password = alloca(strlen(optarg)+1)) == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for argument \"%c\"\n", options.progname, 'p');
                return -2;
            }
            strcpy(options.db_password, optarg);
            break;
           case 's':
            options.db_port = strtoul(optarg, &strtolep, 10);
            if (strlen(strtolep) > 0) {
                fprintf(stderr, "%s: Invalid port argument given: %s\n", options.progname, optarg);
                return -3;
            }
            break;
           case 'x':
            options.no_booking = 1;
            break;
           case 'q':
            options.quiet = 1;
            break;
           case '1':
            options.single_run = 1;
            break;
           default:
            usage(argv[0]);
            return -1;
       }
    }

    if (options.db_host == NULL) {
        if ((options.db_host = alloca(strlen(RPNTTS_DEFAULT_DB_HOST+1))) == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for default db host\n", options.progname);
                return -2;
        }
        strcpy(options.db_host, RPNTTS_DEFAULT_DB_HOST);
    }
    if (options.db_name == NULL) {
        if ((options.db_name = alloca(strlen(RPNTTS_DEFAULT_DB_NAME+1))) == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for default db name\n", options.progname);
                return -2;
        }
        strcpy(options.db_name, RPNTTS_DEFAULT_DB_NAME);
    }
    if (options.db_user == NULL) {
        if ((options.db_user = alloca(strlen(RPNTTS_DEFAULT_DB_USER+1))) == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for default db user\n", options.progname);
                return -2;
        }
        strcpy(options.db_user, RPNTTS_DEFAULT_DB_USER);
    }
    if (options.db_password == NULL) {
        if ((options.db_password = alloca(strlen(RPNTTS_DEFAULT_DB_PASSWORD+1))) == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for default db password\n", options.progname);
                return -2;
        }
        strcpy(options.db_password, RPNTTS_DEFAULT_DB_PASSWORD);
    }

    /* Initialize GLib main context (needed for OSAL) */
    nxp_params.pHalMainContext = g_main_context_new();
    nxp_params.pHalMainLoop = g_main_loop_new(nxp_params.pHalMainContext, FALSE);

    /* Initialize NXP Reader Library params */
    status = init_nxprdlib(&nxp_params);
    if (status != 0) {
        fprintf(stderr, "%s: Error initializing nxp reader library structures: %d\n", options.progname, status);
        return 1;
    }

    /* Init mysql structure */
    if (mysql_init(&mysql) == NULL) {
        fprintf(stderr, "%s: Error initializing mysql structure: %d\n", options.progname, status);
        return 2;
    }

    /* Init espeak */
    if (! options.quiet) {
        memset(&espeak_voice, '\0', sizeof(espeak_VOICE));
        memset(&espeak_position_type, '\0', sizeof(espeak_POSITION_TYPE));
        espeak_voice.languages = "de";
        espeak_voice.gender = 1;
        espeak_voice.variant = 1;
        if ((espeak_error = espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, ESPEAK_BUFFER, NULL, 0)) == EE_INTERNAL_ERROR) {
            fprintf(stderr, "%s: Error initializing espeak: %d\n", options.progname, espeak_error);
            return 3;
        }
        if ((espeak_error = espeak_SetVoiceByProperties(&espeak_voice)) != EE_OK) {
            fprintf(stderr, "%s: Error setting espeak voice: %d\n", options.progname, espeak_error);
            return 4;
        }
        if ((espeak_error = espeak_SetParameter(espeakRATE, ESPEAK_RATE, 0)) != EE_OK) {
            fprintf(stderr, "%s: Error setting espeak rate: %d\n", options.progname, espeak_error);
            return 5;
        }

        strncpy(espeak_text, "rpntts initialisiert, akzeptiere Buchungen", ESPEAK_TEXT_LENGTH-1);
        espeak_Synth(espeak_text, sizeof(espeak_text), 0, espeak_position_type, 0, espeakCHARS_AUTO, NULL, NULL);
        espeak_Synchronize();
    }

    while (1) {

        /* Search for card in field */
        status = detect_card(&nxp_params, bcard_uid, &card_uid_len);
        if (status != 0) {
            fprintf(stderr, "%s: Error detecting card: %d\n", options.progname, status);
        }
        else if (card_uid_len > 0) {

            /* UID to string */
            bin_to_hex(bcard_uid, card_uid_len, card_uid);

            if (options.verbose) {
                fprintf(stderr, "%s: Found card with UID: %s\n", options.progname, card_uid);
            }

            /* Check ndef presence */
            if (options.verbose) {
                fprintf(stderr, "%s: Disc loop exited with: %d\n", options.progname, do_discovery_loop(&nxp_params));
            }

            if (options.no_booking) {
                usleep(SLEEPUSECONDS);
                if (options.single_run) {
                    break;
                }
                continue;
            }

            /* Connect to mysql DB */
            if (! mysql_real_connect(&mysql, options.db_host, options.db_user, options.db_password, options.db_name, options.db_port, NULL, 0)) {
                fprintf(stderr, "%s: Error connecting to mysql: %s\n", options.progname, mysql_error(&mysql));
                mysql_close(&mysql);
                usleep(SLEEPUSECONDS);
                continue;
            }

            /* Lookup user corresponding to card UID */
            memset(&user, '\0', sizeof(rpntts_user));
            status = get_user_by_card_uid(&mysql, card_uid, &user);
            if (status != 0) {
                if (options.verbose) {
                    fprintf(stderr, "%s: Can not find user, status: %d\n", options.progname, status);
                }
                mysql_close(&mysql);
                usleep(SLEEPUSECONDS);
                continue;
            }

            /* Greet user */
            if (! options.quiet) {
                memset(espeak_text, '\0', ESPEAK_TEXT_LENGTH);
                strcat(espeak_text, "Hallo ");
                strcat(espeak_text, user.firstname);
                strcat(espeak_text, " ");
                strcat(espeak_text, user.lastname);
                espeak_Synth(espeak_text, sizeof(espeak_text), 0, espeak_position_type, 0, espeakCHARS_AUTO, NULL, NULL);
                espeak_Synchronize();
            }

            /* Do booking */
            status = do_booking(&mysql, user.pk);
            if (status != 0) {
                fprintf(stderr, "%s: Error during booking: ", options.progname);
                if (status == -1) {
                    fprintf(stderr, "booking time intervall below limit\n");
                }
                else {
                    fprintf(stderr, "%s\n", mysql_error(&mysql));
                }
                mysql_close(&mysql);
            }
            else if (options.verbose) {
                fprintf(stderr, "%s: Successfully performed booking\n", options.progname);
            }

            mysql_close(&mysql);

        }
        else {
            if (options.verbose) {
                fprintf(stderr, "%s: Nothing found...\n", options.progname);
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
    fprintf(stderr, "   -q be quiet - no espeak output\n");
    fprintf(stderr, "   -h [host] mysql host to connect to, default localhost\n");
    fprintf(stderr, "   -s [port] mysql tcp socket to connect to, default 3306\n");
    fprintf(stderr, "   -d [db name] mysql db name to connect to, default rpntts\n");
    fprintf(stderr, "   -u [db user] mysql db user, default rpntts\n");
    fprintf(stderr, "   -p [db password] mysql db password, default rpntts\n");
}
