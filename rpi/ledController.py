#!/usr/bin/python

import paho.mqtt.client as mqtt
import subprocess
import json

MQTT_SERVER = "localhost"
MQTT_PORT   = 1883
MQTT_TOPIC  = "motion"

client = mqtt.Client()

def setup():
 client.on_connect = on_connect
 #client.on_message = on_first_message
 client.on_message = on_message
 client.on_publish = on_publish
 #client.on_log = on_log
 client.on_disconnect = on_disconnect
 client.connect(MQTT_SERVER, MQTT_PORT, 60)

def on_log(client, userdata, level, buf):
 print("log: "+buf)

def on_connect(client, userdata, flags, rc):
 print("Connected to "+MQTT_SERVER+":"+str(MQTT_PORT)+" with result code "+str(rc))
 client.subscribe(MQTT_TOPIC)

def on_disconnect(client, userdata, rc):
 print("client disconnected ok")

def on_message(client, userdata, msg):
 print(msg.topic+" "+str(msg.payload))
 client.publish("ledStreifen", "12")

def on_publish(client, userdata, result):
 print("data published \n")

# after the first message is received, change the callback function
#def on_first_message(client, userdata, msg):
# client.on_message = on_message

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
