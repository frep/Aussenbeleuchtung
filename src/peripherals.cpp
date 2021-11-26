#include "peripherals.h"
#include <LedStripe.h>
#include <MotionSensor.h>
#include <Debug.h>
#include "Config.h"

TaskHandle_t TaskPeri;

// LED-Streifen
CRGB* pLeds;
LedStripe* pLedStripe;

// MotionSensor
MotionSensor* pMotionSensor1;
MotionSensor* pMotionSensor2;
MotionSensor* pMotionSensor3;

extern AsyncMqttClient mqttClient;
extern Config* pConfig; 

void startPeriTask()
{
    xTaskCreatePinnedToCore(
    TaskPeriCode,   // Task function
    "TaskPeri",     // name of task
    10000,          // Stack size of task
    NULL,           // parameter of the task
    1,              // priority of the task
    &TaskPeri,      // Task handle to keep track of created task
    1);             // pin task to core 1
}

void TaskPeriCode(void * pvParameters)
{
  DEBUG_T("TaskPeri running on core: "); DEBUG_P(xPortGetCoreID());

  // setup peripherals (ledStripe and motion sensors)
  setupPeripherals();

  for(;;) // run forever
  {
    loopPeripherals();
  }
}

void setupPeripherals()
{
    // initialize digital pin ledPin as an output.
  pinMode(PIN_ONBOARD_LED, OUTPUT);

  // setup ledStripe:
  pLeds = new CRGB[pConfig->getNumLeds()];
  FastLED.addLeds<WS2811, uint8_t(PIN_LED_STRIPE), GRB>(pLeds, pConfig->getNumLeds()).setCorrection(TypicalLEDStrip);
  pLedStripe = new LedStripe(pConfig->getNumLeds(), PIN_LED_STRIPE, true, pLeds);
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