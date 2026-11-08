#pragma once
#include "TankDrive.h"
#include "Heading.h"
#include "Config.h"

// Links ist positiver Winkel: am Fahrzeug durch Handdrehung bestaetigt.
class Navigation {
public:
  enum class Mode { Idle, Straight, Turn };
  Navigation(TankDrive& drive, Heading& heading) : drive_(drive), heading_(heading) {}
  bool straight(int speed);
  bool turn(int degrees, int maxPwm = Config::MinSpeed); // +90/+180/+360 links, negative Winkel rechts.
  bool heartbeat();
  void cancel(const char* reason = "Gestoppt");
  void update();
  Mode mode() const { return mode_; }
  float target() const { return target_; }
  float error() const { return target_ - heading_.angle(); }
  const char* status() const { return status_; }
  const char* modeName() const;

private:
  void fail(const char* reason);
  void finish();
  void settle();
  enum class TurnPhase { Approach, Settle, TrimPulse };
  TurnPhase turnPhase_ = TurnPhase::Approach;
  bool completed_ = false;
  int trimSign_ = 1;
  unsigned trims_ = 0, pulseMs_ = 0;
  uint32_t phaseAt_ = 0, stillMs_ = 0, turnTimeoutMs_ = Config::TurnTimeoutMs;
  float lastTurnRate_ = 20;
  TankDrive& drive_;
  Heading& heading_;
  Mode mode_ = Mode::Idle;
  int base_ = 0, turnSign_ = 1, turnMaxPwm_ = Config::MinSpeed;
  float target_ = 0, startAngle_ = 0, progressAngle_ = 0;
  uint32_t lease_ = 0, started_ = 0, lastStep_ = 0, progressAt_ = 0;
  const char* status_ = "Bereit";
};
