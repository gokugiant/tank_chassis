#pragma once
#include <TFT_eSPI.h>
#include "TankDrive.h"
#include "RgbLed.h"
#include "Button.h"
#include "Turret.h"
#include "MotionSensor.h"
#include "Heading.h"

class StatusDisplay {
public:
  void begin();
  void update(TankDrive& drive, const RgbLed& led, const Button& button, const Turret& turret,
              const MotionSensor& motion, const Heading& heading, const String& ip);

private:
  TFT_eSPI tft_;
  String last_;
  uint32_t lastDraw_ = 0;
};
