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
    lcd_clear(lcd_handle);
    lcdPosition(lcd_handle, 0, 0);
    lcdPuts(lcd_handle, "rpntts");
    lcdPosition(lcd_handle, 0, 1);
    lcdPuts(lcd_handle, "ready");
}

void lcd_clear(int lcd_handle) {
    lcdClear(lcd_handle);
}
