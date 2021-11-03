#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <Arduino.h>
#include <wireless.h>
#include <peripherals.h>
#include <Debug.h>
#include "SPIFFS.h"
#include "Config.h"

// pointer to configuration 
Config* pConfig;                        

TaskHandle_t TaskWifi;

void TaskWifiCode(void * pvParameters)
{
  DEBUG_T("TaskWifi running on core: "); DEBUG_P(xPortGetCoreID());
  for(;;) // run forever
  {
    handleWebserver();
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

  // start "wifi loop" task on core 1 with priority 4 
  xTaskCreatePinnedToCore(
    TaskWifiCode,   // Task function
    "TaskWifi",     // name of task
    10000,          // Stack size of task
    NULL,           // parameter of the task
    4,              // priority of the task
    &TaskWifi,      // Task handle to keep track of created task
    1);             // pin task to core 1
}

void loop() 
{
  loopPeripherals();
}
