#pragma once
#include <Arduino.h>

// Am Fahrzeug bestaetigte Zuordnung. Links/rechts in Fahrtrichtung.
namespace Pins {
constexpr uint8_t LeftEnable = 21, LeftForward = 17, LeftBackward = 22;
constexpr uint8_t RightEnable = 13, RightForward = 15, RightBackward = 2;
constexpr uint8_t Red = 27, Green = 26, Blue = 25, Button = 32, Servo = 33;
// SPI-Displaypins stehen wegen TFT_eSPI auch als Buildflags in platformio.ini.
constexpr uint8_t DisplayBacklight = 4;
} // namespace Pins
