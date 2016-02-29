#define LED_ACTIVE

#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define COUT_PRINT(s) Serial.print(F(s))
#define COUT_PRINTLN(s) Serial.println(F(s))
#define COUT_PRINTLN_RAW(raw) Serial.println(raw)
#else
#define COUT_PRINT(s) {}
#define COUT_PRINTLN(s) {}
#define COUT_PRINTLN_RAW(raw) {}}
#endif
