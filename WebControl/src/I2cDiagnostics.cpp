#include "I2cDiagnostics.h"
#include "Pins.h"
#include <Arduino.h>
#include <Wire.h>

namespace {
bool readByte(uint8_t address, uint8_t reg, uint8_t& value) {
  Wire.beginTransmission(address);
  Wire.write(reg); // Nur Registeradresse fuer den anschliessenden Lesezugriff.
  if (Wire.endTransmission(false) != 0 || Wire.requestFrom(address, uint8_t(1)) != 1)
    return false;
  value = Wire.read();
  return true;
}
}

void printI2cDiagnostics() {
  Serial.printf("I2C SCAN SDA=%u SCL=%u levels=%d,%d\n", Pins::SensorSda, Pins::SensorScl,
                digitalRead(Pins::SensorSda), digitalRead(Pins::SensorScl));
  unsigned found = 0;
  for (uint8_t address = 0x08; address <= 0x77; ++address) {
    Wire.beginTransmission(address);
    uint8_t result = Wire.endTransmission();
    if (result == 5) {
      Serial.printf("I2C SCAN aborted: bus timeout at 0x%02X\n", address);
      return;
    }
    if (result != 0)
      continue;
    ++found;
    Serial.printf("I2C ACK address=0x%02X\n", address);
    // Dokumentierte IMU-Adressen: BMI160 0x68/69, LSM6DS3 0x6A/6B.
    if (address < 0x68 || address > 0x6B)
      continue;
    uint8_t id = 0;
    if (readByte(address, 0x00, id))
      Serial.printf("I2C ID register=0x00 value=0x%02X%s\n", id,
                    id == 0xD1 ? " (BMI160)" : "");
    else
      Serial.println("I2C ID register=0x00 read failed");
    if (readByte(address, 0x0F, id))
      Serial.printf("I2C ID register=0x0F value=0x%02X%s\n", id,
                    id == 0x69 ? " (LSM6DS3)" : "");
    else
      Serial.println("I2C ID register=0x0F read failed");
  }
  Serial.printf("I2C SCAN complete devices=%u\n", found);
}
