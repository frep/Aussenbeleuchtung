#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <Arduino.h>
#include <wireless.h>
#include <peripherals.h>
#include <Debug.h>
#include "SPIFFS.h"
#include "Config.h"

// pointer to configuration 
Config* pConfig;                        

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

  // disable watchdog-timer core 0
  disableCore0WDT();

  // setup configuration
  pConfig = new Config("/config.json");

  startWifiTask();
  // peripheral task starts after sucessful connection to a wifi.
}

void loop() 
{
}
