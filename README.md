# ESP32 Tank Chassis

Kettenfahrzeug mit TTGO/LILYGO T-Display, L298N, RGB-LED, Taster, Servo und BMI160.
Lernplattform für **Eingebettete Systeme – All in with KI**.

## Aktueller Stand – 8. November 2026

**Die aktuelle Firmware liegt in [`WebControl/`](WebControl/) und ist aufgespielt.**
Sie hostet die Steuerseite im WLAN **Tank-Lab** unter **http://192.168.4.1**
(Demo-Passwort: `TankLab2026`). Kein Internet nötig.

- Bisheriges Heck ist die neue Vorderseite (`Config::FrontReversed=true`), TFT um 180° gedreht.
- Acht Fahrtasten: vor/zurück, Drehen auf der Stelle und vier Kurvenrichtungen.
- Motor-PWM 50–100 %, STOPP 0 %. Kurven fahren fest mit 100 % außen / 50 % innen.
- BMI160-Messwerte, Stillstandskalibrierung und relativer Drehwinkel auf Display/Webseite.
- Zuschaltbare Geradeauskorrektur sowie relative ±90°/±180°/±360°-Drehungen.
- Vorausschauendes Abschalten, Stillstandsmessung und kurze Feinkorrekturimpulse;
  nach erfolgreicher Drehung bleibt die Freigabe erhalten. ±1° ist das Software-Zielfenster,
  keine garantierte absolute mechanische Genauigkeit.
- Start gesperrt. Taster/STOPP & SPERREN und Fehler brechen Aktionen ab.
  Browserbefehle müssen spätestens alle 500 ms erneuert werden.
- RGB- und nominelle Servo-Sollwinkelsteuerung bleiben gekapselt.

**Aktuell offen:** Nach dem letzten Upload antwortet der Sensor nicht mehr:
I²C-Suche an SDA33/SCL32 findet kein Gerät (`devices=0`). Zuvor wurde der BMI160
an `0x68` mit Chipkennung `0xD1` erkannt und lieferte gültige Werte. Sensorpins sind
unverändert. Automatische Winkelaktionen sind ohne gültigen, kalibrierten Sensor
nicht verfügbar. Die neue Vorderseite, Displayausrichtung, Kurven und 360°-Fahrt
sind gebaut und getestet, aber noch nicht vom Besitzer am Fahrzeug bestätigt.

Der Besitzer bestätigte zuvor funktionierende Sensoranzeige, Geradeauskorrektur
und die ausreichende Leistung ab 50 %. Reale Nachlauf-/Genauigkeitsabstimmung der
neuesten Drehregelung steht noch aus. Ein virtueller Joystick ist optional geplant.

## Einstieg

1. [`02-TankWebControl.code-workspace`](02-TankWebControl.code-workspace) in VS Code öffnen.
2. In PlatformIO die Umgebung **tank_webcontrol** wählen. Nur eine andere `.ini`
   anzuklicken wechselt das aktive Projekt nicht zuverlässig.
3. Bauen bzw. bei Bedarf aufspielen (Motorversorgung für den Upload ausgeschaltet):

```sh
cd WebControl
pio run
pio run -t upload --upload-port /dev/cu.usbserial-01E5F16A
pio device monitor --baud 115200 --port /dev/cu.usbserial-01E5F16A
```

Der USB-Port ist gerätespezifisch. Monitor vor Upload schließen.
Danach WLAN Tank-Lab verbinden, Seite neu laden, im Stillstand kalibrieren und erst
für einen bewussten Fahrversuch freigeben. Die Webseite erklärt die Bedienung.
USB ersetzt die externe Motorversorgung nicht.

Für das minimale Vorlesungsbeispiel stattdessen
[`01-HelloWebServer.code-workspace`](01-HelloWebServer.code-workspace) öffnen und
**hello_webserver** wählen. Dieses Beispiel ist ein eigenständiges Projekt.

## Dokumentation

- [Wiedereinstieg und offener Sensorfehler](docs/START_HERE.md)
- [Aktuelle Hardware/Pinbelegung](docs/HARDWARE.md)
- [Bedienung, Klassen, Routen und Reglerparameter](WebControl/README.md)
- [Aktueller Validierungsstand und Verlauf](WebControl/VALIDATION.md)
- [Vorlesungsablauf](docs/TEACHING.md), [Code-Erklärung](WebControl/LESSON.md)
- [Präsentation als PowerPoint und PDF](Presentation/README.md)
- [Historischer Vier-Schritte-Motortest vom 5. November](docs/archive/validation-2026-11-05.md)

## Projektstruktur

| Ordner                  | Zweck                                                                        |
| ----------------------- | ---------------------------------------------------------------------------- |
| `WebControl/`           | Aktuelle modulare Steuerfirmware, Tests und Lehrbeispiele                    |
| `Presentation/`         | Vorlesungsfolien als PPTX/PDF, älterer Hardwarestand gekennzeichnet          |
| `MotorTest/`            | Historisch bestätigter Vier-Schritte-Test mit damaliger Vorderseite/Belegung |
| `PinTest/`              | Historischer RGB-/Displaytest mit damaliger Tasterbelegung                   |
| `Tank/`, `Two wheeler/` | Ursprüngliche/historische Sketche, nicht die aktuelle Firmware               |
| `docs/archive/`         | Frühe Hardwarebefunde und Testprotokolle                                     |

Historische Testprojekte nicht ungeprüft mit der heutigen Verdrahtung verwenden:
Taster war an GPIO32, heute ist dort Sensor-SCL; Taster ist jetzt GPIO36.

## Prüfen und Formatieren

```sh
sh WebControl/tests/host/run.sh
cd WebControl
npm ci
npm run test:web
npm run format:web
pio run
```

Host- und Browserlogiktests ersetzen keine Prüfung der Mechanik, Elektrik oder
Netzwerkabbrüche am echten Fahrzeug. Genauer Stand: [VALIDATION.md](WebControl/VALIDATION.md).

PlatformIO **espressif32 6.10.0**, Arduino-ESP32 2.x, **TFT_eSPI 2.5.43**.
Anwendungs-Pins in `WebControl/include/Pins.h`, Display-Signale zusätzlich als
Buildflags in `WebControl/platformio.ini`, Fahrgrenzen in `Config.h`.

Originaler Flash lokal unter `PinTest/backups/original-2026-11-05.bin` gesichert;
Backups, Buildausgaben und node_modules sind ausgeschlossen. Wiederherstellung nicht getestet.
