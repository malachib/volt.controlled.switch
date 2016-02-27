#include <Arduino.h>
#include <Adafruit_SleepyDog.h>
#include <SystemStatus.h>

#include "states.h"

#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#include "SoftwareSerial.h"

// TODO: move these to 0 and 1 since 3 and 4 tie up ADC channels
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

// using code cues from https://arduinoelectronics.wordpress.com/2014/01/06/ultra-low-power-led-flasher-with-attiny/
// interesting info http://web.engr.oregonstate.edu/~traylor/ece473/lectures/reset.pdf

// reference voltage stuff may be more convenient than tying up an ADC pin:
// http://forum.arduino.cc/index.php?topic=223897.0
// http://forum.arduino.cc/index.php?topic=139958.0

// system status lib wraps up VRef usage.  Cool
#define LED_ACTIVE
// skipcount is number of sleep periods to skip before actually flashing the LED
#define LED_SKIPCOUNT 4

// doesn't use A0, A1, etc. like Uno and friends
// car battery reference voltage input
#define ANALOG_IN_VBAT 1
// onboard capacitor reference voltage input
#define ANALOG_IN_CAP 2

// where our status LED lives
#define PIN_LED 4
// pin connected to switch we take high or low depending on
// reference voltage threshold
#define PIN_SWITCH 0

// pin connected to regulator enable
#define PIN_REGULATOR 1


// voltage (millivolts)
#define THRESHOLD_VOLTAGE 13000
// dial in operating voltage exactly right , I plan to undervolt this sucker
//#define OPERATING_VOLTAGE 4.5

// what fixed dividers we've got out there to bring us within to operating
// voltage range (attiny I expect around 2.5V-5.5V)
#define VOLTAGE_DIVIDER 3.5

#define DIVIDED_THRESHOLD_VOLTAGE (THRESHOLD_VOLTAGE / VOLTAGE_DIVIDER)

// what minimum value to read out of ADC to continue with voltage regulator OFF
#define DIVIDED_THRESHOLD_CAP_VOLTAGE 512

void enableOutputPins()
{
  pinMode(PIN_SWITCH, OUTPUT);
  pinMode(PIN_REGULATOR, OUTPUT);

#ifdef LED_ACTIVE
  pinMode(PIN_LED, OUTPUT);
#endif

#ifdef DEBUG_SERIAL
  pinMode(PIN_TX, OUTPUT);
#endif
}


void disableOutputPins()
{
  pinMode(PIN_SWITCH, INPUT);
  pinMode(PIN_REGULATOR, INPUT);

#ifdef LED_ACTIVE
  pinMode(PIN_LED, INPUT);
#endif

#ifdef DEBUG_SERIAL
  pinMode(PIN_TX, INPUT);
#endif
}

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
  pinMode(ANALOG_IN_VBAT, INPUT);
  pinMode(ANALOG_IN_CAP, INPUT);

#ifdef DEBUG_SERIAL
  pinMode(PIN_RX, INPUT);
  pinMode(PIN_TX, OUTPUT);

  Serial.begin(9600);
#endif

  enableOutputPins();
}

void system_sleep();

void dozeStateHandler()
{
  #ifdef LED_ACTIVE
    static uint8_t skip = LED_SKIPCOUNT;
    if(--skip == 0)
    {
      digitalWrite(PIN_LED,HIGH);  // let led blink
  #ifndef DEBUG_SERIAL
      delay(30);
  #else
      COUT_PRINTLN("pulse");
  #endif
      skip = LED_SKIPCOUNT;
      digitalWrite(PIN_LED,LOW);
    }

  #endif
    disableOutputPins();
    // Q: is that going to be an issue with the switch itself?
    // will the switch be OK with a floating line? or might something
    // externally change its state causing an unwanted on or off?
    // can we
    // properly sleep if we are holding the line to a certain state?
    //pinMode(PIN_SWITCH, INPUT_PULLUP); // see how well this works , measure with voltmeter:
    //  1. sleep mode power usage
    //  2. whether input does stay in PULLUP mode while asleep
    system_sleep();
    enableOutputPins();
}

void loop()
{
  uint16_t vbat = analogRead(ANALOG_IN_VBAT);

  if(vbat < DIVIDED_THRESHOLD_VOLTAGE)
    belowThresholdStateHandler();
  else
    aboveThresholdStateHandler();
}
