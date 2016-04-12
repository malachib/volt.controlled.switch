#include <Arduino.h>
//#include <Adafruit_SleepyDog.h>
#include <SystemStatus.h>
#include <fact/watchdog.h>


#include "features.h"
#include "states.h"
#include "main.h"
#include "lcd.h"
#include "led.h"
#include "pins.h"
#include <Console.h>

#include <avr/wdt.h>

void ledHandler();
void system_sleep();
void debugBlink();


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
  enableOutputPins();
  lcd_setup();
  debugBlink();

  // For debug only, give us time to connect serial debugger
#ifdef DEBUG_SERIAL
  //delay(5000);
#endif

  // NOTE: keep an eye on this brand new (but looking much better)
  // code.  This wakes us up from sleep
  Watchdog.enable(WDTO_500MS);

  // prep analog input to see what kind of voltage values are preset
  pinMode(ANALOG_IN_VBAT, INPUT);
#ifdef REGULATOR_CONTROL
  pinMode(ANALOG_IN_CAP, INPUT);
#endif

#ifdef DEBUG_SERIAL
  pinMode(PIN_RX, INPUT);
  pinMode(PIN_TX, OUTPUT);

  cout.begin(9600);
#endif
}

uint16_t vbat;

void dozeStateHandler()
{
#ifdef LED_ACTIVE
  // wait briefly just so LED is visible
  while(millis() < (ledOnSince + 20)) delay(1);

  ledOff();
#endif

  // clock doesn't run if we actually go to sleep, making our DEBUG_SERIAL wait
  // quite a long time indeed, so disable sleep during DEBUG_SERIAL
#ifdef DEBUG_SERIAL
    delay(500);
#else
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


void loop()
{
  Watchdog.reset();
  
  vbat = analogRead(ANALOG_IN_VBAT);

  ledHandler();

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

  lcd_showvbat();

  //capStateMachine.process();

  if(vbat < DIVIDED_THRESHOLD_VOLTAGE)
    belowThresholdStateHandler();
  else
    aboveThresholdStateHandler();
}
