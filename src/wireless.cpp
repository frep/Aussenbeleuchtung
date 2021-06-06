#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> 
#include <AsyncElegantOTA.h>
#include <ESPmDNS.h>
#include <wireless.h>
#include <Debug.h>
#include <MqttCredentials.h>
#include <peripherals.h>
#include "SPIFFS.h"

extern AsyncWebServer server;
extern bool bWebserverStarted;
extern AsyncWiFiManager wifiManager;
extern AsyncMqttClient mqttClient;
extern TimerHandle_t mqttReconnectTimer;
extern TimerHandle_t wifiReconnectTimer;
extern String processor(const String& var);
extern byte clientId;
extern uint16_t numLeds;

const char* PARAM_ClientId = "inputClientId";
const char* PARAM_NumLeds = "inputNumLeds";

//extern bool bPendingAliveRequest;
//extern uint nUnansweredAliveRequests;
//extern RpiState state;


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



void connectToWifi() 
{
  DEBUG_P("Connecting to Wi-Fi...");
  wifiManager.autoConnect("AutoConnectAP");
}

void connectToMqtt() 
{
  DEBUG_P("Connecting to MQTT...");
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
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
    clientId = inputMessageClientId.toInt();
    numLeds = inputMessageNumLeds.toInt();
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