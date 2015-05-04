#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "rpntts-mysql.h"

int get_user_by_card_uid(MYSQL *mysql, char *card_uid, rpntts_user *user) {
    char sql[SQLBUF] = { 0 };
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    int status = 0;
    unsigned int i = 0;

    strcat(sql, "SELECT user.pk, username, persno, firstname, lastname FROM user, card WHERE card.user_fk=user.pk AND carduid='");
    strcat(sql, card_uid);
    strcat(sql, "' AND user.active='1' AND card.active='1'");

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

int get_user_by_nfc_text(MYSQL *mysql, char *nfc_text, rpntts_user *user) {
    char sql[SQLBUF] = { 0 };
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    int status = 0;
    unsigned int i = 0;
    char *nfc_text_copy = NULL;
    char *token = NULL;
    char *username = NULL;
    char *password = NULL;

    nfc_text_copy = alloca(strlen(nfc_text) * sizeof(char) + 1);
    if (nfc_text_copy == NULL) {
        return 1;
    }

    strcpy(nfc_text_copy, nfc_text);
    token = strtok(nfc_text_copy, "|");
    while (token != NULL) {
        if (i == 0) {
            if (strcmp(token, "rpntts") != 0) {
                return 2;
            }
        }
        else if (i == 1) {
            username = alloca(strlen(token) * sizeof(char) + 1);
            if (username == NULL) {
                return 1;
            }
            strcpy(username, token);
        }
        else if (i == 2) {
            password = alloca(strlen(token) * sizeof(char) + 1);
            if (password == NULL) {
                return 1;
            }
            strcpy(password, token);
        }
        else if (i > 2) {
            /* Don't care */
            break;
        }
        token = strtok(NULL, "|");
        i++;
    }

    strcat(sql, "SELECT user.pk, username, persno, firstname, lastname FROM user WHERE username='");
    strcat(sql, username);
    strcat(sql, "' AND password='");
    strcat(sql, password);
    strcat(sql, "' AND active='1'");

    if ((status = mysql_query(mysql, sql)) != 0) {
        return 1;
    }

    result = mysql_store_result(mysql);
    if (result == NULL) {
        return 2;
    }

    i = 0;
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

int do_booking(MYSQL *mysql, char *user_pk) {
    char sql[SQLBUF] = { 0 };
    int diff = 0;

    strcat(sql, "INSERT INTO booking (timestamp, text, user_fk) VALUES (now(), 'Automatic booking from rpntts-backend', '");
    strcat(sql, user_pk);
    strcat(sql, "')");

    if ((diff = get_min_bookingtime_diff(mysql, user_pk)) < BOOKINGDIFF) {
        return -1;
    }

    return mysql_query(mysql, sql);
}

int do_nfc_text_mass_booking(MYSQL *mysql, char *nfc_text, char *user_pk) {
}

int get_min_bookingtime_diff(MYSQL *mysql, char *user_pk) {
    char sql[SQLBUF] = { 0 };
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    int status = 0;
    int retval = 0;

    strcat(sql, "select min(timestampdiff(second, timestamp, now())) from booking where user_fk='");
    strcat(sql, user_pk);
    strcat(sql, "'");

    if ((status = mysql_query(mysql, sql)) != 0) {
        return 1;
    }

    result = mysql_store_result(mysql);
    if (result == NULL) {
        return 2;
    }

    row = mysql_fetch_row(result);

    if (row == NULL || row[0] == NULL) {
        /* First booking */
        retval = BOOKINGDIFF;
    }
    else {
        retval = strtol(row[0], NULL, 10);
    }

    return retval;
}

