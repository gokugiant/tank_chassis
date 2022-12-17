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
  void forward(int percent);

  void backward();
  void backward(int percent);

  void stop();

private:
  // Setting PWM properties
  const int freq = 30000;
  const int pwmChannelLeft = 6;
  const int pwmChannelRight = 7;
  const int resolution = 8;
  const int _pwmMin = 200;
  const int _pwmMax = 255;  // The calibration value for maximal pwm

	int _leftEnable;
	int _rightEnable;
  int _input1;
  int _input2;
  int _input3;
  int _input4;


  void allDisable();
  void leftMotorForward();
  void leftMotorBackward();
  void leftMotorDisable();
  void rightMotorForward();
  void rightMotorBackward();
  void rightMotorDisable();
};

#endif