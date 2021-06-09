#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include "Arduino.h"
#include <AsyncMqttClient.h>

class MotionSensor
{
  public:
    MotionSensor(byte pin, AsyncMqttClient* pClient, String clientId, String sensorId);
    void init();
    void check();

  protected:
    byte m_DataPin;
    boolean m_movement;
    boolean m_movementOld;
    AsyncMqttClient* m_pClient;
    String m_clientId;
    String m_sensorId;
};

#endif