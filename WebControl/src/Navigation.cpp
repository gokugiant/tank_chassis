#include "Navigation.h"
#include "Config.h"
#include <cmath>
#include <algorithm>

const char* Navigation::modeName() const {
  return mode_ == Mode::Turn ? "turn" : (mode_ == Mode::Straight ? "straight" : "idle");
}

void Navigation::cancel(const char* reason) {
  mode_ = Mode::Idle;
  completed_ = false;
  drive_.stop();
  status_ = reason;
}

void Navigation::fail(const char* reason) {
  mode_ = Mode::Idle;
  completed_ = false;
  drive_.arm(false);
  status_ = reason;
}

void Navigation::finish() {
  drive_.stop();
  mode_ = Mode::Idle;
  completed_ = true;
  status_ = "Drehziel erreicht"; // Erfolgreiche Aktion behaelt die Fahrfreigabe.
}

void Navigation::settle() {
  drive_.stop(); // Enable=0: Ausrollen. Noch keine aktive L298-Bremse.
  turnPhase_ = TurnPhase::Settle;
  phaseAt_ = millis();
  stillMs_ = 0;
  status_ = "Ausrollen und Winkel pruefen";
}

bool Navigation::straight(int speed) {
  if (!drive_.state().armed || !heading_.ready() || heading_.calibrating() ||
      std::abs(speed) < Config::MinSpeed || std::abs(speed) > Config::MaxSpeed)
    return false;
  // Wiederholte Fahrtasten-Befehle erneuern die Frist, nicht die Zielrichtung.
  if (mode_ != Mode::Straight || (base_ > 0) != (speed > 0)) {
    cancel();
    target_ = heading_.angle();
  }
  completed_ = false;
  base_ = speed;
  mode_ = Mode::Straight;
  lease_ = millis();
  status_ = "Richtung halten";
  return true;
}

bool Navigation::turn(int degrees, int maxPwm) {
  if (!drive_.state().armed || !heading_.ready() || heading_.calibrating() ||
      maxPwm < Config::MinSpeed || maxPwm > Config::TurnMaxPwm ||
      (degrees != 90 && degrees != -90 && degrees != 180 && degrees != -180 &&
       degrees != 360 && degrees != -360))
    return false;
  // Keine zweite automatische Aktion waehrend einer laufenden Bewegung starten.
  auto s = drive_.state();
  if (mode_ != Mode::Idle || s.left != 0 || s.right != 0)
    return false;
  completed_ = false;
  turnPhase_ = TurnPhase::Approach;
  trims_ = 0;
  stillMs_ = 0;
  lastTurnRate_ = 20;
  turnTimeoutMs_ = Config::TurnTimeoutMs * (std::abs(degrees) == 360 ? 2 : 1);
  turnMaxPwm_ = maxPwm;
  mode_ = Mode::Turn;
  startAngle_ = progressAngle_ = heading_.angle();
  target_ = startAngle_ + degrees;
  turnSign_ = degrees > 0 ? 1 : -1;
  lease_ = started_ = progressAt_ = millis();
  status_ = "Drehung aktiv";
  return true;
}

bool Navigation::heartbeat() {
  if (!drive_.state().armed)
    return false;
  // Ein bereits unterwegs befindlicher Heartbeat nach Erfolg ist harmlos.
  // Er verlaengert weder eine Bewegung noch die normale 30-s-Leerlauffreigabe.
  if (mode_ == Mode::Idle && completed_)
    return true;
  if (mode_ != Mode::Turn)
    return false;
  // Ein bereits abgelaufener Auftrag darf durch einen spaeten HTTP-Aufruf nicht wieder anlaufen.
  if (uint32_t(millis() - lease_) >= Config::CommandTimeoutMs) {
    fail("Browser-Timeout");
    return false;
  }
  lease_ = millis();
  return true;
}

void Navigation::update() {
  if (mode_ == Mode::Idle)
    return;
  if (!drive_.state().armed) {
    cancel("Antrieb gesperrt");
    return;
  }
  if (!heading_.ready()) {
    fail("Winkel ungueltig: gestoppt");
    return;
  }
  if (uint32_t(millis() - lease_) >= Config::CommandTimeoutMs) {
    fail("Browser-Timeout");
    return;
  }
  if (uint32_t(millis() - lastStep_) < 20)
    return;
  const uint32_t stepMs = millis() - lastStep_;
  lastStep_ = millis();
  const float deviation = error();
  if (mode_ == Mode::Straight) {
    if (std::fabs(deviation) > 20) {
      fail("Zu grosse Richtungsabweichung");
      return;
    }
    // PD-Regler: Winkelfehler korrigieren, gemessene Drehung daempfen.
    const int limit = std::min(Config::StraightMaxCorrection, std::abs(base_) / 2);
    const int correction = constrain(int(std::lround(Config::StraightKp * deviation - Config::StraightKd * heading_.rate())),
                                     -limit, limit);
    // Den gemeinsamen PWM-Anteil verschieben, statt die langsamere Kette
    // unter 50 % zu druecken oder die Regeldifferenz durch Einzel-Clamping zu verlieren.
    const int sign = base_ > 0 ? 1 : -1;
    int left = std::abs(base_) - sign * correction;
    int right = std::abs(base_) + sign * correction;
    int offset = std::max(0, Config::MinSpeed - std::min(left, right));
    offset += std::min(0, Config::MaxSpeed - (std::max(left, right) + offset));
    drive_.differential(sign * (left + offset), sign * (right + offset), lease_);
    return;
  }
  if (uint32_t(millis() - started_) >= turnTimeoutMs_) {
    fail("Drehzeit ueberschritten");
    return;
  }
  if (turnPhase_ == TurnPhase::TrimPulse) {
    if (uint32_t(millis() - phaseAt_) >= pulseMs_ || deviation * trimSign_ <= 0) {
      settle();
      return;
    }
    drive_.differential(-trimSign_ * Config::MinSpeed, trimSign_ * Config::MinSpeed, lease_);
    return;
  }
  if (turnPhase_ == TurnPhase::Settle) {
    drive_.stop();
    const uint32_t age = millis() - phaseAt_;
    if (std::fabs(heading_.rate()) <= Config::TurnStillRate)
      stillMs_ += stepMs;
    else
      stillMs_ = 0;
    if (age >= Config::TurnSettleTimeoutMs) {
      fail("Kein stabiler Stillstand");
      return;
    }
    if (age < Config::TurnSettleMinMs || stillMs_ < Config::TurnStillMs)
      return;
    if (std::fabs(deviation) <= Config::TurnToleranceDegrees) {
      finish();
      return;
    }
    if (trims_ >= Config::TurnMaxTrims || std::fabs(deviation) > 20) {
      fail("Drehziel nicht genau erreicht");
      return;
    }
    // Erst nach gemessenem Stillstand die Richtung wechseln, kurze 50-%-Impulse.
    trimSign_ = deviation > 0 ? 1 : -1;
    pulseMs_ = constrain(unsigned(std::lround(std::fabs(deviation) /
                         std::max(20.0f, lastTurnRate_) * 500.0f)),
                         Config::TurnTrimMinMs, Config::TurnTrimMaxMs);
    ++trims_;
    turnPhase_ = TurnPhase::TrimPulse;
    phaseAt_ = millis();
    status_ = "Winkel fein korrigieren";
    drive_.differential(-trimSign_ * Config::MinSpeed, trimSign_ * Config::MinSpeed, lease_);
    return;
  }
  const float progress = (heading_.angle() - startAngle_) * turnSign_;
  if (progress < -5) {
    fail("Falsche Drehrichtung");
    return;
  }
  const float rateTowardTarget = std::max(0.0f, heading_.rate() * turnSign_);
  lastTurnRate_ = std::max(20.0f, std::fabs(heading_.rate()));
  const float lead = constrain(rateTowardTarget * Config::TurnCoastSeconds,
                               Config::TurnToleranceDegrees, Config::TurnMaxLeadDegrees);
  // Schnelle Drehung frueher abschalten, danach das wirkliche Endergebnis messen.
  if (deviation * turnSign_ <= lead) {
    settle();
    return;
  }
  if (uint32_t(millis() - progressAt_) >= 1500) {
    if ((heading_.angle() - progressAngle_) * turnSign_ < 2) {
      fail("Keine Drehbewegung erkannt");
      return;
    }
    progressAngle_ = heading_.angle();
    progressAt_ = millis();
  }
  const int pwm = constrain(Config::TurnMinPwm + int(std::lround(std::fabs(deviation) * Config::TurnKp)),
                            Config::TurnMinPwm, turnMaxPwm_);
  drive_.differential(-turnSign_ * pwm, turnSign_ * pwm, lease_);
}
