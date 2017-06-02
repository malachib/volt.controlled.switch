#include "features.h"
#include "pins.h"
#include "appConsole.h"

using namespace util;

#ifdef DEBUG_SERIAL
#include "SoftwareSerial.h"

SoftwareSerial Serial(PIN_RX, PIN_TX);

namespace FactUtilEmbedded { namespace std {
    
ostream cout(Serial);
ostream& clog = cout;
    
}}

void appConsole_setup()
{
    pinMode(PIN_RX, INPUT);
    pinMode(PIN_TX, OUTPUT);

    Serial.begin(9600);
}
#else
void appConsole_setup()
{
}
#endif


#ifdef CONSOLE_ACTIVE
Menu menu;
ConsoleMenu console(&menu);
#endif
