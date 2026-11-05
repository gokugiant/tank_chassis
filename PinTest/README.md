# Display- und RGB-Test v2

Eigenständige Testfirmware für das klassische TTGO T-Display. Der alte Sketch unter `Tank/` bleibt erhalten.

## Bedienung

- Start mit Rot auf RGB-LED und Display.
- Externer Taster GPIO32: pro Druck **Rot → Grün → Blau → Rot**. 40 ms Entprellung; Halten schaltet nicht wiederholt weiter.
- Display: drei farbige Ampelkreise, aktive Farbe mit weißem Rand, Farbname, Klickzähler und „MOTOREN AUS“.
- Alle Motorleitungen GPIO13/21/15/2/17/22 bleiben LOW; diese Version enthält **keine Motorfreigabe und keine Fahrbefehle**. Servo GPIO33 bleibt Eingang.
- Keine WLAN-Funktion. Die Bezeichnung „MOTOREN AUS“ beschreibt die Softwareausgänge, keine gemessene Motorbewegung.

Serielle Konsole 115200 Baud, Befehle mit Zeilenende:

| Befehl | Wirkung |
| --- | --- |
| `red`, `green`, `blue` | RGB und Display synchron umschalten |
| `off` | Alle Farbkanäle aus, Display zeigt AUS |
| `status` | Farbe, Taster-Rohwert und Klickzähler |
| `stop` | Alle Motorleitungen LOW |
| `anode`, `cathode` | LED-Polarität wählen und Farbe ausschalten; Standard gemeinsame Kathode |

Die serielle Verbindung funktioniert zum Senden; Antworten wurden zuletzt nicht bei jedem Öffnen empfangen. Sichtkontrolle von Display und LED bleibt erforderlich.

## Displaykonfiguration

ST7789, 135 × 240 Pixel, quer mit `setRotation(1)`. SPI MOSI19/SCLK18, CS5, DC16, Reset23, Backlight4. Konfiguration aus [TFT_eSPI Setup25 für TTGO T-Display](https://github.com/Bodmer/TFT_eSPI/blob/master/User_Setups/Setup25_TTGO_T_Display.h). Diese Pins überschneiden sich nicht mit den verwendeten Motor-, RGB- und Tasterpins. Exakte Boardvariante weiterhin anhand des erfolgreichen Displaytests bestätigen.

## Bauen und Aufspielen

`pio run` in diesem Ordner. Aufspielen: `pio run -t upload --upload-port <Port>`.

Festgelegt: Plattform espressif32 6.10.0 (Arduino-ESP32 2.x), TFT_eSPI 2.5.43. Keine globale Änderung einer installierten TFT-Bibliothek nötig; Displayeinstellungen stehen in `platformio.ini`.

## Hardwarestand

Der Besitzer hat Rot (GPIO27), Grün (GPIO26) und Blau (GPIO25) einzeln bestätigt. Button laut Besitzer funktionsfähig; Ruhepegel war LOW. Die neue Flankenerkennung setzt aktiv HIGH und die bestehende externe Beschaltung voraus. Display, synchroner Farbwechsel und Tasterbedienung wurden anschließend vom Besitzer als funktionierend bestätigt. Diese ältere Demo nutzt Rotation1; für die endgültige Montage ist im aktuellen MotorTest Rotation0 bestätigt.

Originaler 4-MiB-Flash vor der ersten Testfirmware lokal unter `backups/original-2026-11-05.bin` gesichert und von Git ausgeschlossen. Die v1-Testfirmware wurde erfolgreich aufgebaut und aufgespielt. In v2 wurden deren Motorimpuls-Befehle entfernt.

Software kann während Boot/Reset keine sicheren Pegel garantieren. Versorgung und Treiberverdrahtung separat prüfen; USB ist keine geeignete Motorversorgung.
