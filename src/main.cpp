#include <Arduino.h>

// ledPin refers to ESP32 GPIO 23
const int ledPin = 2;

void setup() 
{
  // initialize digital pin ledPin as an output.
  pinMode(ledPin, OUTPUT);
}

void loop() 
{
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                  // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                  // wait for a second
}