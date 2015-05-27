#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "rpntts-common.h"
#include "rpntts-mysql.h"

int get_user_by_card_uid(MYSQL *mysql, char *card_uid, rpnttsUser *user) {
    char sql[SQLBUF] = { 0 };
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    unsigned int i = 0;

    strcat(sql, "SELECT user.pk, username, persno, firstname, lastname FROM user, card WHERE card.user_fk=user.pk AND carduid='");
    strcat(sql, card_uid);
    strcat(sql, "' AND user.active='1' AND card.active='1'");

    if (options.verbose) {
        fprintf(stderr, "%s: Executing SQL: %s\n", options.progname, sql);
    }

    if (mysql_query(mysql, sql) != 0) {
        return 1;
    }

    result = mysql_store_result(mysql);
    if (result == NULL) {
        return 2;
    }

    while ((row = mysql_fetch_row(result))) {
        if (i != 0) {
            /* just one result expected */
            mysql_free_result(result);
            return 3;
        }
        if (strlen(row[0]) > USER_PK_LEN ||
                strlen(row[1]) > USER_USERNAME_LEN ||
                strlen(row[2]) > USER_PERSNO_LEN ||
                strlen(row[3]) > USER_FIRSTNAME_LEN ||
                strlen(row[4]) > USER_LASTNAME_LEN 
           ) {
            /* Values unexepectedly larger than data structure */
            mysql_free_result(result);
            return 4;
        }

        strcpy(user->pk, row[0]);
        strcpy(user->username, row[1]);
        strcpy(user->persno, row[2]);
        strcpy(user->firstname, row[3]);
        strcpy(user->lastname, row[4]);

        i++;
    }

    mysql_free_result(result);
    return (i == 1 ? 0 : 4);

}

int get_user_by_nfc_text(MYSQL *mysql, char *nfc_text, rpnttsUser *user) {
    char sql[SQLBUF] = { 0 };
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
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

    if (options.verbose) {
        fprintf(stderr, "%s: Executing SQL: %s\n", options.progname, sql);
    }

    if (mysql_query(mysql, sql) != 0) {
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
            mysql_free_result(result);
            return 3;
        }
        if (strlen(row[0]) > USER_PK_LEN ||
                strlen(row[1]) > USER_USERNAME_LEN ||
                strlen(row[2]) > USER_PERSNO_LEN ||
                strlen(row[3]) > USER_FIRSTNAME_LEN ||
                strlen(row[4]) > USER_LASTNAME_LEN 
           ) {
            /* Values unexepectedly larger than data structure */
            mysql_free_result(result);
            return 4;
        }

        strcpy(user->pk, row[0]);
        strcpy(user->username, row[1]);
        strcpy(user->persno, row[2]);
        strcpy(user->firstname, row[3]);
        strcpy(user->lastname, row[4]);

        i++;
    }

    mysql_free_result(result);
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

    if (options.verbose) {
        fprintf(stderr, "%s: Executing SQL: %s\n", options.progname, sql);
    }

    return mysql_query(mysql, sql);
}

int do_nfc_text_mass_booking(MYSQL *mysql, char *nfc_text, char *user_pk) {
    char sql[SQLBUF] = { 0 };
    unsigned int i = 0;
    char *nfc_text_copy = NULL;
    char *token = NULL;

    nfc_text_copy = alloca(strlen(nfc_text) * sizeof(char) + 1);
    if (nfc_text_copy == NULL) {
        return 1;
    }
    strcpy(nfc_text_copy, nfc_text);

    if (mysql_autocommit(mysql, 0) != 0) {
        return 2;
    }

    token = strtok(nfc_text_copy, "|");
    while (token != NULL) {
        if (i > 2) {
            memset(sql, '\0', SQLBUF*sizeof(char));
            strcat(sql, "INSERT INTO booking (timestamp, text, user_fk) VALUES (str_to_date('");
            strcat(sql, token);
            strcat(sql, "', '%Y%m%d%H%i%s'), 'NDEF derived booking', '");
            strcat(sql, user_pk);
            strcat(sql, "')");
            if (options.verbose) {
                fprintf(stderr, "%s: Executing SQL: %s\n", options.progname, sql);
            }
            if (mysql_query(mysql, sql) != 0) {
                if (mysql_rollback(mysql) != 0) {
                    return 4;
                }
                return 3;
            }
        }
        token = strtok(NULL, "|");
        i++;
    }

    if (mysql_commit(mysql) != 0) {
        (void) mysql_autocommit(mysql, 1);
        return 5;
    }

    (void) mysql_autocommit(mysql, 1);

    return 0;

}

int get_min_bookingtime_diff(MYSQL *mysql, char *user_pk) {
    char sql[SQLBUF] = { 0 };
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    int retval = 0;

    strcat(sql, "select min(timestampdiff(second, timestamp, now())) from booking where user_fk='");
    strcat(sql, user_pk);
    strcat(sql, "'");

    if (options.verbose) {
        fprintf(stderr, "%s: Executing SQL: %s\n", options.progname, sql);
    }

    if (mysql_query(mysql, sql) != 0) {
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
    
    mysql_free_result(result); 
    return retval;
}

int update_user_timebalance(MYSQL *mysql, rpnttsUser *user) {
    char sql[SQLBUF] = { 0 };
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    double tb;

    strcat(sql, "SELECT timebalance FROM user WHERE pk='");
    strcat(sql, user->pk);
    strcat(sql, "'");

    if (options.verbose) {
        fprintf(stderr, "%s: Executing SQL: %s\n", options.progname, sql);
    }

    if (mysql_query(mysql, sql) != 0) {
        return 1;
    }

    result = mysql_store_result(mysql);
    if (result == NULL) {
        return 2;
    }

    row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        return 3;
    }

    if (strlen(row[0]) > USER_TIMEBALANCE_LEN) {
        /* Value unexepectedly larger than data structure */
        mysql_free_result(result);
        return 4;
    }

    strcpy((user->timebalance).text, row[0]);
    sscanf(row[0], "%lf", &tb);
    (user->timebalance).hours = (int) tb;
    (user->timebalance).minutes = (int) ((tb-(user->timebalance).hours)*60);

    mysql_free_result(result);

    return 0;

}

int call_procedure(MYSQL *mysql, char *procedure) {
    char sql[SQLBUF] = { 0 };

    strcat(sql, "CALL ");
    strcat(sql, procedure);
    
    if (options.verbose) {
        fprintf(stderr, "%s: Executing SQL: %s\n", options.progname, sql);
    }

    return mysql_query(mysql, sql);
}
