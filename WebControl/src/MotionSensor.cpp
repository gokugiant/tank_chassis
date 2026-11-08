#include "MotionSensor.h"
#include "Pins.h"

namespace {
constexpr uint8_t WhoAmI = 0x0F, ControlAccel = 0x10, ControlGyro = 0x11;
constexpr uint8_t ControlCommon = 0x12, Status = 0x1E, GyroOutput = 0x22;
// ST LSM6DS3 Datasheet: 0.061 mg/LSB bei +/-2 g; 8.75 mdps/LSB bei +/-250 dps.
// https://docs.arduino.cc/resources/datasheets/LSM6DS3-datasheet.pdf
float signedValue(const uint8_t* bytes) {
  return static_cast<int16_t>(uint16_t(bytes[0]) | (uint16_t(bytes[1]) << 8));
}
}

bool MotionSensor::begin() {
  Wire.begin(Pins::SensorSda, Pins::SensorScl, 100000);
  Wire.setTimeOut(5); // Kurzes I2C-Treiber-Timeout; keine Warteschleifen im Messbetrieb.
  return connect();
}

bool MotionSensor::readRegisters(uint8_t reg, uint8_t* data, uint8_t count) {
  Wire.beginTransmission(state_.address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0)
    return false;
  if (Wire.requestFrom(state_.address, count) != count)
    return false;
  for (uint8_t i = 0; i < count; ++i)
    data[i] = Wire.read();
  return true;
}

bool MotionSensor::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(state_.address);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool MotionSensor::connect() {
  lastConnect_ = millis();
  state_.connected = state_.valid = false;
  // BMI160 zuerst: am Fahrzeug durch CHIP_ID=0xD1 an 0x68 bestaetigt.
  for (uint8_t address : {0x68, 0x69, 0x6A, 0x6B}) {
    state_.address = address;
    uint8_t id = 0;
    bmi160_ = address < 0x6A;
    if (!readRegisters(bmi160_ ? 0x00 : WhoAmI, &id, 1))
      continue;
    state_.chipId = id;
    if (id != (bmi160_ ? 0xD1 : 0x69))
      continue; // Erst nach bestaetigter Chip-ID konfigurieren.
    if (bmi160_) {
      // Bosch BMI160 Datenblatt, Register 0x40..43 und CMD 0x7E:
      // https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi160-ds000.pdf
      // 100 Hz, normales Filter; +/-2 g und +/-250 Grad/s.
      if (!writeRegister(0x40, 0x28) || !writeRegister(0x41, 0x03) ||
          !writeRegister(0x42, 0x28) || !writeRegister(0x43, 0x03) ||
          !writeRegister(0x7E, 0x11))
        continue;
      // PMU-Kommandos nacheinander: maximal 3.8 ms fuer Accel, 80 ms fuer Gyro.
      // Warten ueber update(), damit Taster und Webserver weiter bedient werden.
      startup_ = Startup::AccelWait;
      startupAt_ = millis();
    } else {
      // LSM6DS3: BDU und Auto-Inkrement, 104 Hz, +/-2 g und +/-250 Grad/s.
      if (!writeRegister(ControlCommon, 0x44) || !writeRegister(ControlAccel, 0x40) ||
          !writeRegister(ControlGyro, 0x40))
        continue;
      startup_ = Startup::Ready;
    }
    state_.connected = true;
    lastPoll_ = state_.sampledAt = millis();
    return true;
  }
  return false;
}

const char* MotionSensor::model() const {
  if (state_.chipId == 0xD1)
    return "BMI160";
  if (state_.chipId == 0x69)
    return "LSM6DS3";
  return "unbekannt";
}

MotionSensor::Reading MotionSensor::reading() const {
  Reading result = state_;
  result.valid = result.valid && uint32_t(millis() - result.sampledAt) <= 300;
  return result;
}

void MotionSensor::update() {
  if (!state_.connected) {
    if (uint32_t(millis() - lastConnect_) >= 3000)
      connect();
    return;
  }
  if (startup_ == Startup::AccelWait) {
    if (uint32_t(millis() - startupAt_) < 5)
      return;
    if (!writeRegister(0x7E, 0x15)) {
      state_.connected = state_.valid = false;
      return;
    }
    startup_ = Startup::GyroWait;
    startupAt_ = millis();
    return;
  }
  if (startup_ == Startup::GyroWait) {
    if (uint32_t(millis() - startupAt_) < 100)
      return;
    uint8_t pmu = 0;
    // Accel und Gyro muessen beide Normalbetrieb melden, sonst keine Messwerte.
    if (!readRegisters(0x03, &pmu, 1) || (pmu & 0x3C) != 0x14) {
      state_.connected = state_.valid = false;
      return;
    }
    startup_ = Startup::Ready;
    state_.sampledAt = millis();
  }
  if (uint32_t(millis() - lastPoll_) < 20)
    return;
  lastPoll_ = millis();
  uint8_t status = 0, data[12];
  if (!readRegisters(bmi160_ ? 0x1B : Status, &status, 1)) {
    state_.connected = state_.valid = false;
    return;
  }
  const uint8_t readyMask = bmi160_ ? 0xC0 : 0x03;
  if ((status & readyMask) != readyMask) {
    if (uint32_t(millis() - state_.sampledAt) > 300)
      state_.connected = state_.valid = false;
    return;
  }
  if (!readRegisters(bmi160_ ? 0x0C : GyroOutput, data, sizeof(data))) {
    state_.connected = state_.valid = false;
    return;
  }
  // BMI160: 16384 LSB/g und 131.2 LSB/(Grad/s), Bosch-Datenblatt.
  const float gyroScale = bmi160_ ? 1.0f / 131.2f : 0.00875f;
  const float accelScale = bmi160_ ? 1.0f / 16384.0f : 0.000061f;
  state_.gx = signedValue(data) * gyroScale;
  state_.gy = signedValue(data + 2) * gyroScale;
  state_.gz = signedValue(data + 4) * gyroScale;
  state_.ax = signedValue(data + 6) * accelScale;
  state_.ay = signedValue(data + 8) * accelScale;
  state_.az = signedValue(data + 10) * accelScale;
  state_.sampledAt = millis();
  state_.valid = true;
}
