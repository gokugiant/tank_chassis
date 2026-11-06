#pragma once
#include <Arduino.h>

class RgbLed {
public:
  void begin();
  void setColor(uint8_t red, uint8_t green, uint8_t blue);

  void red() {
    setColor(255, 0, 0);
  }

  void green() {
    setColor(0, 255, 0);
  }

  void blue() {
    setColor(0, 0, 255);
  }

  void off() {
    setColor(0, 0, 0);
  }

  uint8_t redValue() const {
    return red_;
  }

  uint8_t greenValue() const {
    return green_;
  }

  uint8_t blueValue() const {
    return blue_;
  }

private:
  uint8_t red_ = 0, green_ = 0, blue_ = 0;
};
