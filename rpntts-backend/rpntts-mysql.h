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

typedef struct {
    char pk[USER_PK_LEN+1];
    char username[USER_USERNAME_LEN+1];
    char persno[USER_PERSNO_LEN+1];
    char firstname[USER_FIRSTNAME_LEN+1];
    char lastname[USER_LASTNAME_LEN+1];
} rpntts_user;

int getUserByCardID(MYSQL *mysql, char *cardUID, rpntts_user *user);
int doBooking(MYSQL *mysql, char *userpk);

#endif /* RPNTTS_MYSQL_H */
