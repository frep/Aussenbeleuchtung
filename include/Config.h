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
    void setNumLeds(uint16_t value);
    void setClientId(int value);
    void setMqttHost(const char* host);
    void setMqttPort(int value);
    // getters
    String getNumLedsString();
    String getClientIdString();
    const char* getMqttHost();
    int getMqttPort();
    uint16_t getNumLeds();
    byte getLedEffect();

  protected:
    const char *filename;
    const char *mqtt_host;
    int mqtt_port;
    int clientId;
    uint16_t numLeds;
    byte storedLedEffect;
};

#endif