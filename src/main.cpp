#include <Arduino.h>
#include <LedStripe.h>
#include <EEPROM.h>

// ledPin refers to ESP32 GPIO 23
const int ledPin = 2;

#define NUM_LEDS 276 
#define PIN 14
#define DEFAULT_EFFECT 12 // rainbow

byte storedLedEffect;
LedStripe* pLedStripe;

void setup() 
{
  // initialize digital pin ledPin as an output.
  pinMode(ledPin, OUTPUT);
  // setup ledStripe:
  EEPROM.get(0,storedLedEffect);
  pLedStripe = new LedStripe(NUM_LEDS, PIN, storedLedEffect);
  pLedStripe->setup();
}

void loop() 
{
  pLedStripe->loop();
}