#include "TankDrive.h"
#include "Pins.h"
#include "Config.h"

namespace {
constexpr int LeftChannel = 7, RightChannel = 6;

void track(int channel, int forwardPin, int backwardPin, int percent) {
  ledcWrite(channel, 0); // Vor jedem Richtungswechsel Leistung wegnehmen.
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, LOW);
  if (percent != 0) {
    digitalWrite(percent > 0 ? forwardPin : backwardPin, HIGH);
    ledcWrite(channel, abs(percent) * 255 / 100);
  }
}
} // namespace

bool TankDrive::begin() {
  for (auto p : {Pins::LeftEnable, Pins::RightEnable, Pins::LeftForward, Pins::LeftBackward,
                 Pins::RightForward, Pins::RightBackward}) {
    digitalWrite(p, LOW);
    pinMode(p, OUTPUT);
  }
  mutex_ = xSemaphoreCreateMutex();
  if (!mutex_)
    return false;
  ledcSetup(LeftChannel, 1000, 8);
  ledcSetup(RightChannel, 1000, 8);
  ledcWrite(LeftChannel, 0);
  ledcWrite(RightChannel, 0);
  ledcAttachPin(Pins::LeftEnable, LeftChannel);
  ledcAttachPin(Pins::RightEnable, RightChannel);
  stop();
  esp_timer_create_args_t args = {};
  args.callback = &TankDrive::timerCallback;
  args.arg = this;
  args.name = "driveTimeout";
  if (esp_timer_create(&args, &timer_) != ESP_OK)
    return false;
  return esp_timer_start_periodic(timer_, 20000) == ESP_OK;
}

void TankDrive::stopLocked() {
  track(LeftChannel, Pins::LeftForward, Pins::LeftBackward, 0);
  track(RightChannel, Pins::RightForward, Pins::RightBackward, 0);
  state_.left = state_.right = 0;
}

void TankDrive::arm(bool enabled) {
  xSemaphoreTake(mutex_, portMAX_DELAY);
  stopLocked();
  state_.armed = enabled;
  lastCommand_ = millis();
  xSemaphoreGive(mutex_);
}

void TankDrive::stop() {
  xSemaphoreTake(mutex_, portMAX_DELAY);
  stopLocked();
  xSemaphoreGive(mutex_);
}

bool TankDrive::setTracks(int left, int right, uint32_t commandAt) {
  xSemaphoreTake(mutex_, portMAX_DELAY);
  if (!state_.armed) {
    xSemaphoreGive(mutex_);
    return false;
  }
  if (uint32_t(millis() - commandAt) >= Config::CommandTimeoutMs) {
    stopLocked();
    state_.armed = false;
    xSemaphoreGive(mutex_);
    return false;
  }
  // 0 bleibt STOPP. Jeder laufende Motor bekommt mindestens MinSpeed.
  auto usablePwm = [](int value) {
    value = constrain(value, -Config::MaxSpeed, Config::MaxSpeed);
    if (value > 0 && value < Config::MinSpeed)
      return Config::MinSpeed;
    if (value < 0 && value > -Config::MinSpeed)
      return -Config::MinSpeed;
    return value;
  };
  left = usablePwm(left);
  right = usablePwm(right);
  track(LeftChannel, Pins::LeftForward, Pins::LeftBackward, left);
  track(RightChannel, Pins::RightForward, Pins::RightBackward, right);
  state_.left = left;
  state_.right = right;
  lastCommand_ = commandAt;
  xSemaphoreGive(mutex_);
  return true;
}

bool TankDrive::forward(int speed) {
  return setTracks(speed, speed, millis());
}

bool TankDrive::backward(int speed) {
  return setTracks(-speed, -speed, millis());
}

bool TankDrive::turnLeft(int speed) {
  return setTracks(-speed, speed, millis());
}

bool TankDrive::turnRight(int speed) {
  return setTracks(speed, -speed, millis());
}

bool TankDrive::curve(bool forward, bool left) {
  const int sign = forward ? 1 : -1;
  const int outer = Config::MaxSpeed, inner = Config::MaxSpeed / 2;
  return setTracks(sign * (left ? inner : outer), sign * (left ? outer : inner), millis());
}

DriveState TankDrive::state() {
  xSemaphoreTake(mutex_, portMAX_DELAY);
  auto value = state_;
  xSemaphoreGive(mutex_);
  return value;
}

void TankDrive::timerCallback(void* context) {
  static_cast<TankDrive*>(context)->checkTimeout();
}

void TankDrive::checkTimeout() {
  // Eigener Timer-Task: stoppt auch, wenn HTTP oder Display das loop() aufhaelt.
  xSemaphoreTake(mutex_, portMAX_DELAY);
  uint32_t age = millis() - lastCommand_;
  bool moving = state_.left != 0 || state_.right != 0;
  if (state_.armed &&
      ((moving && age >= Config::CommandTimeoutMs) || age >= Config::ArmTimeoutMs)) {
    stopLocked();
    state_.armed = false;
  }
  xSemaphoreGive(mutex_);
}
