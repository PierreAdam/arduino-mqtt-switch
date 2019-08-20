#include <arduino.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "main.hpp"
#include "config.hpp"

const char* wifiSSID = "Livebox-4F53";
const char* wifiPassword = "10820CF7C6A9103099ECE641E2";

Main::Main() {
  Serial.begin(115200);
  Serial.println(F("Starting"));
  //Serial.println(xPortGetCoreID());
  delay(10);


  this->client = new WiFiClient();
  this->mqtt = new Adafruit_MQTT_Client(this->client, MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
  this->mqttAlarmSubscriber = new Adafruit_MQTT_Subscribe(this->mqtt, MQTT_TOPIC, 2);

  this->mqtt->subscribe(this->mqttAlarmSubscriber);
}

// Function to connect and reconnect the Wifi.
bool    Main::connectWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  WiFi.disconnect();
  Serial.print(F("Connecting to "));
  Serial.println(wifiSSID);

  WiFi.begin(wifiSSID, wifiPassword);
  
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
  int8_t ret;

  // Stop if already connected.
  if (mqtt->connected()) {
    return true;
  }

  Serial.print(F("Connecting to MQTT... "));

  uint8_t retries = 3;
  while ((ret = this->mqtt->connect()) != 0) {
       Serial.println(this->mqtt->connectErrorString(ret));
       Serial.println(F("Retrying MQTT connection in 5 seconds..."));
       mqtt->disconnect();
       delay(5000);
       retries--;
       if (retries == 0) {
         WiFi.disconnect(); // Lost wifi ??
         return false;
       }
  }
  Serial.println(F("MQTT Connected!"));
  return true;
}

void    Main::loop() {
  if (!(this->connectWifi() && this->connectMQTT())) {
    delay(30000);
    return;
  }

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = this->mqtt->readSubscription(5000))) {
    if (subscription == this->mqttAlarmSubscriber) {
      Serial.print(F("Data received : "));
      Serial.println((char *)subscription->lastread);
    }
  }

  if(!this->mqtt->ping(5)) {
    this->mqtt->disconnect();
  }
}

void    Main::buttonInterrupt() {
  delay(300);
}
