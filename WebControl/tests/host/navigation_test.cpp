#include "Navigation.h"
#include <cassert>
#include <cmath>
#include <iostream>
uint32_t testTime = 0;
int gpioValues[40] = {}, pwmValues[16] = {};
esp_timer_create_args_t testTimer;
struct Fixture {
  TankDrive drive;
  Heading heading;
  Navigation nav{drive, heading};
  MotionSensor::Reading sample;
  float physicalRate = 0;
  bool reversed = false;
  int originalSign = 1;
  Fixture() {
    assert(drive.begin());
    sample.valid = true; sample.az = 1;
    assert(heading.calibrate(true, sample));
    for (int i = 0; i < 100; ++i) step(0, false);
    assert(heading.ready());
    drive.arm(true);
  }
  void step(float rate, bool timer = true) {
    testTime += 20; sample.sampledAt = testTime; sample.gz = rate;
    heading.update(sample, !drive.state().armed);
    if (timer) testTimer.callback(testTimer.arg);
    nav.update();
  }
  void physicalStep(float inertia = 0.1f) {
    auto output = drive.state();
    const float desiredRate = 0.8f * (output.right - output.left);
    physicalRate += (desiredRate - physicalRate) * (1 - std::exp(-0.02f / inertia));
    step(physicalRate);
    output = drive.state();
    if ((output.right - output.left) * originalSign < 0)
      reversed = true;
  }
  void finishTurn(float inertia = 0.1f) {
    for (int i = 0; i < 650 && nav.mode() != Navigation::Mode::Idle; ++i) {
      assert(nav.heartbeat()); physicalStep(inertia);
      auto s = drive.state();
      assert(s.left == 0 || std::abs(s.left) >= Config::MinSpeed);
      assert(s.right == 0 || std::abs(s.right) >= Config::MinSpeed);
    }
  }
  void stopped() { auto s = drive.state(); assert(s.left == 0 && s.right == 0); }
};
int main() {
  {
    Fixture f; assert(!f.nav.turn(45)); assert(f.nav.turn(90, 75));
    f.step(0); assert(f.drive.state().left == -75 && f.drive.state().right == 75);
    assert(!f.nav.turn(180));
    f.finishTurn();
    f.stopped(); assert(f.drive.state().armed);
    assert(std::abs(f.heading.angle() - 90) <= Config::TurnToleranceDegrees);
    // Spaeter Heartbeat nach Erfolg darf nicht sperren oder Motoren starten.
    assert(f.nav.heartbeat()); f.stopped();
    // Zweite Drehung ohne erneute Fahrfreigabe.
    const float nextTarget = f.heading.angle() + 90;
    assert(f.nav.turn(90)); f.finishTurn();
    assert(f.drive.state().armed && std::abs(f.heading.angle() - nextTarget) <= Config::TurnToleranceDegrees);
  }
  {
    Fixture f; f.originalSign = -1; assert(f.nav.turn(-180));
    f.physicalStep(); assert(f.drive.state().left > 0 && f.drive.state().right < 0);
    f.finishTurn();
    f.stopped(); assert(f.drive.state().armed);
    assert(std::abs(f.heading.angle() + 180) <= Config::TurnToleranceDegrees);
  }
  {
    Fixture f; assert(f.nav.turn(90));
    for (int i = 0; i < 26; ++i) f.step(0);
    f.stopped(); assert(!f.drive.state().armed); assert(!f.nav.heartbeat());
  }
  {
    Fixture f; assert(f.nav.turn(90));
    for (int i = 0; i < 76 && f.drive.state().armed; ++i) { f.nav.heartbeat(); f.step(0); }
    f.stopped(); assert(!f.drive.state().armed); // Mechanisch blockiert/aufgebockt.
  }
  {
    Fixture f; assert(f.nav.turn(90));
    for (int i = 0; i < 10 && f.drive.state().armed; ++i) { f.nav.heartbeat(); f.step(-90); }
    f.stopped(); assert(!f.drive.state().armed); // Vorzeichenfehler.
  }
  {
    Fixture f; assert(f.nav.turn(90)); f.step(0);
    testTime += 101; f.nav.update(); f.stopped(); assert(!f.drive.state().armed);
  }
  {
    Fixture f; assert(f.nav.straight(50));
    const float target = f.nav.target(); f.step(-30);
    assert(f.drive.state().left < f.drive.state().right); // Positive Gegenkorrektur.
    assert(f.nav.straight(50) && f.nav.target() == target); // Ziel nicht fortlaufend verschieben.
    f.nav.cancel(); f.stopped();
    assert(f.nav.straight(-50)); f.step(30);
    auto s = f.drive.state(); assert(s.left < 0 && s.right < 0 && s.left > s.right);
    f.drive.arm(false); f.nav.update(); f.stopped();
    assert(f.nav.mode() == Navigation::Mode::Idle);
  }
  {
    Fixture f; assert(f.nav.turn(90));
    // Mit frischen Heartbeats, aber sehr wenig Fortschritt: harte Gesamtdauer.
    for (int i = 0; i < 610 && f.drive.state().armed; ++i) { f.nav.heartbeat(); f.step(2); }
    f.stopped(); assert(!f.drive.state().armed);
  }
  {
    Fixture f; const uint32_t began = testTime;
    assert(f.nav.turn(90));
    for (int i = 0; i < 5; ++i) f.step(0);
    testTime = began + 501;
    // Auch ohne weiteren loop()/Navigation-Aufruf muss der Timer selbst stoppen.
    testTimer.callback(testTimer.arg); f.stopped(); assert(!f.drive.state().armed);
  }
  {
    Fixture f;
    assert(!f.nav.turn(90, 49) && !f.nav.turn(90, 101));
    assert(f.nav.turn(90, 100)); f.step(0);
    assert(f.drive.state().left == -100 && f.drive.state().right == 100);
    f.nav.cancel();
    assert(!f.nav.straight(49) && !f.nav.straight(-49));
  }
  for (int base : {50, 55, 75, 95, 100, -50, -55, -75, -95, -100}) {
    Fixture f; assert(f.nav.straight(base));
    for (int i = 0; i < 10; ++i) {
      assert(f.nav.straight(base)); f.step(40);
      auto s = f.drive.state();
      assert(std::abs(s.left) >= Config::MinSpeed && std::abs(s.left) <= Config::MaxSpeed);
      assert(std::abs(s.right) >= Config::MinSpeed && std::abs(s.right) <= Config::MaxSpeed);
      assert((s.left > 0) == (base > 0) && (s.right > 0) == (base > 0));
      assert(s.left != s.right); // Korrekturdifferenz bleibt auch am Rand erhalten.
    }
    f.nav.cancel(); f.stopped();
  }
  // Verschiedener Nachlauf; Erfolg erst im gemessenen Stillstand.
  for (float inertia : {0.05f, 0.15f}) {
    Fixture f; assert(f.nav.turn(90, 100)); f.finishTurn(inertia);
    f.stopped();
    if (inertia > 0.1f) assert(f.reversed); // Ueberdrehen erfordert echte Gegenkorrektur.
    assert(f.drive.state().armed && std::abs(f.heading.angle() - 90) <= Config::TurnToleranceDegrees);
  }
  {
    Fixture f; assert(f.nav.turn(90)); f.step(0);
    // Bis zum fruehen Abschalten drehen, danach bewegt sich das Modell trotz Trim nicht mehr.
    for (int i = 0; i < 55 && f.drive.state().left != 0; ++i) { f.nav.heartbeat(); f.step(90); }
    f.step(0);
    for (int i = 0; i < 550 && f.nav.mode() != Navigation::Mode::Idle; ++i) {
      f.nav.heartbeat(); f.step(0);
    }
    f.stopped(); assert(!f.drive.state().armed); // Begrenzte Zahl erfolgloser Feinkorrekturen.
  }
  for (int degrees : {360, -360}) {
    Fixture f; f.originalSign = degrees > 0 ? 1 : -1;
    assert(f.nav.turn(degrees, 75)); f.finishTurn(); f.stopped();
    assert(f.drive.state().armed && std::abs(f.heading.angle() - degrees) <= Config::TurnToleranceDegrees);
  }
  {
    Fixture f; assert(f.nav.turn(360));
    for (int i = 0; i < 610; ++i) { assert(f.nav.heartbeat()); f.step(20); }
    assert(f.drive.state().armed && f.nav.mode() == Navigation::Mode::Turn);
    f.nav.cancel(); f.stopped(); // 360 Grad haben bis zu 24 s statt 12 s Zeit.
  }
  std::cout << "Navigation: left/right 90/180/360, settling/trim, retained arm, lease, stall, wrong sign, stale angle, straight/reverse, total timeout passed\n";
}
