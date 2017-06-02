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

using namespace FactUtilEmbedded::std;

void enableOutputPins()
{
  pinMode(PIN_SWITCH, OUTPUT);
#ifdef REGULATOR_CONTROL
  pinMode(PIN_REGULATOR, OUTPUT);
#endif

#ifdef LED_ACTIVE
  pinMode(PIN_LED, OUTPUT);
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
}

const uint16_t wdto_ms = 500;
// compiler for some reason doesn't optimize this line out
//const uint8_t wdto = WatchdogControl::getWDTOfromMS<wdto_ms>();

void appConsole_setup();

void setup()
{
  enableOutputPins();
  lcd_setup();
  debugBlink();

  // For debug only, give us time to connect serial debugger
#ifdef DEBUG_SERIAL
  //delay(5000);
#endif

  // NOTE: We need ISR on, otherwise we don't wake up from sleep
  // systemReset defaults to OFF
  uint8_t wdto = Watchdog::WDTO::fromMS<wdto_ms>();
  Watchdog::isr.on();
  Watchdog::enable(wdto);
  //Watchdog.prescalar.cache(wdto);
  //Watchdog.enable();

  // prep analog input to see what kind of voltage values are preset
  pinMode(ANALOG_IN_VBAT, INPUT);
#ifdef REGULATOR_CONTROL
  pinMode(ANALOG_IN_CAP, INPUT);
#endif

    appConsole_setup();
}

uint16_t vbat;

void dozeStateHandler()
{
#ifdef LED_ACTIVE
  // wait briefly just so LED is visible
  while(millis() < (ledOnSince + 20)) delay(1);

  ledOff();
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


void printCurrentState()
{
  cout << F("state = ") << state;
  cout.println();
  cout << F("vbat = ") << vbat << F(" thresh = ") << DIVIDED_THRESHOLD_VOLTAGE;
  cout.println();
}

void loop()
{
  Watchdog::reset();

  vbat = analogRead(ANALOG_IN_VBAT);

  ledHandler();

#ifdef DEBUG_SERIAL
  const uint16_t timeout = 2000;

  static_assert(timeout % wdto_ms == 0, "timeout and wdto_ms must be evenly divisible");

  // on 2 second boundaries,
  // this means we haven't been sleeping, which means the built-in delay
  // of being asleep doesn't exist.  So use millis()
  if(state == Awake)
  {
    static uint32_t m = 0;
    if(millis() > m)
    {
      printCurrentState();
      m += timeout;
    }
  }
  // all other states we assume built-in sleep delay is active, so
  // use a simple counter instead of a timer
  else
  {
    static uint8_t m = 0;
    if((m++ % (timeout / wdto_ms)) == 0) printCurrentState();
  }
#endif

  lcd_showvbat();

  if(vbat < DIVIDED_THRESHOLD_VOLTAGE)
    belowThresholdStateHandler();
  else
    aboveThresholdStateHandler();
}
