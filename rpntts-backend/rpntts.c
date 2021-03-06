#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <alloca.h>
#include <signal.h>

#include <speak_lib.h>

#include "rpntts-common.h"
#include "rpntts-nfc.h"
#include "rpntts-mysql.h"
#include "rpntts-lcd.h"

#define RPNTTS_DEFAULT_DB_HOST "localhost"
#define RPNTTS_DEFAULT_DB_NAME "rpntts"
#define RPNTTS_DEFAULT_DB_USER "rpntts"
#define RPNTTS_DEFAULT_DB_PASSWORD "rpntts"
#define RPNTTS_DEFAULT_DB_PORT 3306 

/* these are useconds */
#define SLEEP_BETWEEN_DETECTIONS 100000
#define SLEEP_AFTER_BOOKING 3000000
#define SLEEP_AFTER_ERROR_DISPLAYED 5000000

#define ESPEAK_BUFFER 500
#define ESPEAK_RATE 180
#define ESPEAK_TEXT_LENGTH 256

#define ESPEAK_ERROR_UNKNOWN_CARD "Unbekannte Karte"
#define ESPEAK_ERROR_INTERVALL "Buchung blockiert - Mindestintervall beachten"

rpnttsOptions options;
volatile unsigned int exit_while = 0;

void usage(char *progname);
void int_handler(int sig);

int main(int argc, char **argv) {
    struct sigaction sa;
    int optopt = 0;
    char *strtolep = NULL;
    nxprdlibParams nxp_params;
    uint8_t bcard_uid[MAXUIDLEN];
    uint8_t card_uid_len = 0;
    char card_uid[(MAXUIDLEN*2)+1];
    MYSQL mysql;
    rpnttsUser user;
    espeak_VOICE espeak_voice;
    espeak_POSITION_TYPE espeak_position_type;
    espeak_ERROR espeak_error;
    char espeak_text[ESPEAK_TEXT_LENGTH+1] = { 0 };
    int lcd_handle = -1;
    int status = 0;

    int disc_loop_status = 0;
    uint8_t tag_type = 0;
    char ndef_text[MAX_NDEF_TEXT];

    uint8_t ndef_mass_booking = 0;

    memset(&options, '\0', sizeof(rpnttsOptions));
    options.progname = argv[0];
    options.db_port = RPNTTS_DEFAULT_DB_PORT;

    while ((optopt = getopt(argc, argv, "vh:d:u:p:s:xq1")) != -1) {
       switch (optopt) {
           case 'v':
            options.verbose = 1;
            break;
           case 'h':
            if ((options.db_host = alloca(strlen(optarg) * sizeof(char) + 1)) == NULL) {
                fprintf(stdout, "%s: Unable to allocate memory for argument \"%c\"\n", options.progname, 'h');
                return -2;
            }
            strcpy(options.db_host, optarg);
            break;
           case 'd':
            if ((options.db_name = alloca(strlen(optarg) * sizeof(char) + 1)) == NULL) {
                fprintf(stdout, "%s: Unable to allocate memory for argument \"%c\"\n", options.progname, 'd');
                return -2;
            }
            strcpy(options.db_name, optarg);
            break;
           case 'u':
            if ((options.db_user = alloca(strlen(optarg) * sizeof(char) + 1)) == NULL) {
                fprintf(stdout, "%s: Unable to allocate memory for argument \"%c\"\n", options.progname, 'u');
                return -2;
            }
            strcpy(options.db_user, optarg);
            break;
           case 'p':
            if ((options.db_password = alloca(strlen(optarg) * sizeof(char) + 1)) == NULL) {
                fprintf(stdout, "%s: Unable to allocate memory for argument \"%c\"\n", options.progname, 'p');
                return -2;
            }
            strcpy(options.db_password, optarg);
            break;
           case 's':
            options.db_port = strtoul(optarg, &strtolep, 10);
            if (strlen(strtolep) > 0) {
                fprintf(stdout, "%s: Invalid port argument given: %s\n", options.progname, optarg);
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
        if ((options.db_host = alloca(strlen(RPNTTS_DEFAULT_DB_HOST) * sizeof(char) + 1)) == NULL) {
                fprintf(stdout, "%s: Unable to allocate memory for default db host\n", options.progname);
                return -2;
        }
        strcpy(options.db_host, RPNTTS_DEFAULT_DB_HOST);
    }
    if (options.db_name == NULL) {
        if ((options.db_name = alloca(strlen(RPNTTS_DEFAULT_DB_NAME) * sizeof(char) + 1)) == NULL) {
                fprintf(stdout, "%s: Unable to allocate memory for default db name\n", options.progname);
                return -2;
        }
        strcpy(options.db_name, RPNTTS_DEFAULT_DB_NAME);
    }
    if (options.db_user == NULL) {
        if ((options.db_user = alloca(strlen(RPNTTS_DEFAULT_DB_USER) * sizeof(char) + 1)) == NULL) {
                fprintf(stdout, "%s: Unable to allocate memory for default db user\n", options.progname);
                return -2;
        }
        strcpy(options.db_user, RPNTTS_DEFAULT_DB_USER);
    }
    if (options.db_password == NULL) {
        if ((options.db_password = alloca(strlen(RPNTTS_DEFAULT_DB_PASSWORD) * sizeof(char) + 1)) == NULL) {
                fprintf(stdout, "%s: Unable to allocate memory for default db password\n", options.progname);
                return -2;
        }
        strcpy(options.db_password, RPNTTS_DEFAULT_DB_PASSWORD);
    }

    /* Configure signal handler */
    sa.sa_handler = int_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* Initialize GLib main context (needed for OSAL) */
    nxp_params.pHalMainContext = g_main_context_new();
    nxp_params.pHalMainLoop = g_main_loop_new(nxp_params.pHalMainContext, FALSE);

    /* Initialize NXP Reader Library params */
    status = init_nxprdlib(&nxp_params);
    if (status != 0) {
        fprintf(stdout, "%s: Error initializing nxp reader library structures: %d\n", options.progname, status);
        return 1;
    }

    /* Configure discovery loop */
    status = configure_dicovery_loop(&nxp_params);
    if (status != 0) {
        fprintf(stdout, "%s: Error configuring discovery loop: %d\n", options.progname, status);
        return 2;
    }

    /* Init mysql structure */
    if (mysql_init(&mysql) == NULL) {
        fprintf(stdout, "%s: Error initializing mysql structure: %d\n", options.progname, status);
        return 3;
    }

    /* Init LCD */
    lcd_handle = lcd_init();
    if (lcd_handle < 0) {
        fprintf(stdout, "%s: Error initializing LCD\n", options.progname);
        return 4;
    }

    /* Init espeak */
    if (! options.quiet) {
        memset(&espeak_voice, '\0', sizeof(espeak_VOICE));
        memset(&espeak_position_type, '\0', sizeof(espeak_POSITION_TYPE));
        espeak_voice.languages = "de";
        espeak_voice.gender = 1;
        espeak_voice.variant = 1;
        if ((espeak_error = espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, ESPEAK_BUFFER, NULL, 0)) == EE_INTERNAL_ERROR) {
            fprintf(stdout, "%s: Error initializing espeak: %d\n", options.progname, espeak_error);
            return 5;
        }
        if ((espeak_error = espeak_SetVoiceByProperties(&espeak_voice)) != EE_OK) {
            fprintf(stdout, "%s: Error setting espeak voice: %d\n", options.progname, espeak_error);
            return 6;
        }
        if ((espeak_error = espeak_SetParameter(espeakRATE, ESPEAK_RATE, 0)) != EE_OK) {
            fprintf(stdout, "%s: Error setting espeak rate: %d\n", options.progname, espeak_error);
            return 7;
        }

        strncpy(espeak_text, "rpntts initialisiert, akzeptiere Buchungen", ESPEAK_TEXT_LENGTH);
        espeak_Synth(espeak_text, sizeof(espeak_text), 0, espeak_position_type, 0, espeakCHARS_AUTO, NULL, NULL);
        //espeak_Synchronize();
    }

    /* Connect to mysql DB */
    if (! mysql_real_connect(&mysql, options.db_host, options.db_user, options.db_password, options.db_name, options.db_port, NULL, 0)) {
        fprintf(stdout, "%s: Error connecting to mysql: %s\n", options.progname, mysql_error(&mysql));
        mysql_close(&mysql);
        return 8;
    }

    while (exit_while == 0) {

        lcd_print_idle(lcd_handle);

        usleep(SLEEP_BETWEEN_DETECTIONS);

        ndef_mass_booking = 0;

        /* Search for card in field */
        status = detect_card(&nxp_params, bcard_uid, &card_uid_len);
        if (status != 0) {
            fprintf(stdout, "%s: Error detecting card: %d\n", options.progname, status);
        }
        else if (card_uid_len > 0) {

            /* UID to string */
            bin_to_hex(bcard_uid, card_uid_len, card_uid);

            if (options.verbose) {
                fprintf(stdout, "%s: Found card with UID: %s\n", options.progname, card_uid);
            }


            /* Lookup user corresponding to card UID */
            memset(&user, '\0', sizeof(rpnttsUser));
            status = get_user_by_card_uid(&mysql, card_uid, &user);
            if (status != 0) {
                if (options.verbose) {
                    fprintf(stdout, "%s: Can not find user for card uid \"%s\", status: %d\n", options.progname, card_uid, status);
                }
                /* Check ndef presence */
                disc_loop_status = do_discovery_loop(&nxp_params);
                if (disc_loop_status == RPNTTS_NFC_DISCLOOP_DETECTED_T1T ||
                        disc_loop_status == RPNTTS_NFC_DISCLOOP_DETECTED_T2T ||
                        disc_loop_status == RPNTTS_NFC_DISCLOOP_DETECTED_T4T) {

                    if (disc_loop_status == RPNTTS_NFC_DISCLOOP_DETECTED_T1T) tag_type = PHAL_TOP_TAG_TYPE_T1T_TAG;
                    if (disc_loop_status == RPNTTS_NFC_DISCLOOP_DETECTED_T2T) tag_type = PHAL_TOP_TAG_TYPE_T2T_TAG;
                    if (disc_loop_status == RPNTTS_NFC_DISCLOOP_DETECTED_T4T) tag_type = PHAL_TOP_TAG_TYPE_T4T_TAG;

                    if (detect_ndef(&nxp_params, tag_type) == RPNTTS_NFC_DETECTNDEF_NDEFPRESENT) {
                        memset(ndef_text, '\0', MAX_NDEF_TEXT);
                        if (get_ndef_text(&nxp_params, tag_type, ndef_text) == 0 ) {
                            if (options.verbose) {
                                fprintf(stdout, "%s: NDEF Text: %s\n", options.progname, ndef_text);
                            }
                            /* Try to look up user by NFC text record infos */
                            status = get_user_by_nfc_text(&mysql, ndef_text, &user);
                            if (status == 0) {
                                if (options.verbose) {
                                    fprintf(stdout, "%s: Found user via NDEF text record, PK: %s\n", options.progname, user.pk);
                                }
                                ndef_mass_booking = 1;
                                /* Proceed with booking, we're not break/continuing the loop here */
                            } else {
                                fprintf(stdout, "%s: Unable to look up user according to NFC text record: %d\n", options.progname, status);
                                lcd_print_text(lcd_handle, "  rpntts ERROR", "NDEF user lookup", SLEEP_AFTER_ERROR_DISPLAYED);
                                if (options.single_run) break; else continue; 
                            }
                        }
                        else { 
                            if (options.verbose) {
                                fprintf(stdout, "%s: NDEF record detected, but there is no text record\n", options.progname);
                                lcd_print_text(lcd_handle, "  rpntts ERROR", "NDEF user lookup", SLEEP_AFTER_ERROR_DISPLAYED);
                            }
                            if (options.single_run) break; else continue; 
                        }
                    }
                    else {
                        if (options.verbose) {
                            fprintf(stdout, "%s: No NDEF record detected\n", options.progname);
                            if (options.quiet) {
                                lcd_print_text(lcd_handle, "  rpntts ERROR", "  unknown card", SLEEP_AFTER_ERROR_DISPLAYED);
                            }
                            else {
                                lcd_print_text(lcd_handle, "  rpntts ERROR", "  unknown card", 0);
                                espeak_Synth(ESPEAK_ERROR_UNKNOWN_CARD, sizeof(ESPEAK_ERROR_UNKNOWN_CARD), 0, espeak_position_type, 0, espeakCHARS_AUTO, NULL, NULL);
                                espeak_Synchronize();
                            }
                        }
                        if (options.single_run) break; else continue; 
                    }
                }
                /* this should not happen */
                else {
                    if (options.verbose) {
                        fprintf(stdout, "%s: No tag detected\n", options.progname);
                    }
                    if (options.single_run) break; else continue; 
                }
            }

            if (options.no_booking) {
                if (options.single_run) break; else continue; 
            }

            /* Do booking */
            if (ndef_mass_booking == 0) {
                status = do_booking(&mysql, user.pk);
                if (status != 0) {
                    fprintf(stdout, "%s: Error during booking: ", options.progname);
                    if (status == -1) {
                        fprintf(stdout, "booking time intervall below limit\n");
                        if (options.quiet) {
                            lcd_print_text(lcd_handle, "rpntts ERROR", "blocked 1m", SLEEP_AFTER_ERROR_DISPLAYED);
                        }
                        else {
                            lcd_print_text(lcd_handle, "rpntts ERROR", "blocked 1m", 0);
                            espeak_Synth(ESPEAK_ERROR_INTERVALL, sizeof(ESPEAK_ERROR_INTERVALL), 0, espeak_position_type, 0, espeakCHARS_AUTO, NULL, NULL);
                            espeak_Synchronize();
                        }
                    }
                    else {
                        fprintf(stdout, "%s\n", mysql_error(&mysql));
                        lcd_print_text(lcd_handle, "rpntts ERROR", "during booking", SLEEP_AFTER_ERROR_DISPLAYED);
                    }
                    if (options.single_run) break; else continue; 
                }
                else if (options.verbose) {
                    fprintf(stdout, "%s: Successfully performed booking\n", options.progname);
                }
            }
            else {
                /* Do mass booking */
                status = do_nfc_text_mass_booking(&mysql, ndef_text, user.pk);
                if (status != 0) {
                    fprintf(stdout, "%s: Error during mass booking, status: %d, mysql error: %s\n", options.progname, status, mysql_error(&mysql));
                    lcd_print_text(lcd_handle, "rpntts ERROR", "NFC mass booking", SLEEP_AFTER_ERROR_DISPLAYED);
                    if (options.single_run) break; else continue; 
                }
                else if (options.verbose) {
                    fprintf(stdout, "%s: Successfully performed booking\n", options.progname);
                }
            }

            /* Get Saldo etc. */

            status = call_procedure(&mysql, "rpntts_update_global_saldo()");
            if (status != 0) {
                fprintf(stdout, "%s: Error calculating global saldo through procedure: %s\n", options.progname, mysql_error(&mysql));
                lcd_print_text(lcd_handle, "rpntts ERROR", "update saldo", SLEEP_AFTER_ERROR_DISPLAYED);
                if (options.single_run) break; else continue; 
            }

            status = update_user_timebalance(&mysql, &user);
            if (status != 0) {
                fprintf(stdout, "%s: Error updating user's time balance: %s\n", options.progname, mysql_error(&mysql));
                lcd_print_text(lcd_handle, "rpntts ERROR", "no timebalance", SLEEP_AFTER_ERROR_DISPLAYED);
                if (options.single_run) break; else continue; 
            }
            else if (options.verbose) {
                fprintf(stdout, "%s: User info: PK: %s timebalance: %s, Hours: %d, Minutes: %d, negative: %d\n", options.progname, user.pk, user.timebalance.text, user.timebalance.hours, user.timebalance.minutes, user.timebalance.negative);
            }

            lcd_print_user(lcd_handle, &user);

            /* Read saldo */
            if (! options.quiet) {
                memset(espeak_text, '\0', ESPEAK_TEXT_LENGTH);
                strcat(espeak_text, "Hallo ");
                strcat(espeak_text, user.firstname);
                strcat(espeak_text, " ");
                strcat(espeak_text, user.lastname);
                strcat(espeak_text, ", Ihr neues Saldo beträgt: ");
                if (user.timebalance.negative) {
                    strcat(espeak_text, "minus ");
                }
                strcat(espeak_text, user.timebalance.hourss);
                strcat(espeak_text, " Stunden und ");
                strcat(espeak_text, user.timebalance.minutess);
                strcat(espeak_text, " Minuten.");
                espeak_Synth(espeak_text, sizeof(espeak_text), 0, espeak_position_type, 0, espeakCHARS_AUTO, NULL, NULL);
                espeak_Synchronize();
            }

            if (options.quiet) {
                usleep(SLEEP_AFTER_BOOKING);
            }               

        }
        /*
        else {
            if (options.verbose) {
                fprintf(stdout, "%s: Nothing found...\n", options.progname);
            }
        }
        */

        if (options.single_run) {
            break;
        }

    }

    if (options.verbose) {
        fprintf(stdout, "%s: Exiting cleanly\n", options.progname);
    }

    mysql_close(&mysql);
    lcd_clear(lcd_handle);


    return EXIT_SUCCESS;

}

void usage(char *progname) {
    fprintf(stdout, "%s: usage: \n", progname);
    fprintf(stdout, "   -1 exit after one run\n");
    fprintf(stdout, "   -v verbose output\n");
    fprintf(stdout, "   -x do not insert booking - dry run\n");
    fprintf(stdout, "   -q be quiet - no espeak output\n");
    fprintf(stdout, "   -h [host] mysql host to connect to, default localhost\n");
    fprintf(stdout, "   -s [port] mysql tcp socket to connect to, default 3306\n");
    fprintf(stdout, "   -d [db name] mysql db name to connect to, default rpntts\n");
    fprintf(stdout, "   -u [db user] mysql db user, default rpntts\n");
    fprintf(stdout, "   -p [db password] mysql db password, default rpntts\n");
}

void int_handler(int sig) {
    sig = sig;
    fprintf(stdout, "%s: Cought signal, exiting...\n", options.progname);
    exit_while = 1;
}
