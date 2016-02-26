#include <Arduino.h>
#include <Adafruit_SleepyDog.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

// using code cues from https://arduinoelectronics.wordpress.com/2014/01/06/ultra-low-power-led-flasher-with-attiny/
// interesting info http://web.engr.oregonstate.edu/~traylor/ece473/lectures/reset.pdf

#define PIN_LED 4

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup()
{
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
  const uint8_t ii = 5; // wake up every 500ms to check for power state change
  uint8_t bb;

  //if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);

  Watchdog.setup(bb);

#ifdef DEBUG
  pinMode(PIN_LED, output);
#endif
}


// set system into the sleep state
// system wakes up when wtchdog is timed out
void system_sleep()
{
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

void loop()
{
#ifdef DEBUG
  digitalWrite(pinLed,HIGH);  // let led blink
  delay(30);
  digitalWrite(pinLed,LOW);

  pinMode(pinLed,INPUT); // set all used port to intput to save power
#endif
  system_sleep();
#ifdef DEBUG
  pinMode(pinLed,OUTPUT); // resume
#endif
}
