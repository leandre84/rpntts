#ifndef RPNTTS_MYSQL_H
#define RPNTTS_MYSQL_H

#include <my_global.h>
#include <mysql.h>

#define USER_PK_LEN 5 
#define USER_USERNAME_LEN 32
#define USER_PERSNO_LEN 32
#define USER_FIRSTNAME_LEN 64
#define USER_LASTNAME_LEN 64

#define SQLBUF 256

#define BOOKINGDIFF 60

typedef struct {
    char pk[USER_PK_LEN+1];
    char username[USER_USERNAME_LEN+1];
    char persno[USER_PERSNO_LEN+1];
    char firstname[USER_FIRSTNAME_LEN+1];
    char lastname[USER_LASTNAME_LEN+1];
} rpntts_user;

int get_user_by_card_uid(MYSQL *mysql, char *card_uid, rpntts_user *user);
int do_booking(MYSQL *mysql, char *user_pk);
int get_min_bookingtime_diff(MYSQL *mysql, char *user_pk);

#endif /* RPNTTS_MYSQL_H */
