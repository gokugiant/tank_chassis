#include "Turret.h"
#include "Pins.h"

namespace {
// Kanal 4/Timer 2: unabhaengig von RGB (0..2) und Antrieb (6..7).
constexpr int Channel = 4, Frequency = 50, Resolution = 16;
constexpr uint32_t HoldMs = 1000;
}

bool Turret::begin() {
  digitalWrite(Pins::Servo, LOW);
  pinMode(Pins::Servo, OUTPUT);
  ready_ = ledcSetup(Channel, Frequency, Resolution) > 0;
  if (!ready_)
    return false;
  ledcWrite(Channel, 0); // Kein automatisches Zentrieren beim Einschalten.
  ledcAttachPin(Pins::Servo, Channel);
  return true;
}

bool Turret::setAngle(int degrees) {
  if (!ready_ || degrees < MinAngle || degrees > MaxAngle)
    return false;
  // Ueblicher Positionsservo: 1000..2000 us entsprechen nominell 0..180 Grad.
  const uint32_t pulseUs = 1000 + degrees * 1000 / 180;
  ledcWrite(Channel, pulseUs * 65535UL / 20000);
  angle_ = degrees;
  active_ = true;
  lastCommand_ = millis();
  return true;
}

void Turret::stop() {
  if (ready_)
    ledcWrite(Channel, 0);
  active_ = false;
}

void Turret::update() {
  if (active_ && uint32_t(millis() - lastCommand_) >= HoldMs)
    stop();
}
