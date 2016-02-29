// represents what the very top end of our ADC is expected to read
#define MAX_VOLTAGE 18

#define VCC_VOLTAGE 5

// voltage above which we go active
#define THRESHOLD_VOLTAGE 13

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
#define RESISTOR1 3900
#define RESISTOR2 1500

// 10-bit adc resolution
#define ADC_RESOLUTION 1024

// what fixed dividers we've got out there to bring us within to operating
// voltage range (attiny I expect around 2.5V-5.5V)
//#define VOLTAGE_DIVIDER 3.5

// divide down voltage, push through ratio of our own max Vcc voltage, then
// adjust for ADC sampling rate
#define ADC_VOLTAGE(V) (uint16_t)(VOLTAGE_DIVIDE(V, RESISTOR1, RESISTOR2) * ADC_RESOLUTION / VCC_VOLTAGE)

#define DIVIDED_THRESHOLD_VOLTAGE ADC_VOLTAGE(THRESHOLD_VOLTAGE)

// what minimum value to read out of ADC to continue with voltage regulator OFF
#define DIVIDED_THRESHOLD_CAP_VOLTAGE 512
