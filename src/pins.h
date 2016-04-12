// pin connected to switch we take high or low depending on
// reference voltage (ANALOG_IN_VBAT) threshold
#define PIN_SWITCH PB4

// doesn't use A0, A1, etc. like Uno and friends
// car battery reference voltage input
#define ANALOG_IN_VBAT PB3

// onboard capacitor reference voltage input
// FIX: can't work right now because collides with PIN_SWITCH
// I think I was gonna use CAP as kind of a battery backup , use it plus
// a super small VREG to power the attiny; however I'm just gonna use
// a regular step down switching and call it a day
#define ANALOG_IN_CAP PB4

#ifdef LCD_ACTIVE
// where our status LED lives
#define PIN_LED PB1
#else
// where our status LED lives
#define PIN_LED PB2
#endif

// pin connected to regulator enable; this would be used with ANALOG_IN_CAP
// to bounce regulator on and off.  Not needed if driving ATtiny direct from
// a smart buck converter
#define PIN_REGULATOR PB0

#define PIN_RX PB0
#define PIN_TX PB1
