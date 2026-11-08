#pragma once
#include <cstdint>
#include <vector>
#include <array>
struct FakeWire {
  uint8_t device = 0x6A, target = 0, reg = 0;
  bool offline = false, shortRead = false;
  int sda = -1, scl = -1;
  std::array<uint8_t, 256> memory{};
  std::vector<uint8_t> tx;
  void begin(int a, int b, int) { sda = a; scl = b; }
  void setTimeOut(int) {}
  void beginTransmission(uint8_t a) { target = a; tx.clear(); }
  void write(uint8_t b) { tx.push_back(b); }
  int endTransmission(bool = true) {
    if (offline || target != device) return 2;
    reg = tx[0];
    if (tx.size() > 1) memory[reg] = tx[1];
    return 0;
  }
  int requestFrom(uint8_t, uint8_t n) { return shortRead ? n-1 : n; }
  int read() { return memory[reg++]; }
};
extern FakeWire Wire;
