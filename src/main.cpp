#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <Arduino.h>
#include <LedStripe.h>
#include <wireless.h>
#include <peripherals.h>
#include <Debug.h>
#include <MotionSensor.h>
#include "SPIFFS.h"
#include "Config.h"

// pointer to configuration 
Config* pConfig;                        
// LED-Streifen
LedStripe* pLedStripe;
// MotionSensor
MotionSensor* pMotionSensor1;
MotionSensor* pMotionSensor2;
MotionSensor* pMotionSensor3;

extern AsyncMqttClient mqttClient;

void setup() 
{
  DEBUG_B(115200);
  DEBUG_P();
  DEBUG_P();

  // Initialize SPIFFS
  if(!SPIFFS.begin(true))
  {
    DEBUG_P("An Error has occurred while mounting SPIFFS");
    return;
  }
  // setup configuration
  pConfig = new Config("/config.json");
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

  // initialize wireless functions
  initWireless();
}

void loop() 
{
  pLedStripe->loop();
  handleWebserver();
  pMotionSensor1->check();
  pMotionSensor2->check();
  pMotionSensor3->check();
}
