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

void ledFlash(uint16_t d)
{
  digitalWrite(PIN_LED,HIGH);
  delay(d);
  digitalWrite(PIN_LED,LOW);
  delay(d);
}

// lets us know we're starting up properly
void debugBlink()
{
  for(int i = 0; i < 3; i++)
  {
    digitalWrite(PIN_LED,HIGH);
    delay(500);
    digitalWrite(PIN_LED,LOW);
    delay(250);
    ledFlash(250);
  }
}


void ledHandler()
{
  //static uint32_t m;

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

      // we won't blink for 4x the periods if voltage is
      // below threshold
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
      ledFlash(100);
      ledFlash(100);
    }
    // this is supposed to flash LED a special way when we get close
    // to our threshold.  The trouble is, the delay incurred here makes
    // us unacceptably slow to respond to a voltage loss scenario,
    // so temporarily disabling.  The way to make this work is to use
    // the old millis() > non blocking version
    else if(vbat < (DIVIDED_THRESHOLD_VOLTAGE + DIVIDED_NEARBY))
    {
      // NOTE: This might get reset by WDT, although my WDT code implicity
      // shuts off reset feature; so keep an eye on whether we reset during
      // awake-mode low voltage condition
      //ledFlash(500);
    }
  }

#endif
}
