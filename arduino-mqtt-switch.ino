#include "main.hpp"
#include "config.hpp"


Main* main;

void setup() {
  main = new Main(&mqttCallback);
  attachInterrupt(digitalPinToInterrupt(BUTTON_GPIO), buttonInterrupt, RISING);
}

void loop() {
  main->loop();
}

void buttonInterrupt() {
  main->buttonInterrupt();
}

void mqttCallback(String &topic, String &payload) {
  main->messageReceived(topic, payload);
}
