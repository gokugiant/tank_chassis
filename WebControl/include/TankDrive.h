#pragma once
#include <Arduino.h>
#include <esp_timer.h>
#include <freertos/semphr.h>

struct DriveState {
  bool armed;
  int left;
  int right;
};

class TankDrive {
public:
  bool begin();
  void arm(bool enabled);
  bool forward(int speed);
  bool backward(int speed);
  bool turnLeft(int speed);
  bool turnRight(int speed);
  // Diagonale Fahrt: aeussere Kette 100 %, innere 50 % (Minimum beachten).
  bool curve(bool forward, bool left);
  void stop();
  // Browser-Frist weiterreichen: lokale Regelschritte duerfen den Watchdog nicht verlaengern.
  bool differential(int left, int right, uint32_t commandAt) { return setTracks(left, right, commandAt); }
  DriveState state();

private:
  bool setTracks(int left, int right, uint32_t commandAt);
  void stopLocked();
  static void timerCallback(void* context);
  void checkTimeout();
  SemaphoreHandle_t mutex_ = nullptr;
  esp_timer_handle_t timer_ = nullptr;
  DriveState state_{false, 0, 0};
  uint32_t lastCommand_ = 0;
};
