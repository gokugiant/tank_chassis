#include <Arduino.h>
#include "Move.h"

Move::Move(int leftEnable, int rightEnable, int input1, int input2, int input3, int input4 ) {
	_leftEnable = leftEnable;
	_rightEnable = rightEnable;
  _input1 = input1;
  _input2 = input2;
  _input3 = input3;
  _input4 = input4;

  pinMode(_input1, OUTPUT);
  pinMode(_input2, OUTPUT);
  pinMode(_input3, OUTPUT);
  pinMode(_input4, OUTPUT);

  _pwmMin = 0;
  _pwmMax = 255;  // Set the maximal pwm value
}

// Turn the object 90 degree to the left
void Move::turn90Left(){
  leftMotorForward();  
  rightMotorBackward();

  //TODO check exaxt 90degree movement

  allDisable();
}

// Turn the object 90 degree to the right
void Move::turn90Right(){
  rightMotorForward();
  leftMotorBackward();
  
  //TODO check exaxt 90degree movement

  allDisable();
}

void Move::turnLeftForMillis(){

}

void Move::turnRightForMillis(){

}

void Move::forward(){

}

void Move::backward(){

}

void Move::stop(){
  allDisable();
}

void Move::allDisable(){
  leftMotorDisable();
  rightMotorDisable();
}

void Move::leftMotorForward(){
  digitalWrite(_input1, false);
  digitalWrite(_input2, true);
}

void Move::leftMotorBackward(){
  digitalWrite(_input1, true);
  digitalWrite(_input2, false);
}

void Move::leftMotorDisable(){
  digitalWrite(_input1, false);
  digitalWrite(_input2, false);
}

void Move::rightMotorForward(){
  digitalWrite(_input3, false);
  digitalWrite(_input4, true);
}

void Move::rightMotorBackward(){
  digitalWrite(_input3, true);
  digitalWrite(_input4, false);
}

void Move::rightMotorDisable(){
  digitalWrite(_input3, false);
  digitalWrite(_input4, false);
}