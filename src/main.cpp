#include <Arduino.h>
#include <Adafruit_SleepyDog.h>
#include <SystemStatus.h>

#include "features.h"
#include "states.h"
#include "main.h"
#include <Console.h>

#ifdef DEBUG_SERIAL
#include "SoftwareSerial.h"

#define PIN_RX PB0
#define PIN_TX PB1

SoftwareSerial Serial(PIN_RX, PIN_TX);
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
// skipcount is number of sleep periods to skip before actually flashing the LED
#define LED_SKIPCOUNT 4

// doesn't use A0, A1, etc. like Uno and friends
// car battery reference voltage input
#define ANALOG_IN_VBAT PB3
// onboard capacitor reference voltage input
// FIX: can't work right now because collides with PIN_SWITCH
#define ANALOG_IN_CAP PB4

// where our status LED lives
#define PIN_LED PB2
// pin connected to switch we take high or low depending on
// reference voltage threshold
#define PIN_SWITCH PB4

// pin connected to regulator enable
#define PIN_REGULATOR PB0


void enableOutputPins()
{
  pinMode(PIN_SWITCH, OUTPUT);
#ifdef REGULATOR_CONTROL
  pinMode(PIN_REGULATOR, OUTPUT);
#endif

#ifdef LED_ACTIVE
  pinMode(PIN_LED, OUTPUT);
#endif

#ifdef DEBUG_SERIAL
  //pinMode(PIN_TX, OUTPUT);

  //Serial.begin(9600);
#endif
}


void disableOutputPins()
{
  pinMode(PIN_SWITCH, INPUT);
#ifdef REGULATOR_CONTROL
  pinMode(PIN_REGULATOR, INPUT);
#endif

#ifdef LED_ACTIVE
  pinMode(PIN_LED, INPUT);
#endif

#ifdef DEBUG_SERIAL
  //Serial.end();

  //pinMode(PIN_TX, INPUT);
#endif
}

void setup()
{
  // For debug only, give us time to connect serial debugger
#ifdef DEBUG_SERIAL
  delay(5000);
#endif

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
#ifdef REGULATOR_CONTROL
  pinMode(ANALOG_IN_CAP, INPUT);
#endif

#ifdef DEBUG_SERIAL
  pinMode(PIN_RX, INPUT);
  pinMode(PIN_TX, OUTPUT);

  Serial.begin(9600);
#endif

  enableOutputPins();
}

void system_sleep();

uint32_t ledOnSince;

void dozeStateHandler()
{
  #ifdef LED_ACTIVE
    // wait briefly just so LED is visible
    while(millis() < (ledOnSince + 30))
      delay(1);
  #endif

  // clock doesn't run if we actually go to sleep, making our DEBUG_SERIAL wait
  // quite a long time indeed, so disable sleep during DEBUG_SERIAL
#ifndef DEBUG_SERIAL
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
    // not sure why but debug serial doesn't recover well from this
    enableOutputPins();
#endif
}

inline void ledOn()
{
  digitalWrite(PIN_LED,HIGH);
  ledOnSince = millis();
}

void ledHandler()
{
  static uint32_t m;
  //uint32_t _m = millis();
#ifdef LED_ACTIVE
  static uint8_t skip = LED_SKIPCOUNT;
  if(state == Doze)
  {
    // only allow LED to stay on when skip gets to 0
    // will get shut right off again when we re-enter sleep
    if(--skip == 0)
    {
      ledOn();
      skip = LED_SKIPCOUNT;
    }
  }
  else if(state == Waking)
  {
    // LED stays on solid when awake
    ledOn();
  }
  else if(state == EnteringDoze)
  {
    // do fast blink here
    //if(millis() < ledOnSince)
  }

#endif
}

void loop()
{
  ledHandler();

  uint16_t vbat = analogRead(ANALOG_IN_VBAT);
#ifdef REGULATOR_CONTROL
  uint16_t vcap = analogRead(ANALOG_IN_CAP);
#endif

#ifdef DEBUG_SERIAL
  static uint32_t m;
  uint32_t _m = millis();

  // on 2 second boundaries,
  if(_m > m)
  {
    cout << F("state = ") << state;
    cout.println();
    cout << F("vbat = ") << vbat << F(" thresh = ") << DIVIDED_THRESHOLD_VOLTAGE;
    cout.println();
    m = _m + 2000;
  }
#endif


  //capStateMachine.process();

  if(vbat < DIVIDED_THRESHOLD_VOLTAGE)
    belowThresholdStateHandler();
  else
    aboveThresholdStateHandler(vbat);
}
