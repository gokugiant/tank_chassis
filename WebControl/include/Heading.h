#pragma once
#include "MotionSensor.h"

// Relative Drehung um die Sensor-Z-Achse, kein absoluter Kompasswinkel.
class Heading {
public:
  bool calibrate(bool stationary, const MotionSensor::Reading& sample);
  bool zero(bool stationary);
  void update(const MotionSensor::Reading& sample, bool stationary);
  bool ready() const { return ready_ && haveSample_ && uint32_t(millis() - lastSample_) <= 100; }
  bool calibrating() const { return calibrating_; }
  float angle() const { return angle_; }
  float rate() const { return rate_; }
  float bias() const { return bias_; }
  const char* status() const { return status_; }

private:
  void invalidate(const char* reason);
  bool ready_ = false, calibrating_ = false, haveSample_ = false;
  uint32_t lastSample_ = 0, started_ = 0;
  unsigned count_ = 0;
  float angle_ = 0, rate_ = 0, previousRate_ = 0, bias_ = 0;
  float sum_ = 0, minimum_ = 0, maximum_ = 0;
  // Beschleunigung ist waehrend der Fahrt kein direkter Neigungsmesser.
  float gravityX_ = 0, gravityY_ = 0, gravityZ_ = 1;
  uint32_t tiltEvidenceMs_ = 0;
  const char* status_ = "Bitte im Stillstand kalibrieren";
};
