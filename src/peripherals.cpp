#include <peripherals.h>
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <Adafruit_NeoPixel.h>
#include <Debug.h>

extern AsyncMqttClient mqttClient;
//extern RpiState state;
//extern bool bPendingAliveRequest;
//extern uint nUnansweredAliveRequests;

/////////////////////////////////////////////////////////////////////////////////////
// Raspberry Pi control functions
/////////////////////////////////////////////////////////////////////////////////////

/* unsigned long lastAliveRequest;
unsigned long startShutdown;

void startup()
{
    DEBUG_P("startup");
    // To start the raspberry pi, the power has to be turned on
    digitalWrite(PIN_POWER, HIGH);
    digitalWrite(PIN_LED, HIGH);

    lastAliveRequest = millis();
    state = rpiStartup;
}

void shutdown()
{
    DEBUG_P("shutdown");
    // send a shutdown request to the raspberry pi
    mqttClient.publish("volumio", 0, false, "shutdown");
    
    startShutdown = millis();
    state = rpiShutdown;
}

void isRpiAlive()
{
    // check periodically, if rpi is alive
    unsigned long currentMillis = millis();
    if((currentMillis - lastAliveRequest) > isAliveInterval)
    {
        // check if a pending alive request kept unanswered
        if(bPendingAliveRequest)
        {
            nUnansweredAliveRequests++;
            if(nUnansweredAliveRequests > 500)
            {
                shutdown();
            }
        }
        // send a new alive request
        mqttClient.publish("volumio", 0, false, "isRpiAlive");
        // Only if RPi state is rpiUp, an answer is expected
        bPendingAliveRequest = (state == rpiUp) ? true : false;

        lastAliveRequest = currentMillis;
    }
}

void isRpiDown()
{
    // check if raspberry pi had enough time to shutdown gracefully
    unsigned long currentMillis = millis();
    if((currentMillis - startShutdown) > shutdownTime)
    {
        // Power off the raspberry pi
        digitalWrite(PIN_POWER, LOW);
        digitalWrite(PIN_LED, LOW);

        state = rpiDown;
    }
}

void handleState(RpiState actualState)
{
    switch(actualState)
    {
        case rpiDown:
        default:
            // nothing to do
        break;
        case rpiStartup:
        case rpiUp:
            // send periodically isAlive requests
            isRpiAlive();
        break;
        case rpiShutdown:
            isRpiDown();
        break;
    }
}

String getRpiState(RpiState state)
{
  switch (state)
  {
  case rpiDown:
    return "RPi down";
    break;

  case rpiStartup:
    return "RPi starting up";
    break;

  case rpiUp:
    return "RPi up";
    break;
  
  case rpiShutdown:
    return "RPi shutdown";
    break;

  default:
    return "UNKNOWN";
    break;
  }
  return "UNKNOWN";
}

String getRelayState()
{
  bool powerOn = digitalRead(PIN_POWER);
  if(powerOn)
  {
    return "ON";
  }

  return "OFF";
} */
