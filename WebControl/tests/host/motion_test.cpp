#include "MotionSensor.h"
#include "Pins.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <cstring>
uint32_t testTime = 0;
int gpioValues[40] = {}, pwmValues[16] = {};
FakeWire Wire;
void raw(int reg, int16_t v) {
  Wire.memory[reg] = v & 255;
  Wire.memory[reg + 1] = uint16_t(v) >> 8;
}
int main() {
  Wire.memory[0x0F] = 0x69;
  MotionSensor sensor;
  assert(sensor.begin());
  assert(Wire.sda == 33 && Wire.scl == 32);
  assert(Wire.memory[0x10] == 0x40 && Wire.memory[0x11] == 0x40);
  assert(!sensor.reading().valid);
  Wire.memory[0x1E] = 3;
  raw(0x22, -1000); raw(0x24, 2000); raw(0x26, 10000);
  raw(0x28, -16384); raw(0x2A, 0); raw(0x2C, 16384);
  testTime = 20; sensor.update();
  auto m = sensor.reading();
  assert(m.valid && std::abs(m.gz - 87.5f) < 0.001f);
  assert(std::abs(m.gx + 8.75f) < 0.001f);
  assert(std::abs(m.ax + 0.999424f) < 0.001f && std::abs(m.az - 0.999424f) < 0.001f);
  testTime = 321; assert(!sensor.reading().valid);
  Wire.shortRead = true; sensor.update();
  assert(!sensor.reading().connected && !sensor.reading().valid);
  Wire.shortRead = false; testTime = 3300; sensor.update();
  assert(sensor.reading().connected && !sensor.reading().valid);
  testTime += 20; sensor.update(); assert(sensor.reading().valid);
  Wire.offline = true; testTime += 20; sensor.update();
  assert(!sensor.reading().valid);
  // Zweite Adresse sowie unbekannter Chip: kein blindes Konfigurieren.
  Wire.offline = false; Wire.device = 0x6B;
  MotionSensor second; assert(second.begin() && second.reading().address == 0x6B);
  Wire.memory[0x0F] = 0xFF; Wire.memory[0x10] = 0;
  MotionSensor unknown; assert(!unknown.begin()); assert(Wire.memory[0x10] == 0);
  // BMI160: echter Wertebereich, Einschaltzeiten und fehlgeschlagener PMU-Start.
  Wire.memory.fill(0); Wire.device = 0x68; Wire.memory[0] = 0xD1;
  testTime = 10000;
  MotionSensor bmi; assert(bmi.begin());
  assert(std::strcmp(bmi.model(), "BMI160") == 0);
  assert(Wire.memory[0x7E] == 0x11 && !bmi.reading().valid);
  testTime += 4; bmi.update(); assert(Wire.memory[0x7E] == 0x11);
  testTime += 1; bmi.update(); assert(Wire.memory[0x7E] == 0x15);
  Wire.memory[0x03] = 0x14; Wire.memory[0x1B] = 0xC0;
  raw(0x0C, -13120); raw(0x0E, 0); raw(0x10, 6560);
  raw(0x12, -16384); raw(0x14, 0); raw(0x16, 16384);
  testTime += 99; bmi.update(); assert(!bmi.reading().valid);
  testTime += 1; bmi.update();
  m = bmi.reading();
  assert(m.valid && m.address == 0x68 && m.chipId == 0xD1);
  assert(std::abs(m.gx + 100) < 0.001f && std::abs(m.gz - 50) < 0.001f);
  assert(m.ax == -1 && m.az == 1);
  Wire.memory[0x1B] = 0; testTime += 301; bmi.update();
  assert(!bmi.reading().valid && !bmi.reading().connected);
  // Weitere BMI160-Adresse und kein Freigeben von Messwerten bei PMU-Fehler.
  Wire.device = 0x69;
  MotionSensor badPmu; assert(badPmu.begin());
  testTime += 5; badPmu.update();
  Wire.memory[0x03] = 0; testTime += 100; badPmu.update();
  assert(!badPmu.reading().connected && !badPmu.reading().valid);
  Wire.device = 0x68; Wire.memory[0] = 0xFF; Wire.memory[0x40] = 0;
  MotionSensor wrongBmi; assert(!wrongBmi.begin()); assert(Wire.memory[0x40] == 0);
  std::cout << "Motion sensor: conversion, freshness, disconnect, retry and identity passed\n";
}
