# ESP32 Tank Chassis

Kettenfahrzeug mit TTGO/LILYGO T-Display, L298N, zwei DC-Motoren, externem Taster und RGB-LED. Lernplattform für **Eingebettete Systeme – All in with KI**.

## Aktueller Stand

**Die Firmware unter [`MotorTest/`](MotorTest/) ist aufgespielt und am Fahrzeug bestätigt.** Nach Korrektur der linken Fahrtrichtung und Displayrotation hat der Besitzer am 5. November 2026 bestätigt: Beide Ketten führen genau die auf dem Display angezeigten Richtungen aus; die Displayausrichtung passt.

- Vier Einzeltests per Taster: links vorwärts, links rückwärts, rechts vorwärts, rechts rückwärts.
- Jeder Test endet nach 750 ms bei PWM128/255 (ca. 50 %) automatisch.
- Display zeigt aktive Kette, Richtungspfeil, Testnummer, nächsten Schritt und Stoppzustand.
- RGB: Rot bei Stopp, Blau während des Tests. Einzelne RGB-Farben wurden zuvor bestätigt.
- Start im Stillstand; keine Wiederholung durch gehaltenen Taster.
- Servo ist deaktiviert und nicht angeschlossen.
- WLAN/Webserver sind noch nicht implementiert.

## Einstieg

1. Projekt in VS Code mit PlatformIO öffnen; **`MotorTest` als Projektordner** wählen, da dort die `platformio.ini` liegt.
2. Motorversorgung zunächst ausgeschaltet lassen. USB verbinden.
3. In `MotorTest/` bauen und bei Bedarf aufspielen:

```sh
pio run
pio run -t upload --upload-port /dev/cu.usbserial-01E5F16A
pio device monitor --baud 115200 --port /dev/cu.usbserial-01E5F16A
```

Der Port ist der im Test verwendete macOS-Port und kann auf einem anderen Rechner abweichen. Seriellen Monitor vor Upload schließen. Nach dem Upload beginnt die Folge bei Schritt 1.

4. Für Motorversuche Chassis stabil aufbocken, beide Ketten frei halten und geeignete externe Motorversorgung anschließen. Im bestätigten Aufbau wurde eine 12-V-Quelle verwendet. Finger von Ketten und Rollen fernhalten.
5. Taster einmal drücken, Bewegung und Anzeige vergleichen, automatischen Stopp abwarten, dann nächster Schritt. Ein weiterer Druck während der Bewegung stoppt vorzeitig.

Die Prüfung erfolgte mit frei drehenden Ketten. Lastbetrieb, Fahrgeschwindigkeit, Mindest-PWM und thermische Dauerbelastung sind nicht validiert. Taster und Timer sind Softwarefunktionen; Pegel während Reset/Boot und die Stromversorgung sind davon nicht abgesichert. USB dient nicht als Motorversorgung.

## Dokumentation

- [Bestätigte Hardware, Pinbelegung und Richtungen](docs/HARDWARE.md)
- [Bedienung des aktuellen Tastertests](MotorTest/README.md)
- [Testnachweis und offene Prüfungen](docs/VALIDATION.md)
- [Nächste Schritte für die Vorlesung](docs/TEACHING.md)
- [RGB-/Displaytest ohne Motorsteuerung](PinTest/README.md)

## Projektstruktur

| Ordner | Zweck |
| --- | --- |
| `MotorTest/` | Aktuelle, am Fahrzeug bestätigte Testfirmware mit PlatformIO-Konfiguration |
| `PinTest/` | Separater RGB-/Displaytest; Taster schaltet Rot → Grün → Blau, Motoren bleiben aus |
| `MotorTest/legacy/` | Alte serielle Testversion v3; wird nicht mitgebaut |
| `Tank/` | Ursprünglicher Arduino-Sketch, unverändert und mit bekannten Fehlern; nicht als aktuellen Stand flashen |
| `Two wheeler/` | Historisches Beispiel für anderen Aufbau |
| `docs/archive/` | Ursprüngliche Analyse und vorläufige Testnotizen, kein aktueller Verdrahtungsplan |

Der alte `Tank/`-Code hat vertauschte Enable-Bezeichnungen, unvollständige PWM-/Drehfunktionen und keine Display-/Netzwerksteuerung. Die funktionierende neue Implementierung liegt separat in `MotorTest/`.

## Build und lokale Sicherung

Festgelegt: PlatformIO-Plattform **espressif32 6.10.0**, Arduino-Framework aus dieser Plattform, **TFT_eSPI 2.5.43**. Das Framework verwendet Arduino-ESP32 2.x; eine spätere 3.x-Portierung erfordert eine LEDC-Anpassung. Displayeinstellungen liegen projektlokal in `platformio.ini`.

Vor dem ersten Flash wurde der ursprüngliche 4-MiB-Flash lokal unter `PinTest/backups/original-2026-11-05.bin` gesichert. Backups und Buildausgaben werden von Git ausgeschlossen. Die Sicherung ist kein Bestandteil des Repositorys und ihre Wiederherstellung wurde nicht getestet.
