#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> 
#include <AsyncElegantOTA.h>
#include <ESPmDNS.h>
#include <wireless.h>
#include <Debug.h>
#include <peripherals.h>
#include "SPIFFS.h"
#include "Config.h"

extern AsyncWebServer server;
extern AsyncWiFiManager wifiManager;
extern AsyncMqttClient mqttClient;
extern TimerHandle_t mqttReconnectTimer;
extern TimerHandle_t wifiReconnectTimer;
extern Config* pConfig; 

const char* PARAM_MQTT_HOST = "inputMqttHost";
const char* PARAM_MQTT_PORT = "inputMqttPort";
const char* PARAM_ClientId = "inputClientId";
const char* PARAM_NumLeds = "inputNumLeds";
const char* PARAM_LED_Effect = "inputLedEffect";

bool bWebserverStarted;


/* MQTT client Unterscheidung:
   ===========================
/hello/clientid1/world  ok
/hello/clientid2/world  err
/hello/clientid3/world  warning

Then subscribe to the topic like this; 
/hello/+/world
Then in your code transform the topic name to get the message and client id.

The other approach I use is to use json in the payload, for example

/hello/world   {"msg":"err", "client":"clientid1"}
*/

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
    //String value = String(pConfig->getMqttHost());
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
  else if (var == "inputLedEffect")
  {
    String value = pConfig->getLedEffectString();
    DEBUG_P(value);
    return value;
  }

  return String();
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

  // ToDo: Stripe-Nummer konfigurierbar ueber ESPAsyncWiFiManager
  // see: https://github.com/alanswx/ESPAsyncWiFiManager/blob/master/examples/AutoConnectWithFSParameters/AutoConnectWithFSParameters.ino
  if(!MDNS.begin("stripe1"))    // http://stripe1.local
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
    digitalWrite(PIN_LED, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(PIN_LED, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String inputMessageClientId = request->getParam(PARAM_ClientId)->value();
    String inputMessageNumLeds = request->getParam(PARAM_NumLeds)->value();
    String inputMessageMqttHost = request->getParam(PARAM_MQTT_HOST)->value();
    String inputMessageMqttPort = request->getParam(PARAM_MQTT_PORT)->value();
    String inputMessageLedEffect = request->getParam(PARAM_LED_Effect)->value();

    // save value in config
    pConfig->setClientId(inputMessageClientId.toInt());
    pConfig->setNumLeds(inputMessageNumLeds.toInt());
    pConfig->setMqttHost(inputMessageMqttHost.c_str());
    pConfig->setMqttPort(inputMessageMqttPort.toInt());
    pConfig->setLedEffect(inputMessageLedEffect.toInt());
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
  DEBUG_T("Session present: ");
  DEBUG_P(sessionPresent);

  // Meldungen fuer den Esp32 abonnieren
  mqttClient.subscribe("ledSteifen", 0);
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
  DEBUG_T("  packetId: ");
  DEBUG_P(packetId);
  DEBUG_T("  qos: ");
  DEBUG_P(qos);
}

void onMqttUnsubscribe(uint16_t packetId) 
{
  DEBUG_P("Unsubscribe acknowledged.");
  DEBUG_T("  packetId: ");
  DEBUG_P(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) 
{
  DEBUG_P("Publish received.");
  DEBUG_T("  topic: ");
  DEBUG_P(topic);
  DEBUG_T("  qos: ");
  DEBUG_P(properties.qos);
  DEBUG_T("  dup: ");
  DEBUG_P(properties.dup);
  DEBUG_T("  retain: ");
  DEBUG_P(properties.retain);
  DEBUG_T("  len: ");
  DEBUG_P(len);
  DEBUG_T("  index: ");
  DEBUG_P(index);
  DEBUG_T("  total: ");
  DEBUG_P(total);

  if(!strcmp(topic, "ledSteifen"))
  {
    /*
      if(!strcmp(payload, "alive"))
      {
        DEBUG_P("radioPiEsp32: alive");
        if(state == rpiStartup)
        {
          // startup has finished
          state = rpiUp;
        }
        // a pending alive request was answered
        bPendingAliveRequest = false;
        nUnansweredAliveRequests = 0;
      }

      if(!strcmp(payload, "ledOff"))
      {
        DEBUG_P("radioPiEsp32: ledOff");
        pixels.ledOff();
      }
    */
  }

}

void onMqttPublish(uint16_t packetId) 
{
  DEBUG_P("Publish acknowledged.");
  DEBUG_T("  packetId: ");
  DEBUG_P(packetId);
}