#include <stdio.h>
#include "rpntts-common.h"

void bin_to_hex(uint8_t *in, uint16_t in_length, char *out) {
    char *ppos;
    uint16_t i;

    ppos = out;
    for(i = 0; i < in_length; i++) {
        sprintf(ppos, "%02X", in[i]);
        ppos += 2;
    }
}
