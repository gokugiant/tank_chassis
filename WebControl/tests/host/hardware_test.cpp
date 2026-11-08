#include "Pins.h"
#include <cassert>
#include <iostream>
#include "TankDrive.h"
#include "Config.h"
#include "Button.h"
#include "RgbLed.h"
#include "Turret.h"
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
  assert(Config::FrontReversed);
  // Neue linke Kette ist die alte rechte; neue Vorwaertsrichtung ist altes Rueckwaerts.
  assert(Pins::LeftEnable == 13 && Pins::RightEnable == 21);
  assert(pwmAttachedPins[7] == 13 && pwmAttachedPins[6] == 21);
  assert(drive.forward(40));
  assert(gpioValues[22] == HIGH && gpioValues[2] == HIGH);
  assert(gpioValues[17] == LOW && gpioValues[15] == LOW);
  assert(gpioValues[Pins::LeftForward] == HIGH && gpioValues[Pins::LeftBackward] == LOW);
  assert(gpioValues[Pins::RightForward] == HIGH && gpioValues[Pins::RightBackward] == LOW);
  assert(pwmValues[7] == Config::MinSpeed * 255 / 100 && pwmValues[6] == Config::MinSpeed * 255 / 100);
  assert(drive.backward(40));
  assert(gpioValues[Pins::LeftBackward] == HIGH && gpioValues[Pins::RightBackward] == HIGH);
  assert(gpioValues[Pins::LeftForward] == LOW && gpioValues[Pins::RightForward] == LOW);
  assert(drive.turnLeft(Config::MaxSpeed + 10));
  assert(drive.state().left == -Config::MaxSpeed && drive.state().right == Config::MaxSpeed);
  // Untere Hardwaregrenze gilt fuer beide Richtungen, STOPP bleibt exakt null.
  for (int speed : {1, 49, 50, 100}) {
    assert(drive.forward(speed));
    assert(drive.state().left == std::max(speed, Config::MinSpeed));
    assert(drive.state().left == drive.state().right);
    assert(drive.backward(speed));
    assert(drive.state().left == -std::max(speed, Config::MinSpeed));
  }
  assert(drive.differential(1, -1, millis()));
  assert(drive.state().left == Config::MinSpeed && drive.state().right == -Config::MinSpeed);
  assert(drive.forward(0)); stopped(drive);
  assert(drive.turnLeft(50));
  assert(gpioValues[22] == HIGH && gpioValues[15] == HIGH); // Positive Gier bleibt links.
  for (bool forward : {false, true}) {
    for (bool left : {false, true}) {
      assert(drive.curve(forward, left));
      const auto s = drive.state();
      assert(std::abs(s.left) == (left ? 50 : 100));
      assert(std::abs(s.right) == (left ? 100 : 50));
      assert((s.left > 0) == forward && (s.right > 0) == forward);
    }
  }
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
  assert(drive.state().left == Config::MinSpeed);
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
  gpioValues[Pins::Button] = LOW;
  testTime = 0;
  button.begin();
  testTime = 40;
  button.update();
  gpioValues[Pins::Button] = HIGH;
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
  gpioValues[Pins::Button] = LOW;
  button.update();
  testTime = 540;
  button.update();
  gpioValues[Pins::Button] = HIGH;
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
  Turret turret;
  assert(!turret.center());
  assert(turret.begin());
  assert(!turret.active() && pwmValues[4] == 0);
  assert(!turret.setAngle(-1) && !turret.setAngle(181));
  assert(turret.setAngle(0) && pwmValues[4] == 3276);
  assert(turret.setAngle(180) && pwmValues[4] == 6553);
  testTime = 0xffffff00;
  assert(turret.center());
  assert(turret.angle() == 90 && pwmValues[4] == 4915);
  testTime += 999;
  turret.update();
  assert(turret.active());
  testTime += 1;
  turret.update();
  assert(!turret.active() && pwmValues[4] == 0);
  assert(turret.setAngle(120));
  turret.stop();
  assert(!turret.active() && pwmValues[4] == 0);
  std::cout << "PASS: reversed front, four curves 100/50, directions, PWM cap, stop, command/arm timeouts, wraparound, button "
               "debounce, RGB, turret limits/pulse/timeout/stop\n";
}
