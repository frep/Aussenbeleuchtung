#include "MotionSensor.h"
#include "Debug.h"

MotionSensor::MotionSensor(byte pin, AsyncMqttClient* pClient, String clientId, String sensorId)
{
    m_DataPin = pin;
    m_movement = false;
    m_movementOld = false;
    m_pClient = pClient;
    m_clientId = clientId;
    m_sensorId = sensorId;
}

void MotionSensor::init()
{
    pinMode(m_DataPin, INPUT);
}

void MotionSensor::check()
{
    m_movement = digitalRead(m_DataPin);
    // a new movement was detected
    if(m_movement == true && m_movementOld == false)
    {
        m_movementOld = true;
        String message(m_clientId);
        message.concat("_");
        message.concat(m_sensorId);
        m_pClient->publish("motion", 0, true, message.c_str());
        DEBUG_P("Motion detected");
    }
    // no motion after motion is detected
    if(m_movement == false && m_movementOld == true)
    {
        m_movementOld = false;
        DEBUG_P("No movement");
    }
}