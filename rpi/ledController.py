#!/usr/bin/python

import paho.mqtt.client as mqtt
import subprocess
import json
import time, threading

MQTT_SERVER = "localhost"
MQTT_PORT   = 1883
MQTT_TOPIC_MOTION = "motion"
MQTT_TOPIC_EFFEKT = "effekt"

# led Effect for the led stripes
DEFAULT_EFFECT = 13
EFFECT_TIME = 10 # in seconds
NUM_OF_LED_STRIPES = 7
actualEffect = DEFAULT_EFFECT

client = mqtt.Client()

def setup():
 client.on_connect = on_connect
 client.on_message = on_message
 client.on_publish = on_publish
 client.on_log = on_log
 client.on_disconnect = on_disconnect
 client.connect(MQTT_SERVER, MQTT_PORT, 60)

def on_log(client, userdata, level, buf):
 print("log: "+buf)

def on_connect(client, userdata, flags, rc):
 print("Connected to "+MQTT_SERVER+":"+str(MQTT_PORT)+" with result code "+str(rc))
 client.subscribe(MQTT_TOPIC_MOTION)
 client.subscribe(MQTT_TOPIC_EFFEKT)

def on_disconnect(client, userdata, rc):
 print("client disconnected ok")

def on_message(client, userdata, msg):
 # message received
 print(msg.topic+" "+str(msg.payload))
 if msg.topic == MQTT_TOPIC_MOTION:
   # turn leds on
   setLedEffectToAllStripes(actualEffect)
   # start timer to turn leds off again
   threading.Timer(EFFECT_TIME, timeUp).start()
 elif msg.topic == MQTT_TOPIC_EFFEKT:
   # turn leds on
   setLedEffectToAllStripes(msg.payload)

def on_publish(client, userdata, result):
 print("data published \n")

def timeUp():
  turnOffAllStripes()

# led functions
def setLedEffectToAllStripes(effect):
  for x in range(1, (NUM_OF_LED_STRIPES+1)):
    msgTopic = "ledStreifen/"+str(x)
    msgPayload = str(effect)
    # print("topic: "+msgTopic+ " payload:"+msgPayload)
    client.publish(msgTopic, msgPayload)

def turnOffAllStripes():
  setLedEffectToAllStripes(0)

def changeEffect(newEffect):
  actualEffect = newEffect


if __name__ == '__main__':

 # setup
 setup()

 # loop
 try:
  while True:
   client.loop(0.1)
 # Stop on Ctrl+C and clean up
 except KeyboardInterrupt:
  client.disconnect()
  print("script terminated by KeyboardInterrupt")
