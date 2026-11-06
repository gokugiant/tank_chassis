#include "RgbLed.h"
#include "Pins.h"

void RgbLed::begin() {
  const uint8_t pins[] = {Pins::Red, Pins::Green, Pins::Blue};
  for (int i = 0; i < 3; i++) {
    digitalWrite(pins[i], LOW);
    pinMode(pins[i], OUTPUT);
    ledcSetup(i, 1000, 8);
    ledcWrite(i, 0);
    ledcAttachPin(pins[i], i);
  }
  off();
}

void RgbLed::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  red_ = red;
  green_ = green;
  blue_ = blue;
  ledcWrite(0, red);
  ledcWrite(1, green);
  ledcWrite(2, blue);
}
