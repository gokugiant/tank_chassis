#include "Button.h"
#include "Pins.h"

void Button::begin() {
  pinMode(Pins::Button, INPUT);
  raw_ = stable_ = digitalRead(Pins::Button);
  changed_ = millis();
}

bool Button::rawPressed() const {
  return digitalRead(Pins::Button) == HIGH;
}

void Button::update() {
  bool value = rawPressed();
  if (value != raw_) {
    raw_ = value;
    changed_ = millis();
  }
  if (millis() - changed_ < 40)
    return;
  if (!raw_)
    ready_ = true;
  if (stable_ != raw_) {
    stable_ = raw_;
    if (stable_ && ready_) {
      ready_ = false;
      event_ = true;
      count_++;
    }
  }
}

bool Button::wasPressed() {
  bool result = event_;
  event_ = false;
  return result;
}
