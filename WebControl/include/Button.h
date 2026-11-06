#pragma once
#include <Arduino.h>

class Button {
public:
  void begin();
  void update();     // Oft aus loop() aufrufen, kein delay() noetig.
  bool wasPressed(); // Einmaliges Ereignis statt Wiederholung beim Halten.

  bool isPressed() const {
    return stable_;
  }

  bool rawPressed() const;

  uint32_t pressCount() const {
    return count_;
  }

private:
  bool raw_ = false, stable_ = false, event_ = false, ready_ = false;
  uint32_t changed_ = 0, count_ = 0;
};
