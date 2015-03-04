#include <stdlib.h>
#include <stdio.h>

#include "rpntts-nfc.h"

int main(int argc, char** argv) {

    nxprdlibparams nxpparams;
    uint8_t bHalBufferTx[HALBUFSIZE];
    uint8_t bHalBufferRx[HALBUFSIZE];

    int8_t status = 0;
    
    argc = argc;
    argv = argv;


    status = initNxprdlib(&nxpparams, bHalBufferTx, bHalBufferRx);
    if (status != 0) {
        fprintf(stderr, "Error initializing: %d\n", status);
    }

    return 0;
}
