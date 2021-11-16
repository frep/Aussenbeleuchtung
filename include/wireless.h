#ifndef WIRELESS_H
#define WIRELESS_H

#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include <peripherals.h>

// =======================================================================
// MQTT message format:
// =======================================================================
// topic:   <MQTT_Topic>/<ClientId>  e.g.    ledStreifen/1
// message: <ledEffect>              e.g.    12
// -----------------------------------------------------------------------
// topic:   <MQTT_Topic>             e.g.    motion
// message: <ClientId>               e.g.    1
// -----------------------------------------------------------------------
// topic:   <MQTT_Topic>             e.g.    effekt
// message: <ClientId>               e.g.    13
// -----------------------------------------------------------------------

void TaskWifiCode(void * pvParameters);

void initWireless();
void connectToWifi();
void connectToMqtt();
void startWebserver();
void handleWebserver();
void WiFiEvent(WiFiEvent_t event);
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);
String processor(const String& var);

#endif