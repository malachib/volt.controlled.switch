#ifdef LCD_ACTIVE
void lcd_setup();
void lcd_showvbat();
void lcd_setBacklight(bool on);
void lcd_off();
void lcd_on();
#else
inline void lcd_setup() {}
inline void lcd_showvbat() {}
inline void lcd_setBacklight(bool on) {}
inline void lcd_off() {}
inline void lcd_on() {}
#endif
