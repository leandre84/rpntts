#ifndef RPNTTS_LCD_H
#define RPNTTS_LCD_H

#include "rpntts-mysql.h"

#define LCD_RS 3
#define LCD_E 2
#define LCD_D4 0
#define LCD_D5 7 
#define LCD_D6 9 
#define LCD_D7 8

#define LCD_SALDO "Saldo: "

int lcd_init(void);
void lcd_print_idle(int lcd_handle);
void lcd_print_user(int lcd_handle, rpnttsUser *user);
void lcd_print_text(int lcd_handle, char *line1, char *line2, int delay);
void lcd_clear(int lcd_handle);

#endif /* RPNTTS_LCD_H */
