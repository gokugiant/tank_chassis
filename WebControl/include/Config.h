#pragma once

namespace Config {
constexpr char WifiName[] = "Tank-Lab";
constexpr char WifiPassword[] = "TankLab2026"; // Demo-Passwort, keine privaten Zugangsdaten.
constexpr int MaxSpeed = 100;                  // PWM-Prozent, keine gemessene Geschwindigkeit.
constexpr unsigned CommandTimeoutMs = 500;
constexpr unsigned ArmTimeoutMs = 30000;
} // namespace Config
