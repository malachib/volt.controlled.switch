#pragma once

#include "features.h"
#include "pins.h"


extern uint32_t ledOnSince;

inline void ledOn()
{
  digitalWrite(PIN_LED,HIGH);
  ledOnSince = millis();
}

inline void ledOff()
{
  digitalWrite(PIN_LED,LOW);
}
