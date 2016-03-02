#include <Arduino.h>

#include "features.h"
#include "main.h"
#include "lcd.h"

#ifdef LCD_ACTIVE
#include <TinyWireM.h>
#include <LiquidCrystal_I2C.h>

#define BACKLIGHT_PIN     3

LiquidCrystal_I2C

//lcd(0x27);  // Set the LCD I2C address

// 2 = En
// 1 = Rw
// 0 = Rs
// 4 = d0
// 5 = d1
// 6 = d2
// 7 = d3
lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, BACKLIGHT_PIN, POSITIVE);


void lcd_setup()
{
  TinyWireM.begin();

  lcd.begin(16,2);               // initialize the lcd

  lcd.home ();                   // go home
  lcd.print ("Built: " __TIME__);
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print ("mV: ");
}


void lcd_showvbat()
{
  lcd.setCursor ( 4, 1 );
  lcd.print (((uint32_t)vbat) * MAX_MILLIVOLT / ADC_RESOLUTION);
  lcd.print("    ");

}

void lcd_off()
{
  lcd.off();
}


void lcd_on()
{
  lcd.on();
}


void lcd_setBacklight(bool on)
{
  if(on)
    lcd.backlight();
  else
    lcd.noBacklight();
}
#endif
