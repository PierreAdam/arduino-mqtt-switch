#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include <MQTT.h>


class Main {
private:
  WiFiClient*   wifiClient;
  MQTTClient*   mqttClient;
  uint8_t       duration;
  int32_t       remaning;
  unsigned long lastMillis;

public:
                Main(MQTTClientCallbackSimple mqttCallback);
  bool          connectWifi();
  bool          connectMQTT();
  void          loop();
  void          messageReceived(String &topic, String &payload);
  void          buttonInterrupt();
  void          turnOn();
  void          turnOff();
};
