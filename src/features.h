#pragma once

#include <fact/lib.h>

#define LED_ACTIVE
//#define LCD_ACTIVE
#define SLEEP_ENABLED
//#define CONSOLE_ACTIVE

// unfinished feature to power down/off regulator in pulses and
// draw power from a capacitor to save electricity
//#define REGULATOR_CONTROL

#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define COUT_PRINT(s) cout.print(F(s))
#define COUT_PRINTLN(s) cout.println(F(s))
#define COUT_PRINTLN_RAW(raw) cout.println(raw)
#else
#define COUT_PRINT(s) {}
#define COUT_PRINTLN(s) {}
#define COUT_PRINTLN_RAW(raw) {}}
#endif
