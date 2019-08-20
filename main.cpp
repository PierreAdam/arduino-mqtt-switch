#include <arduino.h>
#include <WiFi.h>
#include "main.hpp"
#include "config.hpp"

Main::Main(MQTTClientCallbackSimple mqttCallback) {
  Serial.begin(115200);
  Serial.println(F("Starting"));
  //Serial.println(xPortGetCoreID());
  delay(10);

  this->connectWifi();

  this->wifiClient = new WiFiClient();
  this->mqttClient = new MQTTClient();
  
  this->mqttClient->begin(MQTT_SERVER, MQTT_PORT, *(this->wifiClient));
  this->mqttClient->onMessage(mqttCallback);
  this->connectMQTT();
}

// Function to connect and reconnect the Wifi.
// Should be called in the loop function and it will take care if connecting.
bool    Main::connectWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  WiFi.disconnect();
  Serial.print(F("Connecting to "));
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  uint8_t timeout = 20;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println(F("Unabled to connect to WiFi"));
    return false;
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
bool    Main::connectMQTT() {
  if (this->mqttClient->connected()) {
    return true;
  }

  this->mqttClient->disconnect();
  Serial.print(F("Connecting to MQTT server "));
  Serial.println(MQTT_SERVER);

  uint8_t timeout = 20;
  while (!this->mqttClient->connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD) && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }

  Serial.println("");

  if (this->mqttClient->connected()) {
    Serial.println(F("MQTT Connected !"));
    this->mqttClient->subscribe(MQTT_TOPIC, 1);
    return true;
  } else {
    Serial.println(F("MQTT Unable to connect to the server !"));
    return false;
  }
}

void    Main::loop() {
  if (!(this->connectWifi() && this->connectMQTT())) {
    delay(30000);
    return;
  }

  this->mqttClient->loop();
  
  delay(1000);
}

void    Main::messageReceived(String &topic, String &payload) {
  Serial.print(topic);
  Serial.print(" : ");
  Serial.println(payload);
}

void    Main::buttonInterrupt() {
  delay(300);
}
