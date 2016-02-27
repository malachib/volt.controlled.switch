#include <Arduino.h>
#include <Adafruit_SleepyDog.h>
#include <SystemStatus.h>

#include "states.h"

#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#include "SoftwareSerial.h"

#define PIN_RX 3
#define PIN_TX 4

SoftwareSerial Serial(PIN_RX, PIN_TX);

#define COUT_PRINTLN(s) Serial.println(F(s))
#else
#define COUT_PRINTLN(s) {}
#endif

// attiny45 doing a similar job, but where's the source code?
// http://austindavid.com/jm3/index.php/hardware/57-lvd-project

// TODO: need simplistic Hysteresis so that we don't accidentally bounce
// it up and down a bunch (surface charge/sleep mode lack of draw may bring it back.. ?)
// TODO: have a capacitor onboard and power down voltage regulator since regulator quiescent
// cuurent gonna be 150ua all the time whether we draw power or not

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

#ifdef DEBUG_SERIAL
  pinMode(PIN_RX, INPUT);
  pinMode(PIN_TX, OUTPUT);

  Serial.begin(9600);
#endif
}

void system_sleep();

void dozeStateHandler()
{
  #ifdef LED_ACTIVE
    static uint8_t skip = LED_SKIPCOUNT;
    if(--skip == 0)
    {
      COUT_PRINTLN("pulse");
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

void loop()
{
  uint16_t vbat = analogRead(ANALOG_IN);

  if(vbat < DIVIDED_THRESHOLD_VOLTAGE)
    belowThresholdStateHandler();
  else
    aboveThresholdStateHandler();
}
