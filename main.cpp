#include <arduino.h>
#include <WiFi.h>
#include "main.hpp"
#include "config.hpp"

Main::Main(MQTTClientCallbackSimple mqttCallback) {
  Serial.begin(115200);
  Serial.println(F("Starting"));
  delay(10);

  pinMode(LED_BLUE_GPIO, OUTPUT);
  pinMode(LED_RED_GPIO, OUTPUT);
  pinMode(LED_GREEN_GPIO, OUTPUT);
  pinMode(MOTOR_GPIO, OUTPUT);
  pinMode(BUTTON_GPIO, INPUT_PULLUP);
  pinMode(PROBE_GPIO, INPUT_PULLUP);
  digitalWrite(LED_RED_GPIO, HIGH);
  digitalWrite(MOTOR_GPIO, LOW);

  this->duration = 0;
  this->remaning = 0;
  this->lastMillis = millis();

  this->wifiClient = new WiFiClient();
  this->mqttClient = new MQTTClient();
  
  this->mqttClient->begin(MQTT_SERVER, MQTT_PORT, *(this->wifiClient));
  this->mqttClient->onMessage(mqttCallback);
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
    this->mqttClient->subscribe(MQTT_TOPIC_DURATION, 1);
    this->mqttClient->subscribe(MQTT_TOPIC_TRIGGER, 1);
    return true;
  } else {
    Serial.println(F("MQTT Unable to connect to the server !"));
    return false;
  }
}

void    Main::loop() {
  if (!(this->connectWifi() && this->connectMQTT())) {
    digitalWrite(LED_GREEN_GPIO, LOW);
    digitalWrite(LED_RED_GPIO, HIGH);
    this->turnOff();
    delay(30000);
    return;
  }

  digitalWrite(LED_RED_GPIO, LOW);
  digitalWrite(LED_GREEN_GPIO, HIGH);

  unsigned int currentMillis = millis();
  unsigned int elapsedTime = currentMillis - this->lastMillis;

  this->mqttClient->loop();

  if (this->remaning > 0) {
    Serial.println(this->remaning);
    this->remaning -= elapsedTime;
    if (this->remaning <= 0) {
      this->turnOff();
    }
  }

  this->lastMillis = currentMillis;
  delay(250);
}

void    Main::messageReceived(String &topic, String &payload) {
  Serial.print(topic);
  Serial.print(" : ");
  Serial.println(payload);
  if (topic.equals(MQTT_TOPIC_DURATION)) {
    this->duration = payload.toInt();
    Serial.print(F("Set duration to "));
    Serial.println(this->duration);
  } else if (topic.equals(MQTT_TOPIC_TRIGGER)) {
    if (payload.equals(F("True"))) {
      this->remaning = 1000 * this->duration;
      this->turnOn();
    } else if (this->remaning != 0) {
      this->turnOff();
    }
  }
}

void    Main::buttonInterrupt() {
  delay(300);
}

void    Main::turnOn() {
  // Serial.println("TURN ON !");-
  digitalWrite(LED_BLUE_GPIO, HIGH);
  digitalWrite(MOTOR_GPIO, HIGH);
}

void    Main::turnOff() {
  Serial.println("TURN OFF !");
  digitalWrite(LED_BLUE_GPIO, LOW);
  digitalWrite(MOTOR_GPIO, LOW);
  this->remaning = 0;
  this->mqttClient->publish(MQTT_TOPIC_TRIGGER, "False", true, 1);
}
