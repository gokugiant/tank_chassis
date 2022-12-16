#ifndef Move_h
#define Move_h
#include <Arduino.h>

class Move {
public:
  Move(int leftEnable, int rightEnable, int input1, int input2, int input3, int input4);
  void turn90Left();
  void turn90Right();
  void turnLeftForMillis();
  void turnRightForMillis();
  void forward();
  void backward();
  void stop();

private:
	int _leftEnable;
	int _rightEnable;
  int _input1;
  int _input2;
  int _input3;
  int _input4;

  int _pwmMin;
  int _pwmMax;  // The calibration value for maximal pwm

  void allDisable();
  void leftMotorForward();
  void leftMotorBackward();
  void leftMotorDisable();
  void rightMotorForward();
  void rightMotorBackward();
  void rightMotorDisable();
};

#endif