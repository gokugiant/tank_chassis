# Hardware und bestätigte Pinbelegung

Stand: 5. November 2026. Quellen: vorhandener Code, Fotos, Angaben des Besitzers und abschließender Vier-Schritte-Test. Links/rechts immer aus Sicht des Fahrzeugs in Fahrtrichtung.

## Controller und Display

ESP32-D0WDQ6 Revision 1.0 vom Flashwerkzeug erkannt. TTGO/LILYGO T-Display mit erfolgreich getesteter ST7789-Konfiguration; genaue PCB-Revision nicht abgelesen. TFT-Farbdisplay, kein OLED. Native Auflösung 135 × 240, im bestätigten Aufbau Hochformat mit **`setRotation(0)`**. Die obere Displaykante entspricht der Fahrzeugfront. Rotation 1 war aus Besitzersicht um 90° nach rechts verdreht.

| Displaysignal | GPIO |
| --- | ---: |
| MOSI | 19 |
| SCLK | 18 |
| CS | 5 |
| DC | 16 |
| Reset | 23 |
| Hintergrundbeleuchtung, aktiv HIGH | 4 |

SPI 40 MHz. Konfiguration nach [TFT_eSPI Setup25](https://github.com/Bodmer/TFT_eSPI/blob/master/User_Setups/Setup25_TTGO_T_Display.h), im Projekt per Buildflags festgelegt.

## L298N und Motoren

| L298N | Kabelfarbe laut Fotos | ESP32-GPIO | Physischer Kanal |
| --- | --- | ---: | --- |
| ENA | Braun | 13 | Rechts, PWM-Kanal 6 |
| IN1 | Rot | 15 | Rechts Richtung 1 |
| IN2 | Orange | 2 | Rechts Richtung 2 |
| IN3 | Gelb | 17 | Links Richtung 1 |
| IN4 | Grün | 22 | Links Richtung 2 |
| ENB | Blau | 21 | Links, PWM-Kanal 7 |

Braun13 und Rot15 wurden ausdrücklich vom Besitzer bestätigt; die übrigen Farbzuordnungen wurden anhand der Fotos und des ursprünglichen Codes angenommen. Die vollständige GPIO-/Motorfunktion ist durch den abschließenden Test bestätigt; ein Durchgangsmessprotokoll liegt nicht vor.

| Bewegung | Aktives Enable | Eingang HIGH | Eingang LOW |
| --- | ---: | ---: | ---: |
| Links vorwärts | 21 | 17 | 22 |
| Links rückwärts | 21 | 22 | 17 |
| Rechts vorwärts | 13 | 15 | 2 |
| Rechts rückwärts | 13 | 2 | 15 |

Beim Einzeltest bleibt der andere Enable auf PWM0 und alle anderen Richtungseingänge LOW. Stopp setzt beide PWM-Kanäle auf 0 und alle Motorleitungen LOW. PWM: 1 kHz, 8 Bit, Testwert 128/255. 50 % PWM ist ein elektrischer Sollwert, keine gemessene halbe Fahrgeschwindigkeit.

## Taster, LED und Servo

| Funktion | GPIO | Bestätigtes Verhalten |
| --- | ---: | --- |
| Externer Taster | 32 | Ruhe LOW, gedrückt HIGH, externe Beschaltung; `INPUT`, 40 ms Entprellung |
| RGB Rot | 27 | einzeln sichtbar bestätigt |
| RGB Grün | 26 | einzeln sichtbar bestätigt |
| RGB Blau | 25 | einzeln sichtbar bestätigt |
| Servo | 33 | nicht angeschlossen, Eingang; kein Servo-PWM |

RGB wird aktiv HIGH betrieben (entspricht gemeinsamer Kathode); einzelne Farbkanäle und synchroner Displaywechsel wurden geprüft. Vorwiderstände sind auf Fotos vorhanden, ihre Werte wurden nicht gemessen.

## Versorgung und Grenzen

USB für ESP32; externe 12-V-Quelle am L298N während des Motortests. Gemeinsame Masse ist für Logiksignale erforderlich. Vollständiger Stromlauf, Regler-/Jumperbeschaltung und Rückspeisung sind nicht elektrisch vermessen. ENA/ENB dürfen für PWM-Ansteuerung nicht gleichzeitig per Brücke an die Logikversorgung gelegt sein. Reglerjumper nicht mit Enable-Jumpern verwechseln.

GPIO2 und GPIO15 beeinflussen beim klassischen ESP32 den Bootmodus. Funktion nach dem Flash wurde geprüft; keine Garantie für beliebige Einschaltreihenfolgen oder geänderte Beschaltung. Vor Umstecken sämtliche Versorgung trennen.
