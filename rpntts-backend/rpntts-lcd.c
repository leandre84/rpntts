#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include <lcd.h>
#include "rpntts-lcd.h"

int lcd_init(void) {
    wiringPiSetup();
    /* rows, cols, shift register bits, rs, strb(=e), d0, d1, ... */
    return lcdInit(2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
}

void lcd_print_idle(int lcd_handle) {
    time_t rawtime;
    struct tm *timeinfo;
    char times[9] = { 0 };

    time (&rawtime);
    timeinfo = localtime(&rawtime);
    snprintf(times, 9, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    lcd_clear(lcd_handle);
    lcdPosition(lcd_handle, 0, 0);
    lcdPuts(lcd_handle, "rpntts");
    lcdPosition(lcd_handle, 8, 0);
    lcdPuts(lcd_handle, times);
    lcdPosition(lcd_handle, 0, 1);
    lcdPuts(lcd_handle, "     ready");
}

void lcd_print_user(int lcd_handle, rpnttsUser *user) {
    char line1[17] = { 0 };
    char line2[17] = { 0 };
    unsigned int line2_spacing = 0;

    strncat(line1, user->lastname, 13);
    strcat(line1, " ");
    strncat(line1, user->firstname, 1);
    strcat(line1, ".");

    strncat(line2, LCD_SALDO, strlen(LCD_SALDO));
    line2_spacing = 16-strlen(LCD_SALDO)-strlen(user->timebalance.hourss)-1-strlen(user->timebalance.minutess);
    if (user->timebalance.negative) {
        line2_spacing -= 1;
    }
    while (line2_spacing) {
        strcat(line2, " ");
        line2_spacing--;
    }
    if (user->timebalance.negative) {
        strcat(line2, "-");
    }
    strncat(line2, user->timebalance.hourss, 4);
    strcat(line2, ":");
    strncat(line2, user->timebalance.minutess, 2);

    lcd_clear(lcd_handle);
    lcdPosition(lcd_handle, 0, 0);
    lcdPuts(lcd_handle, line1);
    lcdPosition(lcd_handle, 0, 1);
    lcdPuts(lcd_handle, line2);
}

void lcd_print_text(int lcd_handle, char *line1, char *line2, int delay) {
    lcd_clear(lcd_handle);
    lcdPosition(lcd_handle, 0, 0);
    lcdPuts(lcd_handle, line1);
    lcdPosition(lcd_handle, 0, 1);
    lcdPuts(lcd_handle, line2);
    if (delay > 0) {
       usleep(delay); 
    }
}

void lcd_clear(int lcd_handle) {
    lcdClear(lcd_handle);
}
