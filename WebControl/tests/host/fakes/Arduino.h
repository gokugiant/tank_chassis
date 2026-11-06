#pragma once
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <initializer_list>
constexpr int HIGH = 1, LOW = 0, OUTPUT = 1, INPUT = 0;
extern uint32_t testTime;
extern int gpioValues[40], pwmValues[16];

inline uint32_t millis() {
  return testTime;
}

inline void digitalWrite(int p, int v) {
  gpioValues[p] = v;
}

inline int digitalRead(int p) {
  return gpioValues[p];
}

inline void pinMode(int, int) {
}

inline void ledcSetup(int, int, int) {
}

inline void ledcAttachPin(int, int) {
}

inline void ledcWrite(int channel, int v) {
  pwmValues[channel] = v;
}

template <class T> T constrain(T v, T low, T high) {
  return std::max(low, std::min(v, high));
}
