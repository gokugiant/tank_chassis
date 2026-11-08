# Aktuelle Hardware und Pinbelegung

Stand: **8. November 2026**, aktive Firmware `WebControl/`.
Die Kabel sind gegenüber dem ursprünglichen Motoraufbau unverändert; das bisherige
Heck ist jetzt vorn (`Config::FrontReversed=true`). Links/rechts gelten aus Sicht
dieser neuen Fahrzeugfront. [Historischer Befund vom 5. November](archive/hardware-2026-11-05.md).

## Motoren / L298N

| Logische Kette, neue Fahrtrichtung | Enable-GPIO | Vorwärts HIGH | Rückwärts HIGH | LEDC-Kanal |
| ---------------------------------- | ----------: | ------------: | -------------: | ---------: |
| Links (bisher rechts)              |          13 |             2 |             15 |          7 |
| Rechts (bisher links)              |          21 |            22 |             17 |          6 |

Vorwärts/Rückwärts setzt den jeweils anderen Richtungseingang LOW. Beide
gegenläufigen Ketten drehen auf der Stelle. Stillstand: beide Enable-PWM 0 und
alle Richtungseingänge LOW (freies Ausrollen, keine aktive elektrische Bremse).
1 kHz, 8 Bit; laufende Motoren 50–100 %, STOPP 0. PWM ist keine gemessene Geschwindigkeit.
Neue logische Zuordnung ist im Hosttest geprüft; physische Bestätigung der neuen
Vorderseite und Kurven steht noch aus.

## Display

TTGO/LILYGO T-Display, ST7789, 135×240, **Rotation2** statt vorher Rotation0.
ESP32-D0WDQ6 Rev.1.0 erkannt; genaue Boardrevision nicht abgelesen.

| Signal                             | GPIO |
| ---------------------------------- | ---: |
| MOSI                               |   19 |
| SCLK                               |   18 |
| CS                                 |    5 |
| DC                                 |   16 |
| Reset                              |   23 |
| Hintergrundbeleuchtung, aktiv HIGH |    4 |

SPI 40 MHz. Displaypins für TFT_eSPI zusätzlich in `WebControl/platformio.ini`.
Servopfeil ist nominell in die neue Fahrzeugansicht transformiert; reale Servo-
Mitte, Drehrichtung und mechanische Anschläge sind weiterhin nicht vollständig validiert.

## Taster, RGB, Servo

| Funktion              |         GPIO | Hinweise                                                         |
| --------------------- | -----------: | ---------------------------------------------------------------- |
| Externer Taster       |           36 | aktiv HIGH, externer Pulldown bleibt; GPIO36 hat keinen internen |
| RGB Rot / Grün / Blau | 27 / 26 / 25 | aktiv HIGH, vorhandene Vorwiderstände                            |
| Positionsservo        |           12 | 50 Hz, nominell 0–180°, kein Startsignal, nach 1 s Signal aus    |

Die damalige Tasterleitung wurde von32 auf36 umgesteckt. GPIO12 ist ein Strapping-
Pin und darf beim Reset nicht durch den Servo auf HIGH gezogen werden.

## Bewegungssensor

Auf der violetten „160 / 6DS3“-Platine wurde elektronisch **BMI160** bestätigt:
I²C-Adresse `0x68`, CHIP_ID-Register `0x00` = `0xD1`.

| Sensoranschluss                | ESP32 / Verbindung |
| ------------------------------ | ------------------ |
| 3V3                            | 3V3                |
| GND                            | GND                |
| SDA                            | GPIO33             |
| SCL                            | GPIO32             |
| CS                             | Sensor 3V3         |
| SA0                            | GND                |
| VIN, OCS, INT1, INT2, SCX, SDX | unbeschaltet       |

Mit dieser Versorgung wurden gültige Messwerte empfangen; kein zusätzlicher VIN-
Anschluss ist dafür belegt. Der exakte Breakout-Schaltplan ist nicht bestätigt.
100 kHz I²C, 100 Hz Sensorrate, ±2 g / ±250 °/s. Die Rohachsen beziehen sich auf die
Platine. Flach mit Z nach oben montieren. Positive Z-Drehung nach links am Fahrzeug bestätigt.

**Letzter Zustand:** Sensor antwortet nicht; I²C-Suche findet 0 Geräte. Die obige
Verkabelung beschreibt die zuvor funktionierende Zuordnung, nicht eine aktuell
bestätigte Sensorverbindung. Erst Versorgung/Kontakte prüfen, keine Spannung erraten.

## Versorgung und Grenzen

USB für Controller, externe 12 V für Motoren, gemeinsame Masse. Versorgungspfad,
Regler-/Jumperbeschaltung, Rückspeisung, Stromaufnahme und Dauerthermik sind nicht
vollständig vermessen. Enable-Jumper dürfen PWM nicht überbrücken. Vor Umstecken
sämtliche Versorgung trennen. GPIO2/15 beeinflussen Bootverhalten; keine Garantie
für geänderte Beschaltung/Einschaltreihenfolgen. Historische Testfirmware verwendet
andere Vorderseite und Tasterbelegung und darf nicht ungeprüft aufgespielt werden.
