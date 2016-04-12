#include "features.h"
#include "pins.h"
#include "appConsole.h"

using namespace util;

#ifdef DEBUG_SERIAL
#include "SoftwareSerial.h"

SoftwareSerial cout(PIN_RX, PIN_TX);
#endif


#ifdef CONSOLE_ACTIVE
Menu menu;
ConsoleMenu console(&menu);
#endif
