#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


class Main {
private:
  WiFiClient* client;
  Adafruit_MQTT_Client* mqtt;
  Adafruit_MQTT_Subscribe* mqttAlarmSubscriber;

public:
  Main();
  void loop();
  void buttonInterrupt();
  bool connectWifi();
  bool connectMQTT();
};
