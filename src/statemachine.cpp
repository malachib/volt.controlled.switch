#include <Arduino.h>

#include "states.h"

State state;


// de-bounce code to avoid wiggly shutoffs
// not sure if we really need debouncing for our application, so leaving
// this here & incomplete since it won't consume too many cycles or code
uint32_t lastAwakeTime;

void belowThresholdStateHandler()
{
  switch(state)
  {
    case Awake:
      state = EnteringDoze;
      break;

    case EnteringDoze:
      state = Doze;
      break;

    case Doze:
      dozeStateHandler();
      break;
  }
}

void aboveThresholdStateHandler()
{
  switch(state)
  {
    case Doze:
      state = Waking;
      break;

    case Waking:
      state = Awake;
      break;

    case Awake:
      lastAwakeTime = millis();
      break;
  }
}
