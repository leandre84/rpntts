#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "rpntts-mysql.h"

int getUserByCardID(MYSQL *mysql, char *cardUID, rpntts_user *user) {
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char sql[SQLBUF] = { 0 };
    unsigned int i = 0;
    int status = 0;

    strcat(sql, "SELECT user.pk, username, persno, firstname, lastname FROM user, card WHERE card.user_fk = user.pk AND carduid = '");
    strcat(sql, cardUID);
    strcat(sql, "'");

    if ((status = mysql_query(mysql, sql)) != 0) {
        return 1;
    }

    result = mysql_store_result(mysql);
    if (result == NULL) {
        return 2;
    }

    while ((row = mysql_fetch_row(result))) {
        if (i != 0) {
            /* just one result expected */
            return 3;
        }
        if (strlen(row[0]) > USER_PK_LEN ||
                strlen(row[1]) > USER_USERNAME_LEN ||
                strlen(row[2]) > USER_PERSNO_LEN ||
                strlen(row[3]) > USER_FIRSTNAME_LEN ||
                strlen(row[4]) > USER_LASTNAME_LEN 
           ) {
            /* Values unexepectedly larger than data structure */
            return 3;
        }

        strcpy(user->pk, row[0]);
        strcpy(user->username, row[1]);
        strcpy(user->persno, row[2]);
        strcpy(user->firstname, row[3]);
        strcpy(user->lastname, row[4]);

        i++;
    }

    return (i == 1 ? 0 : 4);

}

int doBooking(MYSQL *mysql, char *userpk) {
    char sql[SQLBUF] = { 0 };

    strcat(sql, "INSERT INTO booking (timestamp, text, user_fk) VALUES (now(), 'Automatic booking from rpntts-backend', '");
    strcat(sql, userpk);
    strcat(sql, "')");

    return mysql_query(mysql, sql);
}
