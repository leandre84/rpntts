#ifndef RPNTTS_COMMON_H
#define RPNTTS_COMMON_H

#include <inttypes.h>

typedef struct {
    char **progname;
    int verbose;
    int no_booking;
    int quiet;
    char *db_host;
    char *db_name;
    char *db_user;
    char *db_password;
    uint16_t db_port;
} rpnttsOptions;

extern rpnttsOptions options;

void bin_to_hex(uint8_t *in, uint8_t in_length, char *out);

#endif /* RPNTTS_COMMON_H */
