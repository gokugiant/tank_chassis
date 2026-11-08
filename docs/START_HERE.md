# Wiedereinstieg – 8. November 2026

## Zuerst lesen

Die aktive Firmware ist **WebControl**, zuletzt mit neuer Vorderseite, TFT-Rotation2,
Kurvenpad und 360°-Option erfolgreich hochgeladen. Keine Bewegung beim Upload ausgelöst.
Repository: `gokugiant/tank_chassis`, Branch `master`.

**Vorrangig offen: BMI160 antwortet nicht mehr.** Nach dem letzten Upload wurde auch
nach Neustart `I2C SCAN SDA=33 SCL=32 levels=1,1` und `devices=0` gelesen.
Zuvor: Adresse `0x68`, Register `0x00` = `0xD1`, gültige Messwerte. Der Sensor ist
BMI160, auch wenn auf der Platine „160 / 6DS3“ steht. Nicht blind wieder auf einen
LSM6DS3-Treiber oder andere Versorgung wechseln.

1. Arbeitsbaum, USB-Port und bestehende Firmware prüfen; keinen Bewegungstest automatisch starten.
2. Versorgung/Steckkontakte des Sensors prüfen, unveränderte Pins SDA33/SCL32.
   Versorgung wurde zuvor direkt über 3V3 erfolgreich betrieben; VIN war unbeschaltet.
3. Vor Umstecken sämtliche Versorgung trennen. Keine Motorversorgung über USB.
4. Nach wiederhergestellter Verbindung Seite neu laden, Tank flach/still halten und
   zwei Sekunden kalibrieren. Nase von oben nach links entspricht positivem Winkel.
5. Neue Vorderseite/Display zunächst prüfen; danach einzelne kontrollierte Kurven-/Drehversuche.
   Nachlauf-/Trimparameter sind noch nicht am Fahrzeug abgestimmt.

## Geltende Belegung und Verhalten

- Neue linke Kette: Enable13, vorwärts2, rückwärts15.
- Neue rechte Kette: Enable21, vorwärts22, rückwärts17.
- Taster36 aktiv HIGH mit vorhandenem externen Pulldown; RGB27/26/25; Servo12.
- Displayrotation2, SPI-Signale unverändert. Sensor33/32.
- Laufende Ketten immer 50–100 % PWM; 0 ist STOPP. Diagonale Kurven fest 100/50.
- 90/180/360 links/rechts: früher abschalten, ausrollen, Stillstand messen,
  höchstens sechs kurze Korrekturimpulse. Software-Zielfenster ±1°.
- Erfolg behält die Fahrfreigabe; Fehler und STOPP & SPERREN sperren.
- 500-ms-Browserfrist bleibt unabhängig von internen Regelschritten im Motortimer erhalten.
- Joystick noch nicht implementiert. Mechanische Servomitte nicht verändert.

## Orientierung im Code

| Datei/Klasse                                  | Aufgabe                                                             |
| --------------------------------------------- | ------------------------------------------------------------------- |
| `include/Config.h`, `include/Pins.h`          | Parameter, Vorderseite und Belegung                                 |
| `TankDrive`                                   | Ketten, PWM-Grenzen, Freigabe und unabhängiger Watchdog             |
| `MotionSensor`, `I2cDiagnostics`              | Chip-ID, Messwerte, I²C-Diagnose                                    |
| `Heading`                                     | Stillstandskalibrierung, relativer Winkel, geglätteter Kippverdacht |
| `Navigation`                                  | Geradeaus-PD-Regelung, Drehphasen und begrenzte Feinkorrektur       |
| `TankWebServer`, `WebPage.h`                  | HTTP-Routen und im Browser laufendes JavaScript                     |
| `RgbLed`, `Button`, `Turret`, `StatusDisplay` | Sonstige Hardware und Anzeige                                       |

Pfade sind relativ zu `WebControl/`. Tests: `sh WebControl/tests/host/run.sh`,
`cd WebControl && npm run test:web`, `pio run`. Logiktests sind keine reale Fahrvalidierung.
Aktueller Verlauf und genaue Nachweise: [WebControl/VALIDATION.md](../WebControl/VALIDATION.md).
