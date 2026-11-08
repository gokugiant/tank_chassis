#include "Heading.h"
#include <cmath>

void Heading::invalidate(const char* reason) {
  ready_ = calibrating_ = haveSample_ = false;
  rate_ = 0;
  tiltEvidenceMs_ = 0;
  status_ = reason;
}

bool Heading::calibrate(bool stationary, const MotionSensor::Reading& sample) {
  if (!stationary || !sample.valid)
    return false;
  invalidate("Kalibrierung: 2 s still und flach halten");
  calibrating_ = true;
  count_ = 0;
  sum_ = 0;
  started_ = millis();
  return true;
}

bool Heading::zero(bool stationary) {
  if (!stationary || !ready_)
    return false;
  angle_ = 0;
  return true;
}

void Heading::update(const MotionSensor::Reading& sample, bool stationary) {
  if (!sample.valid || !std::isfinite(sample.gz) || !std::isfinite(sample.ax) ||
      !std::isfinite(sample.ay) || !std::isfinite(sample.az)) {
    if (ready_ || calibrating_)
      invalidate("Sensorluecke: neu kalibrieren");
    return;
  }
  if (calibrating_ && !stationary) {
    invalidate("Kalibrierung abgebrochen: Antrieb freigegeben");
    return;
  }
  if (haveSample_ && sample.sampledAt == lastSample_)
    return; // Nur neue Messwerte integrieren, nicht jede loop()-Runde.
  const uint32_t dt = sample.sampledAt - lastSample_;
  if (haveSample_ && dt > 150) {
    invalidate("Messluecke: neu kalibrieren");
    return;
  }
  const bool hadSample = haveSample_;
  haveSample_ = true;
  lastSample_ = sample.sampledAt;
  if (calibrating_) {
    const float norm = std::sqrt(sample.ax * sample.ax + sample.ay * sample.ay + sample.az * sample.az);
    // Z muss nach oben zeigen. Starke Bewegung oder Vibration verwirft den Abgleich.
    if (norm < 0.85f || norm > 1.15f || std::fabs(sample.ax) > 0.25f ||
        std::fabs(sample.ay) > 0.25f || sample.az < 0.8f || std::fabs(sample.gz) > 3.0f) {
      invalidate("Kalibrierung: Tank flach und still halten");
      return;
    }
    if (!count_)
      minimum_ = maximum_ = sample.gz;
    minimum_ = std::fmin(minimum_, sample.gz);
    maximum_ = std::fmax(maximum_, sample.gz);
    if (maximum_ - minimum_ > 0.8f) {
      invalidate("Kalibrierung: zu viel Bewegung");
      return;
    }
    sum_ += sample.gz;
    ++count_;
    if (uint32_t(millis() - started_) >= 2000 && count_ >= 80) {
      bias_ = sum_ / count_;
      angle_ = rate_ = previousRate_ = 0;
      gravityX_ = sample.ax;
      gravityY_ = sample.ay;
      gravityZ_ = sample.az;
      tiltEvidenceMs_ = 0;
      calibrating_ = false;
      ready_ = true;
      status_ = "Winkel bereit (relativ)";
    }
    return;
  }
  if (!ready_)
    return;
  // Einzelne Beschleunigungs-/Vibrationsspitzen sind keine Kippdiagnose.
  // Nur Beschleunigung nahe 1 g als Lagehinweis verwenden, Richtung glaetten
  // (250 ms) und erst bei >50 Grad fuer mindestens 300 ms abbrechen.
  // Auch das ist unter laengerer Fahrbeschleunigung nur ein Kippverdacht.
  const float norm = std::sqrt(sample.ax * sample.ax + sample.ay * sample.ay + sample.az * sample.az);
  if (norm >= 0.8f && norm <= 1.2f) {
    const float alpha = dt / (250.0f + dt);
    gravityX_ += alpha * (sample.ax - gravityX_);
    gravityY_ += alpha * (sample.ay - gravityY_);
    gravityZ_ += alpha * (sample.az - gravityZ_);
    const float gravityNorm = std::sqrt(gravityX_ * gravityX_ + gravityY_ * gravityY_ + gravityZ_ * gravityZ_);
    if (gravityNorm > 0.2f && gravityZ_ / gravityNorm < 0.642788f) {
      tiltEvidenceMs_ += dt;
      if (tiltEvidenceMs_ >= 300) {
        invalidate("Anhaltender Kippverdacht: neu kalibrieren");
        return;
      }
    } else {
      tiltEvidenceMs_ = 0;
    }
  } else {
    tiltEvidenceMs_ = 0; // Stoß/Fahrbeschleunigung: keine bestaetigte Lageinformation.
  }
  rate_ = sample.gz - bias_;
  if (hadSample)
    angle_ += (previousRate_ + rate_) * 0.5f * (dt / 1000.0f);
  previousRate_ = rate_;
}
