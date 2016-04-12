#include "features.h"
#include "pins.h"
#include "led.h"
#include "states.h"
#include "main.h"

// system status lib wraps up VRef usage.  Cool
// skipcount is number of sleep periods to skip before actually flashing the LED
#define LED_SKIPCOUNT 4

uint32_t ledOnSince;

extern uint16_t vbat;

void ledHandler()
{
  static uint32_t m;
  //uint32_t _m = millis();
#ifdef LED_ACTIVE
  static uint8_t skip = LED_SKIPCOUNT;
  if(state == Doze || state == Sleep)
  {
    // only allow LED to stay on when skip gets to 0
    // will get shut right off again when we re-enter sleep
    if(--skip == 0)
    {
      ledOn();
      skip = LED_SKIPCOUNT;

      if(vbat < DIVIDED_THRESHOLD_LOWVOLTAGE)
        skip *= 4;
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
  else if(state == Awake)
  {
    if(vbat == MAX_VOLTAGE)
    {
      // alert we have a high voltage situation
      delay(100);
      ledOff();
      delay(100);
      ledOn();
      delay(100);
      ledOff();
      delay(100);
      ledOn();
    }
    else if(vbat < (DIVIDED_THRESHOLD_VOLTAGE + DIVIDED_NEARBY))
    {
      delay(500);
      ledOff();
      delay(500);
      ledOn();
    }
  }

#endif
}
