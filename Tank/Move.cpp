#include <Arduino.h>
#include "Move.h"

Move::Move(int leftEnable, int rightEnable, int input1, int input2, int input3, int input4 ) {
	_leftEnable = leftEnable;
	_rightEnable = rightEnable;
  _input1 = input1;
  _input2 = input2;
  _input3 = input3;
  _input4 = input4;


  pinMode(_leftEnable, OUTPUT);
  pinMode(_rightEnable, OUTPUT);
  pinMode(_input1, OUTPUT);
  pinMode(_input2, OUTPUT);
  pinMode(_input3, OUTPUT);
  pinMode(_input4, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannelLeft, freq, resolution);
  ledcSetup(pwmChannelRight, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  //ledcAttachPin(_leftEnable, pwmChannelLeft);
  //ledcAttachPin(_rightEnable, pwmChannelRight);
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

// Both motors go forward at full speed
void Move::forward(){
  forward(100);
}

void Move::forward(int percent){
  allDisable();
  leftMotorForward();
  rightMotorForward();

  int speedPercent = map(percent, _pwmMin, _pwmMax, 0, 100); //Translate from PWM min and max values to percent

  digitalWrite(_leftEnable , true);
  digitalWrite(_rightEnable , true);
  //ledcWrite(pwmChannelLeft, speedPercent); 
  //ledcWrite(pwmChannelRight, speedPercent);
}

// Both motors go forward at full speed
void Move::backward(){
  backward(100);
}

void Move::backward(int percent){
  allDisable();
  leftMotorBackward();
  rightMotorBackward();

  int speedPercent = map(percent, _pwmMin, _pwmMax, 0, 100); //Translate from PWM min and max values to percent

  digitalWrite(_leftEnable , true);
  digitalWrite(_rightEnable , true);
  //ledcWrite(pwmChannelLeft, speedPercent); 
  //ledcWrite(pwmChannelRight, speedPercent);
}

void Move::stop(){
  allDisable();
}

void Move::allDisable(){
  leftMotorDisable();
  rightMotorDisable();
}

void Move::leftMotorForward(){
  digitalWrite(_input3, true);
  digitalWrite(_input4, false);
}

void Move::leftMotorBackward(){
  digitalWrite(_input3, false);
  digitalWrite(_input4, true);
}

void Move::leftMotorDisable(){
  digitalWrite(_input3, false);
  digitalWrite(_input4, false);
}

void Move::rightMotorForward(){
  digitalWrite(_input1, true);
  digitalWrite(_input2, false);
}

void Move::rightMotorBackward(){
  digitalWrite(_input1, false);
  digitalWrite(_input2, true);
}

void Move::rightMotorDisable(){
  digitalWrite(_input1, false);
  digitalWrite(_input2, false);
}