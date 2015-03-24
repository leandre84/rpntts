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

int main(int argc, char **argv) {
    nxprdlibParams nxp_params;
    uint8_t bcard_uid[MAXUIDLEN];
    uint8_t card_uid_len = 0;
    char card_uid[(MAXUIDLEN*2)+1];
    int status = 0;

    memset(&options, '\0', sizeof(rpnttsOptions));
    options.progname = argv[0];

    /* Initialize GLib main context (needed for OSAL) */
    nxp_params.pHalMainContext = g_main_context_new();
    nxp_params.pHalMainLoop = g_main_loop_new(nxp_params.pHalMainContext, FALSE);

    /* Initialize NXP Reader Library params */
    status = init_nxprdlib(&nxp_params);
    if (status != 0) {
        fprintf(stderr, "%s: Error initializing nxp reader library structures: %d\n", options.progname, status);
        return 1;
    }

    while (1) {

        status = do_discovery_loop(&nxp_params);
        if (status == RPNTTS_NFC_DISCLOOP_NOTHING_FOUND) {
            printf("Nothing found...\n");
            sleep(1);
            break;
        }

        fprintf(stderr, "%s: Disc loop exited with: %d\n", options.progname, status);

        break;

        sleep(1);


        /* Search for card in field 
        status = detect_card(&nxp_params, bcard_uid, &card_uid_len);
        if (status != 0) {
            fprintf(stderr, "%s: Error detecting card: %d\n", options.progname, status);
        }
        else if (card_uid_len > 0) {
            bin_to_hex(bcard_uid, card_uid_len, card_uid);
            if (options.verbose) {
                fprintf(stderr, "%s: Found card with UID: %s\n", options.progname, card_uid);
            }
        }
        */
    }


    return EXIT_SUCCESS;

}
