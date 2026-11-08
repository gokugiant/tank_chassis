#pragma once
#include <Arduino.h>
#include "Config.h"

// Physische Kabel unveraendert. Logisches links/rechts folgt der gewaehlten Vorderseite.
namespace Pins {
constexpr uint8_t LeftEnable = Config::FrontReversed ? 13 : 21;
constexpr uint8_t LeftForward = Config::FrontReversed ? 2 : 17;
constexpr uint8_t LeftBackward = Config::FrontReversed ? 15 : 22;
constexpr uint8_t RightEnable = Config::FrontReversed ? 21 : 13;
constexpr uint8_t RightForward = Config::FrontReversed ? 22 : 15;
constexpr uint8_t RightBackward = Config::FrontReversed ? 17 : 2;
constexpr uint8_t Red = 27, Green = 26, Blue = 25, Button = 36, Servo = 12;
// GPIO36: Eingang ohne internen Pulldown; vorhandenen Tasterwiderstand behalten.
constexpr uint8_t SensorSda = 33, SensorScl = 32;
// SPI-Displaypins stehen wegen TFT_eSPI auch als Buildflags in platformio.ini.
constexpr uint8_t DisplayBacklight = 4;
} // namespace Pins
