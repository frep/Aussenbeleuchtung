#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>      // https://github.com/tzapu/WiFiManager
#include <AsyncElegantOTA.h>
#include <ESPmDNS.h>
#include <wireless.h>
#include <Debug.h>
#include <peripherals.h>
#include "SPIFFS.h"
#include "Config.h"
#include "LedStripe.h"

TaskHandle_t TaskWifi;

// Async MQTT
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
// WifiManager
AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);

extern Config* pConfig;
extern LedStripe* pLedStripe;

const char* PARAM_MQTT_HOST = "inputMqttHost";
const char* PARAM_MQTT_PORT = "inputMqttPort";
const char* PARAM_ClientId  = "inputClientId";
const char* PARAM_NumLeds   = "inputNumLeds";

bool bWebserverStarted;

void startWifiTask()
{
    xTaskCreatePinnedToCore(
    TaskWifiCode,   // Task function
    "TaskWifi",     // name of task
    10000,          // Stack size of task
    NULL,           // parameter of the task
    1,              // priority of the task
    &TaskWifi,      // Task handle to keep track of created task
    0);             // pin task to core 0
}

void TaskWifiCode(void * pvParameters)
{
  DEBUG_T("TaskWifi running on core: "); DEBUG_P(xPortGetCoreID());

  // initialize wireless functions
  initWireless();

  for(;;) // run forever
  {
    handleWebserver();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Replaces placeholder with LED state value
String processor(const String& var)
{
  DEBUG_T(var);
  DEBUG_T(": ");
  if (var == "inputClientId")
  {
    String value = pConfig->getClientIdString();
    DEBUG_P(value);
    return value;
  }
  else if (var == "inputNumLeds")
  {
    String value = pConfig->getNumLedsString();
    DEBUG_P(value);
    return value;
  }
  else if (var == "inputMqttHost")
  {
    String value = pConfig->getMqttHost();
    DEBUG_P(value);
    return value;
  }
  else if (var == "inputMqttPort")
  {
    String value = pConfig->getMqttPortString();
    DEBUG_P(value);
    return value;
  }

  return String();
}

void initWireless()
{
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

void connectToWifi() 
{
  DEBUG_P("Connecting to Wi-Fi...");
  wifiManager.autoConnect("AutoConnectAP");
}

void connectToMqtt() 
{
  DEBUG_P("Connecting to MQTT...");
  mqttClient.setServer(pConfig->getMqttHost().c_str(), pConfig->getMqttPort());
  mqttClient.connect();
}

void startWebserver()
{
  DEBUG_P("start webserver");

  String name = String("stripe") + pConfig->getClientIdString();
  
  if(!MDNS.begin(name.c_str()))    // e.g. http://stripe1.local
  {
    DEBUG_P("Error starting mDNS");
    return;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(PIN_ONBOARD_LED, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(PIN_ONBOARD_LED, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String inputMessageClientId = request->getParam(PARAM_ClientId)->value();
    String inputMessageNumLeds  = request->getParam(PARAM_NumLeds)->value();
    String inputMessageMqttHost = request->getParam(PARAM_MQTT_HOST)->value();
    String inputMessageMqttPort = request->getParam(PARAM_MQTT_PORT)->value();

    // save value in config
    pConfig->setClientId(inputMessageClientId.toInt());
    pConfig->setNumLeds(inputMessageNumLeds.toInt());
    pConfig->setMqttHost(inputMessageMqttHost.c_str());
    pConfig->setMqttPort(inputMessageMqttPort.toInt());
    pConfig->saveConfigToFile();
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  bWebserverStarted = true;
}

void handleWebserver()
{
  if(bWebserverStarted)
  {
    AsyncElegantOTA.loop();
  }
}

void WiFiEvent(WiFiEvent_t event) 
{
    DEBUG_F("[WiFi-event] event: %d\n", event);
    switch(event) 
    {
      case SYSTEM_EVENT_STA_GOT_IP:
        DEBUG_P("WiFi connected");
        DEBUG_P("IP address: ");
        DEBUG_P(WiFi.localIP());
        startPeriTask();
        startWebserver();
        connectToMqtt();
        break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
        DEBUG_P("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
		    xTimerStart(wifiReconnectTimer, 0);
        break;
      default:
        // nothing to do yet...
        break;
    }
}

void onMqttConnect(bool sessionPresent) 
{
  DEBUG_P("Connected to MQTT.");
  DEBUG_T("Session present: "); DEBUG_P(sessionPresent);

  // Meldungen fuer den Esp32 abonnieren
  mqttClient.subscribe(pConfig->getMqttTopic().c_str(), 0);
  DEBUG_T("subscribe to topic: "); DEBUG_P(pConfig->getMqttTopic());
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
  DEBUG_P("Disconnected from MQTT.");

  if (WiFi.isConnected()) 
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) 
{
  DEBUG_P("Subscribe acknowledged.");
  DEBUG_T("  packetId: "); DEBUG_P(packetId);
  DEBUG_T("  qos: "); DEBUG_P(qos);
}

void onMqttUnsubscribe(uint16_t packetId) 
{
  DEBUG_P("Unsubscribe acknowledged.");
  DEBUG_T("  packetId: "); DEBUG_P(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) 
{
  DEBUG_P("Publish received.");
  //DEBUG_T("  topic: "); DEBUG_P(topic);
  //DEBUG_T("  qos: "); DEBUG_P(properties.qos);
  //DEBUG_T("  dup: "); DEBUG_P(properties.dup);
  //DEBUG_T("  retain: "); DEBUG_P(properties.retain);
  //DEBUG_T("  len: "); DEBUG_P(len);
  //DEBUG_T("  index: "); DEBUG_P(index);
  //DEBUG_T("  total: "); DEBUG_P(total);
  
  if(!strcmp(topic, pConfig->getMqttTopic().c_str()))
  {
    // Read payload
    char payload_str[len];
    for (int i = 0; i < len; i++) 
    {
      payload_str[i] = payload[i];    
    }
    // decode payload (ledEffect)
    byte ledEffect = String(payload_str).toInt();
    DEBUG_T("topic: "); DEBUG_T(topic); DEBUG_T(" payload: "); DEBUG_P(ledEffect);
    // check if ledEffect is valid
    if(pLedStripe->isLedEffectValid(ledEffect))
    {
      pLedStripe->changeEffect(ledEffect);
    }
    else
    {
      DEBUG_T("Error! Invalid ledEffect: "); DEBUG_P(ledEffect);
    }
  }
}

void onMqttPublish(uint16_t packetId) 
{
  DEBUG_P("Publish acknowledged.");
  DEBUG_T("  packetId: "); DEBUG_P(packetId);
}