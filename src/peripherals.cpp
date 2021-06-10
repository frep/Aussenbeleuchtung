#include "peripherals.h"
#include <LedStripe.h>
#include <MotionSensor.h>
#include "Config.h"

// LED-Streifen
LedStripe* pLedStripe;
// MotionSensor
MotionSensor* pMotionSensor1;
MotionSensor* pMotionSensor2;
MotionSensor* pMotionSensor3;

extern AsyncMqttClient mqttClient;
extern Config* pConfig; 

void setupPeripherals()
{
    // initialize digital pin ledPin as an output.
  pinMode(PIN_ONBOARD_LED, OUTPUT);

  // setup ledStripe:
  pLedStripe = new LedStripe(pConfig->getNumLeds(), PIN_LED_STRIPE);
  pLedStripe->setup();

  // setup motion sensors
  pMotionSensor1 = new MotionSensor(PIN_SENSOR_1, &mqttClient, pConfig->getClientIdString(), "1");
  pMotionSensor2 = new MotionSensor(PIN_SENSOR_2, &mqttClient, pConfig->getClientIdString(), "2");
  pMotionSensor3 = new MotionSensor(PIN_SENSOR_3, &mqttClient, pConfig->getClientIdString(), "3");
  pMotionSensor1->init();
  pMotionSensor2->init();
  pMotionSensor3->init();
}

void loopPeripherals()
{
  pLedStripe->loop();
  pMotionSensor1->check();
  pMotionSensor2->check();
  pMotionSensor3->check();
}