// voltage (millivolts)
#define THRESHOLD_VOLTAGE 13000
// dial in operating voltage exactly right , I plan to undervolt this sucker
//#define OPERATING_VOLTAGE 4.5

// what fixed dividers we've got out there to bring us within to operating
// voltage range (attiny I expect around 2.5V-5.5V)
#define VOLTAGE_DIVIDER 3.5

#define DIVIDED_THRESHOLD_VOLTAGE (THRESHOLD_VOLTAGE / VOLTAGE_DIVIDER)

// what minimum value to read out of ADC to continue with voltage regulator OFF
#define DIVIDED_THRESHOLD_CAP_VOLTAGE 512
