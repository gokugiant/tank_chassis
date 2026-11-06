#include <cassert>
#include <iostream>
#include "TankDrive.h"
#include "Config.h"
#include "Button.h"
#include "RgbLed.h"
uint32_t testTime = 0;
int gpioValues[40] = {}, pwmValues[16] = {};
esp_timer_create_args_t testTimer;

void tick(uint32_t t) {
  testTime = t;
  testTimer.callback(testTimer.arg);
}

void stopped(TankDrive& d) {
  auto s = d.state();
  assert(s.left == 0 && s.right == 0);
  assert(pwmValues[6] == 0 && pwmValues[7] == 0);
  for (int p : {15, 2, 17, 22})
    assert(gpioValues[p] == LOW);
}

int main() {
  TankDrive drive;
  assert(drive.begin());
  stopped(drive);
  assert(!drive.forward(40));
  drive.arm(true);
  assert(drive.forward(40));
  assert(gpioValues[17] == HIGH && gpioValues[22] == LOW);
  assert(gpioValues[15] == HIGH && gpioValues[2] == LOW);
  assert(pwmValues[7] == 102 && pwmValues[6] == 102);
  assert(drive.backward(40));
  assert(gpioValues[22] == HIGH && gpioValues[2] == HIGH);
  assert(gpioValues[17] == LOW && gpioValues[15] == LOW);
  assert(drive.turnLeft(Config::MaxSpeed + 10));
  assert(drive.state().left == -Config::MaxSpeed && drive.state().right == Config::MaxSpeed);
  assert(drive.turnRight(50));
  assert(drive.state().left == 50 && drive.state().right == -50);
  tick(499);
  assert(drive.state().armed);
  tick(500);
  stopped(drive);
  assert(!drive.state().armed);
  drive.arm(true);
  assert(drive.forward(30));
  testTime = 900;
  assert(drive.forward(30));
  tick(1399);
  assert(drive.state().left == 30);
  tick(1400);
  stopped(drive);
  drive.arm(true);
  drive.stop();
  assert(drive.state().armed);
  tick(31400);
  assert(!drive.state().armed);
  // Unsigned timeout must also work over millis() wraparound.
  testTime = 0xffffff00;
  drive.arm(true);
  assert(drive.forward(40));
  tick(0x100);
  stopped(drive);
  assert(!drive.state().armed);
  Button button;
  gpioValues[32] = LOW;
  testTime = 0;
  button.begin();
  testTime = 40;
  button.update();
  gpioValues[32] = HIGH;
  button.update();
  testTime = 60;
  button.update();
  assert(!button.wasPressed());
  testTime = 80;
  button.update();
  assert(button.wasPressed());
  assert(!button.wasPressed());
  assert(button.pressCount() == 1);
  testTime = 500;
  button.update();
  assert(button.pressCount() == 1);
  gpioValues[32] = LOW;
  button.update();
  testTime = 540;
  button.update();
  gpioValues[32] = HIGH;
  button.update();
  testTime = 580;
  button.update();
  assert(button.wasPressed() && button.pressCount() == 2);
  RgbLed led;
  led.begin();
  led.setColor(1, 128, 255);
  assert(pwmValues[0] == 1 && pwmValues[1] == 128 && pwmValues[2] == 255);
  led.off();
  assert(pwmValues[0] == 0 && pwmValues[1] == 0 && pwmValues[2] == 0);
  std::cout << "PASS: directions, PWM cap, stop, command/arm timeouts, wraparound, button "
               "debounce, RGB\n";
}
