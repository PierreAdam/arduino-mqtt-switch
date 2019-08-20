#include "main.hpp"
#include "config.hpp"


Main* main;

void setup() {
  main = new Main();
  attachInterrupt(digitalPinToInterrupt(BUTTON_GPIO), buttonInterrupt, RISING);
}

void loop() {
  main->loop();
}

void buttonInterrupt() {
  main->buttonInterrupt();
}
