#include <Arduino.h>
#include "Turret.h"

Turret::Turret(int pin) {
  _pin = pin;
  _turret.attach(_pin); // attaches the servo on pin 18 to the servo object
}

// Turn the turret to the center
void Turret::center(){
  turnToDegree(90); // Set turrent to center
}

// Turn the turret fully left
void Turret::turnLeft(){
  turnToDegree(0);
}

// Turn the turret fully right
void Turret::turnRight(){
  turnToDegree(180);
}

void Turret::turnToDegree(int direction){
  // Correct the direction of the turrent to a calibrated value that was tested before for the servo
  int calibratedDirection = map(direction, 0, 180, _calibrateMin, _calibrateMax);
  _turret.write(calibratedDirection);
}