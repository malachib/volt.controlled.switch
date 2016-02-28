#include <Arduino.h>

#include "features.h"
#include "states.h"
#include "Console.h"

State state;


// de-bounce code to avoid wiggly shutoffs
// not sure if we really need debouncing for our application, so leaving
// this here & incomplete since it won't consume too many cycles or code
uint32_t lastAwakeTime;

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
      SETSTATE(Doze);
      break;

    case Doze:
      dozeStateHandler();
      break;
  }
}

void aboveThresholdStateHandler(uint16_t vbat)
{
  switch(state)
  {
    case Doze:
      SETSTATE(Waking);
      break;

    case Waking:
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
