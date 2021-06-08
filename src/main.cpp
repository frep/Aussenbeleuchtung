#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <Arduino.h>
#include <LedStripe.h>
#include <wireless.h>
#include <peripherals.h>
#include <Debug.h>

#include "SPIFFS.h"
#include "Config.h"

#define NUM_LEDS 276 
#define PIN 14
#define DEFAULT_EFFECT 12 // rainbow

// pointer to configuration 
Config* pConfig;                        

// LED-Streifen
LedStripe* pLedStripe;

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
  pinMode(PIN_LED, OUTPUT);
  // setup ledStripe:
  pLedStripe = new LedStripe(pConfig->getNumLeds(), PIN);
  pLedStripe->setup();
  // initialize wireless functions
  initWireless();
}

void loop() 
{
  pLedStripe->loop();
  handleWebserver();
}