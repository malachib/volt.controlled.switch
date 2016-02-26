#include <Arduino.h>
#include <Adafruit_SleepyDog.h>
#include <SystemStatus.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

// using code cues from https://arduinoelectronics.wordpress.com/2014/01/06/ultra-low-power-led-flasher-with-attiny/
// interesting info http://web.engr.oregonstate.edu/~traylor/ece473/lectures/reset.pdf

// reference voltage stuff may be more convenient than tying up an ADC pin:
// http://forum.arduino.cc/index.php?topic=223897.0
// http://forum.arduino.cc/index.php?topic=139958.0

// system status lib wraps up VRef usage.  Cool
#define LED_ACTIVE
#define PIN_LED 4
// skipcount is number of sleep periods to skip before actually flashing the LED
#define LED_SKIPCOUNT 4

// doesn't use A0, A1, etc. like Uno and friends
#define ANALOG_IN 1

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


// voltage (millivolts)
#define THRESHOLD_VOLTAGE 13000
// dial in operating voltage exactly right , I plan to undervolt this sucker
//#define OPERATING_VOLTAGE 4.5

// what fixed dividers we've got out there to bring us within to operating
// voltage range (attiny I expect around 2.5V-5.5V)
#define VOLTAGE_DIVIDER 3.5

#define DIVIDED_THRESHOLD_VOLTAGE (THRESHOLD_VOLTAGE / VOLTAGE_DIVIDER)


#define ADC_INPUT_THRESHOLD

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

  // prep analog input to see what kind of voltage values are preset
  pinMode(ANALOG_IN, INPUT);

#ifdef LED_ACTIVE
  pinMode(PIN_LED, OUTPUT);
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
#ifdef LED_ACTIVE
  static uint8_t skip = LED_SKIPCOUNT;
  if(skip-- == 0)
  {
    digitalWrite(PIN_LED,HIGH);  // let led blink
    delay(30);
    skip = LED_SKIPCOUNT;
    digitalWrite(PIN_LED,LOW);
  }

  pinMode(PIN_LED,INPUT); // set all used port to intput to save power
#endif
  system_sleep();
#ifdef LED_ACTIVE
  pinMode(PIN_LED,OUTPUT); // resume
#endif
}
