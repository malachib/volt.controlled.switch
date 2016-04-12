#include <Arduino.h>

#include "features.h"

#ifdef LCD_ACTIVE
#include "main.h"
#include "lcd.h"

#include <TinyWireM.h>
#include <LiquidCrystal_I2C.h>

#include <SystemStatus.h>

SystemStatus SystemStatus;

#define BACKLIGHT_PIN     3

bool lcdOn = true;

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
  lcd.print ("Vcc " VCC_VOLTAGE_STR " Vbat");
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print ("ain:");
  lcd.setCursor ( 9, 1 );
  lcd.print ((uint32_t)(DIVIDED_THRESHOLD_VOLTAGE));
}


void lcd_showvbat()
{
  static uint32_t nextDraw = 0;

  uint32_t converted = vbat;
  uint32_t m = millis();

  //converted *= MAX_MILLIVOLT;
  //converted /= ADC_RESOLUTION;

  lcd.setCursor ( 12, 0 );
  lcd.print(SystemStatus.getVCC());
  lcd.setCursor ( 4, 1 );

  if(m > nextDraw)
  {
    if(converted < 10000) lcd.print(' ');
    if(converted < 1000) lcd.print(' ');
    if(converted < 100) lcd.print(' ');
    if(converted < 10) lcd.print(' ');

    lcd.print (converted);

    nextDraw += 250; // update 4x a second max to avoid flicker (left padding avoids some flicker already)
  }
}

void lcd_off()
{
  if(lcdOn)
  {
    lcdOn = false;
    lcd.off();
  }
}


void lcd_on()
{
  if(!lcdOn)
  {
    lcdOn = true;
    lcd.on();
  }
}


void lcd_setBacklight(bool on)
{
  if(on)
    lcd.backlight();
  else
    lcd.noBacklight();
}
#endif
