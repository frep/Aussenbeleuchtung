#ifndef INPUTS_H
#define INPUTS_H

/////////////////////////////////////////////////////////////////////////////////////
// Hardware: esp32doit-devkit-v1
/////////////////////////////////////////////////////////////////////////////////////

#define PIN_ONBOARD_LED     2
#define PIN_LED_STRIPE     14

#define PIN_SENSOR_1       36
#define PIN_SENSOR_2       39
#define PIN_SENSOR_3       34

/////////////////////////////////////////////////////////////////////////////////////

void setupPeripherals();
void loopPeripherals();

#endif