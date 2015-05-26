#ifndef RPNTTS_LCD_H
#define RPNTTS_LCD_H

#define LCD_RS 3
#define LCD_E 2
#define LCD_D4 0
#define LCD_D5 7 
#define LCD_D6 9 
#define LCD_D7 8

int lcd_init(void);
void lcd_print_idle(int lcd_handle);
void lcd_clear(int lcd_handle);

#endif /* RPNTTS_LCD_H */
