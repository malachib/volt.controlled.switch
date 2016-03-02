#include <Arduino.h>

#include "features.h"
#include "states.h"
#include "Console.h"
#include "lcd.h"


State state;


// de-bounce code to avoid wiggly shutoffs
// not sure if we really need debouncing for our application, so leaving
// this here & incomplete since it won't consume too many cycles or code
uint32_t lastAwakeTime; // careful, millis() doesn't work when asleep
uint32_t uninterruptedDozeCycles = 0;

#define SETSTATE(s) { state = s; COUT_PRINT("State="); COUT_PRINTLN(#s); }
//#define SETSTATE(s) state = s

void belowThresholdStateHandler()
{
  switch(state)
  {
    case Waking:
    case Awake:
      SETSTATE(EnteringDoze);
      break;

    case EnteringDoze:
      lcd_setBacklight(false);
      SETSTATE(Doze);
      break;

    case EnteringSleep:
      lcd_off();
      SETSTATE(Sleep);
      break;

    case Doze:
    case Sleep:
      if(uninterruptedDozeCycles++ == 20)
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
  switch(state)
  {
    case Doze:
    case Sleep:
      SETSTATE(Waking);
      break;

    case Waking:
      lcd_setBacklight(true);
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
