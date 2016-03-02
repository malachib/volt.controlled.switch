#pragma once

// represents what the very top end of our ADC is expected to read
#define MAX_MILLIVOLT 20150
//#define MAX_VOLTAGE ((double)MAX_MILLIVOLT / 1000)
#define MAX_VOLTAGE 20.15

//#define VCC_VOLTAGE 5
// running off the 3xAA battery pack.  remember to change this back!
// looks like Attiny is boosting it up to 5v again
#define VCC_MILLIVOLT 5000
#define VCC_VOLTAGE 5.0
#define VCC_VOLTAGE_STR "5.0"
//#define STR_NUMERIC(n) (## n)

// voltage above which we go active
#define THRESHOLD_VOLTAGE 13

// going this low, we blink LED more slowly
#define THRESHOLD_LOWVOLTAGE 9

// readings float too high by about this much
#define THRESHOLD_ERR 0.2

// this is the "wiggle" area, flash LED gently to indicate we're near
// our threshold
#define VOLTAGE_NEARBY 0.1

// adapted from http://www.ohmslawcalculator.com/voltage-divider-calculator
// produces divided voltage from Vs input given two resisters in a voltage
// divider circuit
#define VOLTAGE_DIVIDER(R1, R2) ((double)R2 / (R1 + R2))
//#define VOLTAGE_DIVIDE(Vs, R1, R2)  (((double)Vs * R2) / (R1 + R2))
// following just spits out a 0.  As if integer math was somehow getting involved
#define VOLTAGE_DIVIDE(Vs, R1, R2)  (Vs * VOLTAGE_DIVIDER(R1, R2))

// this combo:
//   18V = 5V
//   12V = 3.3V (convenient coincidence)
//   9V = 2.5V
//#define RESISTOR1 3900
//#define RESISTOR2 1500

// 15.7V = 5.006V
// 13V = 4.145V
// 9V = 2.87V
//#define RESISTOR1 47000
//#define RESISTOR2 22000

// 20.15 = 5V
// 13V = 3.226V
// 9V = 2.233V
#define RESISTOR1 1000000
#define RESISTOR2 330000

// 10-bit adc resolution
#define ADC_RESOLUTION 1024
#define ADC_MAX (ADC_RESOLUTION - 1)

// what fixed dividers we've got out there to bring us within to operating
// voltage range (attiny I expect around 2.5V-5.5V)
//#define VOLTAGE_DIVIDER 3.5

// divide down voltage, push through ratio of our own max Vcc voltage, then
// adjust for ADC sampling rate
#define ADC_VOLTAGE(V) (uint16_t)(VOLTAGE_DIVIDE(V, RESISTOR1, RESISTOR2) * ADC_RESOLUTION / VCC_VOLTAGE)

#define DIVIDED_THRESHOLD_VOLTAGE ADC_VOLTAGE((THRESHOLD_VOLTAGE + THRESHOLD_ERR))

#define DIVIDED_THRESHOLD_LOWVOLTAGE ADC_VOLTAGE(THRESHOLD_LOWVOLTAGE)

#define DIVIDED_NEARBY ADC_VOLTAGE(VOLTAGE_NEARBY)

// what minimum value to read out of ADC to continue with voltage regulator OFF
#define DIVIDED_THRESHOLD_CAP_VOLTAGE 512

extern uint16_t vbat;
