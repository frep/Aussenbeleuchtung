#include <Arduino.h>
#include <LedStripe.h>
#include <EEPROM.h>

#include <AsyncMqttClient.h>
#include <wireless.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>      // https://github.com/tzapu/WiFiManager
#include <peripherals.h>
#include <Debug.h>

#define NUM_LEDS 276 
#define PIN 14
#define DEFAULT_EFFECT 12 // rainbow

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

void setup() 
{
  DEBUG_B(115200);
  DEBUG_P();
  DEBUG_P();

  // initialize digital pin ledPin as an output.
  pinMode(PIN_LED, OUTPUT);
  // setup ledStripe:
  EEPROM.get(0,storedLedEffect);
  pLedStripe = new LedStripe(NUM_LEDS, PIN, storedLedEffect);
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