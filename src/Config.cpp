#include <Config.h>
#include <Debug.h>

Config::Config(const char *file)
{
    filename = file;
    // init members with default values
    mqtt_host = "192.168.0.266";
    mqtt_port = 1883;
    clientId = 1;
    numLeds = 276;
    storedLedEffect = 12;
    // try to load values from file
    loadConfigFromFile();
}

bool Config::loadConfigFromFile()
{
  DEBUG_P("mounting FS...");

  if (SPIFFS.begin()) 
  {
    DEBUG_P("mounted file system");
    if (SPIFFS.exists(filename)) 
    {
      //file exists, reading and loading
      DEBUG_P("reading config file");
      File configFile = SPIFFS.open(filename, "r");
      if (configFile) 
      {
        DEBUG_P("opened config file");

        // write data from file to JsonDocument
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, configFile);
        configFile.close();
        // copy data from JsonDocument to config
        clientId = doc["clientId"];
        mqtt_host = doc["mqtt_host"];
        mqtt_port = doc["mqtt_port"];
        numLeds = doc["numLeds"];
        storedLedEffect = doc["storedLedEffect"];
        return true;
      }
    }
  } 
  else 
  {
    DEBUG_P("failed to mount FS");
  }
  // error occured!
  return false;
}

bool Config::saveConfigToFile()
{
    // store config values in Json document
    DynamicJsonDocument doc(2048);
    doc["clientId"] = clientId;
    doc["mqtt_host"] = mqtt_host;
    doc["mqtt_port"] = mqtt_port;
    doc["numLeds"] = numLeds;
    doc["storedLedEffect"] = storedLedEffect;

    // write Json document to file
  DEBUG_P("mounting FS...");
  if (SPIFFS.begin()) 
  {
    DEBUG_P("mounted file system");
    if (SPIFFS.exists(filename)) 
    {
      //file exists, reading and loading
      DEBUG_P("reading config file");
      File configFile = SPIFFS.open(filename, "w");
      if (configFile) 
      {
        serializeJson(doc, configFile);
        configFile.close();
        return true;
      }
    }
  } 
  else 
  {
    DEBUG_P("failed to mount FS");
  }
  // error occured!
  return false;
}

void Config::setNumLeds(uint16_t value)
{
    numLeds = value;
}

void Config::setClientId(int value)
{
    clientId = value;
}

void Config::setMqttHost(const char* host)
{
    mqtt_host = host;
}

void Config::setMqttPort(int value)
{
    mqtt_port = value;
}

String Config::getNumLedsString()
{
    return String(numLeds);
}

String Config::getClientIdString()
{
    return String(clientId);
}

const char* Config::getMqttHost()
{
    return mqtt_host;
}

int Config::getMqttPort()
{
    return mqtt_port;
}

uint16_t Config::getNumLeds()
{
    return numLeds;
}

byte Config::getLedEffect()
{
    return storedLedEffect;
}