#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <Arduino.h>
#include <wireless.h>
#include <peripherals.h>
#include <Debug.h>
#include "SPIFFS.h"
#include "Config.h"

// pointer to configuration 
Config* pConfig;                        

TaskHandle_t Task1;

void Task1Code(void * pvParameters)
{
    DEBUG_T("Task1 running on core: "); DEBUG_P(xPortGetCoreID());
    for(;;) // run forever
    {
      handleWebserver();
      loopPeripherals();
    }
}

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

  // setup peripherals (ledStripe and motion sensors)
  setupPeripherals();

  // initialize wireless functions
  initWireless();

  // start "loop" task on core 1 with lowest priority 
  xTaskCreatePinnedToCore(
    Task1Code,          // Task function
    "Task1",            // name of task
    10000,              // Stack size of task
    NULL,               // parameter of the task
    0,                  // priority of the task
    &Task1,             // Task handle to keep track of created task
    1);                 // pin task to core 1
}

void loop() 
{
  // nothing to do here. 
  // Task1 runs the periodic "loop" task with low priority
}
