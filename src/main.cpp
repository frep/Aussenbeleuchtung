#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <Arduino.h>
#include <LedStripe.h>
#include <EEPROM.h>

#include <AsyncMqttClient.h>
#include <wireless.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>      // https://github.com/tzapu/WiFiManager
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
byte storedLedEffect;
LedStripe* pLedStripe;
// Async MQTT
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
// WifiManager
AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);

//bool bPendingAliveRequest;
//uint nUnansweredAliveRequests;
bool bWebserverStarted;

String ledState;
// Replaces placeholder with LED state value
String processor(const String& var)
{
  DEBUG_P(var);
  if(var == "STATE")
  {
    if(digitalRead(PIN_LED))
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    DEBUG_P(ledState);
    return ledState;
  }
  else if (var == "inputClientId")
  {
    return pConfig->getClientIdString();
  }
  else if (var == "inputNumLeds")
  {
    return pConfig->getNumLedsString();
  }

  return String();
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

  pConfig = new Config("/config.json");

  // initialize digital pin ledPin as an output.
  pinMode(PIN_LED, OUTPUT);
  // setup ledStripe:
  //EEPROM.get(0,storedLedEffect);
  //pLedStripe = new LedStripe(NUM_LEDS, PIN, storedLedEffect);
  pLedStripe = new LedStripe(pConfig->getNumLeds(), PIN, pConfig->getLedEffect());
  pLedStripe->setup();

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  connectToWifi();
}

void loop() 
{
  pLedStripe->loop();
  handleWebserver();
}