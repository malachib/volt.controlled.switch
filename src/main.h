#define MAX_VOLTAGE 18000

// voltage (millivolts)
#define THRESHOLD_VOLTAGE 13
// dial in operating voltage exactly right , I plan to undervolt this sucker
//#define OPERATING_VOLTAGE 4.5

// adapted from http://www.ohmslawcalculator.com/voltage-divider-calculator
// produces divided voltage from Vs input given two resisters in a voltage
// divider circuit
#define VOLTAGE_DIVIDER(R1, R2) (R2 / (R1 + R2))
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

#define ADC_VOLTAGE(V) (VOLTAGE_DIVIDE(V, RESISTOR1, RESISTOR2) * ADC_RESOLUTION)

//#define DIVIDED_THRESHOLD_VOLTAGE (THRESHOLD_VOLTAGE * ADC_RESOLUTION / VOLTAGE_DIVIDER)
#define DIVIDED_THRESHOLD_VOLTAGE ADC_VOLTAGE(THRESHOLD_VOLTAGE)

// what minimum value to read out of ADC to continue with voltage regulator OFF
#define DIVIDED_THRESHOLD_CAP_VOLTAGE 512
