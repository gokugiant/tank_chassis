#pragma once
#include <Arduino.h>
#include <Wire.h>

// BMI160 / LSM6DS3: nur Messwerte, noch keine automatische Motorregelung.
class MotionSensor {
public:
  struct Reading {
    bool connected = false, valid = false;
    uint8_t address = 0, chipId = 0;
    float ax = 0, ay = 0, az = 0; // Beschleunigung inklusive Schwerkraft, in g.
    float gx = 0, gy = 0, gz = 0; // Rohe Drehrate, in Grad/s (noch ohne Nullpunktabgleich).
    uint32_t sampledAt = 0;
  };
  bool begin();
  void update();
  Reading reading() const;
  const char* model() const;

private:
  bool connect();
  bool readRegisters(uint8_t reg, uint8_t* data, uint8_t count);
  bool writeRegister(uint8_t reg, uint8_t value);
  enum class Startup { Ready, AccelWait, GyroWait };
  Startup startup_ = Startup::Ready;
  uint32_t startupAt_ = 0;
  bool bmi160_ = false;
  Reading state_;
  uint32_t lastPoll_ = 0, lastConnect_ = 0;
};
