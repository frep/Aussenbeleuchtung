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
TaskHandle_t TaskPeri;

void setup() 
{
  DEBUG_B(115200);
  DEBUG_P();
  DEBUG_P();

  delay(1000);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true))
  {
    DEBUG_P("An Error has occurred while mounting SPIFFS");
    return;
  }

  // setup configuration
  pConfig = new Config("/config.json");

  // start "wifi loop" task on core 0 
  xTaskCreatePinnedToCore(
    TaskWifiCode,   // Task function
    "TaskWifi",     // name of task
    10000,          // Stack size of task
    NULL,           // parameter of the task
    1,              // priority of the task
    &TaskWifi,      // Task handle to keep track of created task
    0);             // pin task to core 0

  // start "peripheral loop" task on core 1 
  xTaskCreatePinnedToCore(
    TaskPeriCode,   // Task function
    "TaskPeri",     // name of task
    10000,          // Stack size of task
    NULL,           // parameter of the task
    1,              // priority of the task
    &TaskPeri,      // Task handle to keep track of created task
    1);             // pin task to core 1
}

void loop() 
{
}
