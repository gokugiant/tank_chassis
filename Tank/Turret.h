#ifndef Turret_h
#define Turret_h
#include <Arduino.h>
#include <ESP32Servo.h>

class Turret {
public:
  Turret(int pin);
  void center();
  void turnToDegree(int direction);
  void turnLeft();
  void turnRight();
private:
	int _pin;
  Servo _turret;

  int _calibrateMin = 0;
  int _calibrateMax = 147;

};

#endif