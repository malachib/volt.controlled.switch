#include <Arduino.h>

#include "features.h"
#include "states.h"
#include "Console.h"
#include "main.h"
#include "lcd.h"
#include "pins.h"


State state;


// de-bounce code to avoid wiggly shutoffs
// not sure if we really need debouncing for our application, so leaving
// this here & incomplete since it won't consume too many cycles or code
uint32_t lastAwakeTime; // careful, millis() doesn't work when asleep
uint32_t uninterruptedDozeCycles = 0;

#define SETSTATE(s) { state = s; COUT_PRINT("State="); COUT_PRINTLN(#s); }
//#define SETSTATE(s) state = s

void powerOnTracker()
{
  digitalWrite(PIN_SWITCH, HIGH);
}


void powerOffTracker()
{
  digitalWrite(PIN_SWITCH, LOW);
}

void belowThresholdStateHandler()
{
  switch(state)
  {
    case Waking:
    case Awake:
      SETSTATE(EnteringDoze);
      break;

    case EnteringDoze:
      powerOffTracker();
      lcd_setBacklight(false);
      SETSTATE(Doze);
      break;

    case EnteringSleep:
      SETSTATE(Sleep);
      break;

    case Doze:
    case Sleep:
      if(uninterruptedDozeCycles++ == 40) // should be 20 seconds, but feeling more like 10
      {
        SETSTATE(EnteringSleep);
        return;
      }
      dozeStateHandler();
      break;
  }
}

void aboveThresholdStateHandler()
{
  uninterruptedDozeCycles = 0;

  switch(state)
  {
    case Doze:
      lcd_setBacklight(true);
      SETSTATE(Waking);
      break;

    case Sleep:
      //lcd_on();
      SETSTATE(Waking);
      break;

    case Waking:
      powerOnTracker();
      SETSTATE(Awake);
      break;

    case Awake:
      lastAwakeTime = millis();
      break;
  }
}

CapStateMachine capStateMachine;

void CapStateMachine::process(uint16_t vcap)
{
  auto state = getState();

  cout << F("TEST: ") << state;
}
