#include <Arduino.h>
#include <Adafruit_SleepyDog.h>
#include <SystemStatus.h>


#include "features.h"
#include "states.h"
#include "main.h"
#include "lcd.h"
#include "pins.h"
#include <Console.h>

#include <avr/wdt.h>


#ifdef DEBUG_SERIAL
#include "SoftwareSerial.h"

#define PIN_RX PB0
#define PIN_TX PB1

SoftwareSerial Serial(PIN_RX, PIN_TX);
#endif

// attiny45 doing a similar job, but where's the source code?
// http://austindavid.com/jm3/index.php/hardware/57-lvd-project

// TODO: add trim pot to design for fine-tune adjustment to voltage sensitivity
// TODO: need simplistic Hysteresis so that we don't accidentally bounce
// it up and down a bunch (surface charge/sleep mode lack of draw may bring it back.. ?)
// TODO: have a capacitor onboard and power down voltage regulator since regulator quiescent
// cuurent gonna be 150ua all the time whether we draw power or not

// using code cues from https://arduinoelectronics.wordpress.com/2014/01/06/ultra-low-power-led-flasher-with-attiny/
// interesting info http://web.engr.oregonstate.edu/~traylor/ece473/lectures/reset.pdf

// reference voltage stuff may be more convenient than tying up an ADC pin:
// http://forum.arduino.cc/index.php?topic=223897.0
// http://forum.arduino.cc/index.php?topic=139958.0

// system status lib wraps up VRef usage.  Cool
// skipcount is number of sleep periods to skip before actually flashing the LED
#define LED_SKIPCOUNT 4

// doesn't use A0, A1, etc. like Uno and friends
// car battery reference voltage input
#define ANALOG_IN_VBAT PB3
// onboard capacitor reference voltage input
// FIX: can't work right now because collides with PIN_SWITCH
#define ANALOG_IN_CAP PB4

#ifdef LCD_ACTIVE
// where our status LED lives
#define PIN_LED PB1
#else
// where our status LED lives
#define PIN_LED PB2
#endif

// pin connected to regulator enable
#define PIN_REGULATOR PB0


void enableOutputPins()
{
  pinMode(PIN_SWITCH, OUTPUT);
#ifdef REGULATOR_CONTROL
  pinMode(PIN_REGULATOR, OUTPUT);
#endif

#ifdef LED_ACTIVE
  pinMode(PIN_LED, OUTPUT);
#endif

#ifdef DEBUG_SERIAL
  //pinMode(PIN_TX, OUTPUT);

  //Serial.begin(9600);
#endif
}


void disableOutputPins()
{
  pinMode(PIN_SWITCH, INPUT);
#ifdef REGULATOR_CONTROL
  pinMode(PIN_REGULATOR, INPUT);
#endif

#ifdef LED_ACTIVE
  pinMode(PIN_LED, INPUT);
#endif

#ifdef DEBUG_SERIAL
  //Serial.end();

  //pinMode(PIN_TX, INPUT);
#endif
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
    digitalWrite(PIN_LED,HIGH);
    delay(250);
    digitalWrite(PIN_LED,LOW);
    delay(250);
  }
}

void setup()
{
  enableOutputPins();
  lcd_setup();
  debugBlink();

  // For debug only, give us time to connect serial debugger
#ifdef DEBUG_SERIAL
  //delay(5000);
#endif

#ifdef UNUSED
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
  const uint8_t ii = 5; // wake up every 500ms to check for power state change
  uint8_t bb;

  //if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);

  Watchdog.setup(bb);
#endif
  // NOTE: keep an eye on this brand new (but looking much better)
  // code.
  Watchdog.setupPreset(WDTO_500MS);

  // prep analog input to see what kind of voltage values are preset
  pinMode(ANALOG_IN_VBAT, INPUT);
#ifdef REGULATOR_CONTROL
  pinMode(ANALOG_IN_CAP, INPUT);
#endif

#ifdef DEBUG_SERIAL
  pinMode(PIN_RX, INPUT);
  pinMode(PIN_TX, OUTPUT);

  Serial.begin(9600);
#endif
}

void system_sleep();

uint32_t ledOnSince;
uint16_t vbat;

inline void ledOn()
{
  digitalWrite(PIN_LED,HIGH);
  ledOnSince = millis();
}

inline void ledOff()
{
  digitalWrite(PIN_LED,LOW);
}

void dozeStateHandler()
{
  #ifdef LED_ACTIVE
    // wait briefly just so LED is visible
    while(millis() < (ledOnSince + 20))
      delay(1);
  #endif

  // clock doesn't run if we actually go to sleep, making our DEBUG_SERIAL wait
  // quite a long time indeed, so disable sleep during DEBUG_SERIAL
#ifdef DEBUG_SERIAL
    ledOff();
    delay(500);
#else
    disableOutputPins();
    // Q: is that going to be an issue with the switch itself?
    // will the switch be OK with a floating line? or might something
    // externally change its state causing an unwanted on or off?
    // can we
    // properly sleep if we are holding the line to a certain state?
    //pinMode(PIN_SWITCH, INPUT_PULLUP); // see how well this works , measure with voltmeter:
    //  1. sleep mode power usage
    //  2. whether input does stay in PULLUP mode while asleep
    system_sleep();
    // not sure why but debug serial doesn't recover well from this
    enableOutputPins();
#endif
}

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

void loop()
{
  vbat = analogRead(ANALOG_IN_VBAT);

  ledHandler();

#ifdef REGULATOR_CONTROL
  uint16_t vcap = analogRead(ANALOG_IN_CAP);
#endif

#ifdef DEBUG_SERIAL
  static uint32_t m;
  uint32_t _m = millis();

  // on 2 second boundaries,
  if(_m > m)
  {
    cout << F("state = ") << state;
    cout.println();
    cout << F("vbat = ") << vbat << F(" thresh = ") << DIVIDED_THRESHOLD_VOLTAGE;
    cout.println();
    m = _m + 2000;
  }
#endif

  lcd_showvbat();

  //capStateMachine.process();

  if(vbat < DIVIDED_THRESHOLD_VOLTAGE)
    belowThresholdStateHandler();
  else
    aboveThresholdStateHandler();
}
