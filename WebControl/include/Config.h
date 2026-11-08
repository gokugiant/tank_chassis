#pragma once

namespace Config {
constexpr bool FrontReversed = true; // Neue Vorderseite: bisheriges Heck.
constexpr char WifiName[] = "Tank-Lab";
constexpr char WifiPassword[] = "TankLab2026"; // Demo-Passwort, keine privaten Zugangsdaten.
constexpr int MinSpeed = 50, MaxSpeed = 100;                  // PWM-Prozent, keine gemessene Geschwindigkeit.
constexpr unsigned CommandTimeoutMs = 500;
constexpr unsigned ArmTimeoutMs = 30000;
// Startwerte fuer die Fahrzeugabstimmung, noch keine zugesicherte Winkelgenauigkeit.
constexpr int TurnMinPwm = MinSpeed, TurnMaxPwm = MaxSpeed, StraightMaxCorrection = 12;
constexpr float StraightKp = 1.2f, StraightKd = 0.25f;
constexpr float TurnKp = 0.65f, TurnToleranceDegrees = 1.0f;
constexpr unsigned TurnTimeoutMs = 12000;
// Schaetzwert fuer Nachlauf; am Fahrzeug noch abzustimmen.
constexpr float TurnCoastSeconds = 0.08f, TurnMaxLeadDegrees = 12.0f;
constexpr float TurnStillRate = 2.5f;
constexpr unsigned TurnSettleMinMs = 250, TurnStillMs = 160, TurnSettleTimeoutMs = 1500;
constexpr unsigned TurnTrimMinMs = 20, TurnTrimMaxMs = 60, TurnMaxTrims = 6;
} // namespace Config
