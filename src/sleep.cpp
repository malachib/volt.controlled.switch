#include <fact/power.h>
#include <fact/sleep.h>
#include <fact/lib.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

using namespace util;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif



// set system into the sleep state
// system wakes up when wtchdog is timed out
void system_sleep()
{
#ifdef OLD
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
#else
  Power.adc.off();
  Sleep.powerDown();
  Power.adc.on();


#endif
}
