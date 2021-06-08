#ifndef CONFIG_h
#define CONFIG_h

#include <Arduino.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>

class Config
{
  public:
    Config(const char *file);
    bool loadConfigFromFile();
    bool saveConfigToFile();
    // setters
    void setNumLeds(int value);
    void setClientId(int value);
    void setMqttHost(const char* host);
    void setMqttPort(int value);
    void setLedEffect(byte value);
    // getters
    String getNumLedsString();
    String getClientIdString();
    String getMqttPortString();
    String getLedEffectString();
    String getMqttHost();
    String getMqttTopic();
    int getMqttPort();
    uint16_t getNumLeds();
    byte getLedEffect();

  protected:
    const char *filename;
    String mqtt_host;
    String mqtt_topic;
    int mqtt_port;
    int clientId;
    int numLeds;
    byte storedLedEffect;
};

#endif