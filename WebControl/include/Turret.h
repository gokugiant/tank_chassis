#pragma once
#include <Arduino.h>

class Turret {
public:
  static constexpr int MinAngle = 0, MaxAngle = 180, CenterAngle = 90;
  bool begin();
  bool setAngle(int degrees);
  bool center() {
    return setAngle(CenterAngle);
  }
  void stop();
  void update();
  // Sollwinkel, keine Positionsmessung.
  int angle() const {
    return angle_;
  }
  bool active() const {
    return active_;
  }

private:
  int angle_ = CenterAngle;
  bool ready_ = false, active_ = false;
  uint32_t lastCommand_ = 0;
};
