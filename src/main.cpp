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
}

void loop() 
{
  handleWebserver();
  loopPeripherals();
}
