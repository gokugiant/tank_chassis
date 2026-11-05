> Historische Bestandsaufnahme vor den Hardwaretests. Für den aktuellen Stand siehe die Haupt-README.

# Tank Chassis – ESP32-Lernplattform

Ein älteres Arduino/ESP32-Projekt zur Steuerung eines Kettenfahrzeugs mit zwei DC-Motoren und einem L298N-Motortreiber. Das Fahrzeug soll als Beispiel für die Vorlesung **Eingebettete Systeme – All in with KI** weiterentwickelt werden: vom lokalen Ein-/Ausgang über eine Webseite auf dem ESP32 bis zur Übertragung von Statusdaten an einen Internetserver.

## Stand der Bestandsaufnahme

Geprüft am 5. November 2026, Ausgangscommit `be637c6`. Die Aussagen zur Software stammen aus dem Quellcode; die Hardwarebeschreibung zusätzlich aus den bereitgestellten Fotos und Angaben des Besitzers. Es wurde nichts geflasht oder am Fahrzeug ausgeführt. Ein erfolgreicher Build und ein Hardwaretest stehen aus; Arduino CLI und PlatformIO sind in der untersuchten Umgebung nicht installiert.

**Der vorhandene Code steuert das Fahrzeug per Taster. Eine Netzwerksteuerung und eine Displayausgabe sind noch nicht implementiert.**

## Hardware

| Bauteil | Stand / Zuordnung |
| --- | --- |
| ESP32-Board von TTGO/LILYGO | Auf den Fotos vermutlich das klassische T-Display; genaue Boardrevision noch bestätigen |
| Integriertes Display | Vermutlich ST7789-Farb-TFT, 240 × 135 Pixel, kein OLED; gilt bei Bestätigung des klassischen T-Display |
| Zwei Ketten mit je einem DC-Motor | Differenzialantrieb: gemeinsame Richtung zum Fahren, unterschiedliche Richtung/Geschwindigkeit zum Lenken |
| L298N-Modul | Auf den Fotos erkennbar; zwei H-Brücken für die Motoren |
| Externer Taster | Im Code aktiv HIGH erwartet, externe Beschaltung prüfen |
| RGB-LED mit vier Anschlüssen | Drei Vorwiderstände sichtbar; laut Besitzer zuletzt umgesteckt. Gemeinsame Anode/Kathode und Farbzuordnung noch elektrisch prüfen |
| Servo | Mechanisch vorhanden, laut Besitzer derzeit nicht angeschlossen; alter Code initialisiert ihn trotzdem |

Die Fotos belegen weder den vollständigen Stromlauf noch die korrekte RGB-Polung. Für die LED jeden Farbkanal einzeln prüfen; die Beinchenreihenfolge ist vom Bauteil abhängig. Versorgung, gemeinsame Masse und Motortreiber-Jumper vor dem Betrieb nachvollziehen. Motoren und Servo brauchen eine geeignete Versorgung; sie dürfen nicht aus einem GPIO gespeist werden.

## Dateien

| Pfad | Bedeutung |
| --- | --- |
| `Tank/Tank.ino` | Hauptsketch für den aktuellen Aufbau |
| `Tank/Wiring.h` | GPIO-Konstanten |
| `Tank/Move.h`, `Tank/Move.cpp` | Fahrtrichtungen und unvollständige PWM-/Drehfunktionen |
| `Tank/Turret.h`, `Tank/Turret.cpp` | Servoklasse mit alter Kalibrierung |
| `Tank/TankFirmware/TankFirmware.ino` | Separater älterer Versuch mit anderer Pinbelegung; nicht zusätzlich zum Hauptsketch verwenden |
| `Two wheeler/motorshield/motorshield.ino` | Separates MotorDriver-Beispiel für anderen Aufbau |

## Pinbelegung des Hauptsketches

Quelle: `Tank/Wiring.h`. Dies ist die Softwarebelegung, kein bestätigter Verdrahtungsplan.

| Signal | ESP32 GPIO |
| --- | --- |
| Servo, derzeit nicht angeschlossen | 33 |
| Externer Taster | 32 |
| RGB Rot / Grün / Blau | 27 / 26 / 25 |
| Enable „links“ / „rechts“ laut Variablennamen | 13 / 21 |
| L298N IN1 / IN2 / IN3 / IN4 | 15 / 2 / 17 / 22 |

**Zuordnung prüfen:** `Move.cpp` verwendet IN3/IN4 für den linken Motor und IN1/IN2 für den rechten. Am L298N gehört ENA zum Kanal IN1/IN2 und ENB zum Kanal IN3/IN4. Die tatsächliche Verbindung von GPIO13/GPIO21 zu ENA/ENB muss geprüft werden; bei gemeinsamem Vollgas fällt eine vertauschte Enable-Zuordnung möglicherweise nicht auf.

GPIO2 und GPIO15 sind beim klassischen ESP32 Boot-Strapping-Pins. Ihre externe Beschaltung muss beim Start berücksichtigt werden. Eine spätere Pinänderung braucht einen Abgleich mit Board, Display und Verdrahtung.

## Aktuelles Verhalten

1. Globale Objekte initialisieren Servo und Motor-GPIOs bereits vor `setup()`.
2. `setup()` startet die serielle Ausgabe mit **9600 Baud**, konfiguriert den Taster als `INPUT`, schaltet die LED aus und setzt den Servo auf den logischen Winkel 0.
3. Bei HIGH am Taster wechselt der Zustand: **Vorwärts → Rückwärts → Stopp → …**.
4. Jeder erkannte Tastendruck erhöht zusätzlich die LED-Werte und übergibt den Zustand 0, 1 oder 2 als Servowinkel.
5. Nach jedem Wechsel blockiert das Programm für 500 ms. Ein gehaltener Taster löst danach den nächsten Wechsel aus. Loslassen stoppt die Motoren nicht.

Die Servo-Zustände 0/1/2 sind keine sinnvolle Schwenksteuerung. Die Servoklasse bildet logische 0–180° auf alte Kalibrierwerte 0–147 ab; diese Kalibrierung ist nicht als passend für den heutigen Aufbau bestätigt.

## Befunde und Modernisierungsbedarf

| Befund | Auswirkung / nächste Änderung |
| --- | --- |
| Keine Displaybibliothek oder Displayinitialisierung | Diese Firmware zeigt selbst nichts auf dem integrierten Display an |
| Kein WLAN, HTTP, WebSocket oder MQTT | Browsersteuerung und Telemetrie müssen ergänzt werden |
| PWM-Anbindung und `ledcWrite()` auskommentiert | `forward(percent)` und `backward(percent)` fahren unabhängig vom Prozentwert mit digital aktivierten Enables |
| `map(percent, 200, 255, 0, 100)` | Abbildung für eine Prozentvorgabe verkehrt herum; durch begrenzten Sollwert und korrekte PWM-Abbildung ersetzen |
| `turn90Left/Right()` setzt nur Richtungen und deaktiviert sie direkt wieder | Keine definierte Drehung; ohne Sensorik ist ein exakter Winkel ohnehin nicht sichergestellt |
| `turnLeftForMillis/turnRightForMillis()` leer | Noch keine Implementierung |
| `stop()` setzt nur Richtungseingänge LOW | Modernisierte Stoppfunktion soll zusätzlich beide Enables/PWM auf 0 setzen |
| Keine Flankenerkennung, blockierende Tasterbehandlung | Entprellung und Zustandswechsel ohne `delay()` ergänzen |
| Kein Fahrbefehl-Timeout | Bei künftiger Funksteuerung muss ausbleibende Befehlsauffrischung zum Stopp führen |
| Hardwareinitialisierung in Konstruktoren | In explizite `begin()`-Methoden nach Arduino-Start verlagern |
| Alte `ledcSetup`-/`ledcAttachPin`-API | Für Arduino-ESP32 3.x auf die aktuelle pinbezogene LEDC-API umstellen |
| Abhängigkeiten und Versionen nicht festgelegt | Reproduzierbare Buildkonfiguration ergänzen |

## Alten Sketch öffnen

In der Arduino IDE **`Tank/Tank.ino`** öffnen. Der Sketch benötigt `ESP32Servo` und eine Bibliothek mit `LedRGB.h`. Die konkrete Herkunft/Version von `LedRGB` ist im Repository nicht dokumentiert. Der alte PWM-Code setzt eine Arduino-ESP32-2.x-API voraus; mit 3.x ist eine Portierung erforderlich. Deshalb ist diese Anleitung noch keine reproduzierbare Buildanleitung. Die beiden anderen `.ino`-Dateien sind eigenständige Versuche.

## Vorschlag für die Modernisierung

### 1. Hardware und Basisfunktionen bestätigen

- Boardrevision und Displaytyp bestätigen, GPIO-Verdrahtung einschließlich ENA/ENB aufnehmen.
- Start mit deaktivierten Motoren; RGB-Kanäle, Taster und Display zunächst einzeln testen.
- Servo standardmäßig deaktivieren, solange er nicht angeschlossen ist.
- Linken/rechten Antrieb mit aufgebockten Ketten einzeln und mit kleiner Leistung prüfen; Richtung und Mindest-PWM kalibrieren.
- Feste Versionen für Toolchain und Bibliotheken festlegen und einen erfolgreichen Build dokumentieren.

### 2. Display als verständliche Statusanzeige

Vorschlag für das quer ausgerichtete Display:

```text
TANK LAB       GESPERRT
WLAN: Tank-Lab-01
IP:   192.168.4.1
Links: 0%   Rechts: 0%
Taster: Freigabe
```

SSID und Adresse sind Zielbeispiele für einen zukünftigen Access-Point-Modus, keine heutigen Einstellungen. Nach Freigabe könnten Fahrzustand, letzte Befehlszeit und Verbindung angezeigt werden. Keine Batteriespannung oder tatsächliche Geschwindigkeit anzeigen, solange dafür keine bestätigte Messung existiert. PWM-Sollwerte sind keine gemessene Geschwindigkeit.

### 3. Webseite direkt auf dem ESP32

Der ESP32 eröffnet zunächst ein eigenes WLAN, damit die Demo ohne Hochschulnetz funktioniert. Er liefert HTML/CSS/JavaScript selbst aus. Browser und Fahrzeug tauschen Status und Befehle aus.

- Zuerst ausschließlich Status, Taster und RGB-Farben im Browser zeigen.
- Danach Motorfreigabe, begrenzte Geschwindigkeit, Richtungstasten und Stopp ergänzen.
- Fahrbefehle nur während gedrückter Bedientaste regelmäßig erneuern; das Gerät stoppt bei überschrittenem Timeout, beispielsweise nach 300–500 ms ohne gültige Auffrischung. Auch Browser-Fokusverlust und Verbindungsabbruch berücksichtigen.
- Der physische Taster kann als lokale Freigabe/Sperre dienen. Das ist eine Softwarefunktion, kein unabhängiger Not-Aus.
- Für mehrere Browser eine eindeutige Steuerberechtigung vorsehen.

Mögliche HTTP-Schnittstellen: `GET /api/status`, `POST /api/led`, `POST /api/drive`, `POST /api/stop`. Diese existieren noch nicht. Zustandsänderungen gehören in POST-Anfragen mit validierten Werten. Für die erste Lehrdemo reicht periodisches Abrufen; WebSocket ist eine spätere Erweiterung.

### 4. Statusdaten auf einer Internetseite

```text
Browser im Geräte-WLAN ← HTTP → ESP32 → Motortreiber

Spätere Erweiterung:
ESP32 im Internet-WLAN → HTTPS oder MQTT/TLS → Server → Webseite
```

Zuerst nur Telemetrie übertragen: Laufzeit, Taster, LED-Farbe, Sollwerte und Verbindungsstatus. Server und Webseite können einen Verlauf darstellen. Der ESP32 baut die Verbindung ausgehend auf; eine Portfreigabe auf das Fahrzeug ist dafür nicht erforderlich. Zugangsdaten nicht in das öffentliche Repository schreiben. Fahrsteuerung aus dem Internet ist eine gesonderte Erweiterung mit Authentifizierung, Steuerberechtigung und lokalem Timeout.

## Einsatz in der Vorlesung

Die Plattform eignet sich gut als durchgängiges Anschauungsobjekt: Ein- und Ausgänge, PWM, Zustandsautomaten, Browser/Server, JSON und Kommunikation werden am selben Gerät sichtbar. Auch ohne zusätzlichen Sensor lassen sich echte Zustände wie Taster und Laufzeit übertragen. Für eine Sensor-Vorlesung müsste ein Sensor ergänzt werden.

Für etwa 90 Minuten:

| Dauer | Schritt | Ergebnis |
| --- | --- | --- |
| 15 min | Hardware, GPIOs und Motortreiber erklären | Trennung von Logiksignal und Motorleistung verstehen |
| 15 min | Taster/RGB/Display und Zustandsmodell | Lokale Interaktion nachvollziehen |
| 25 min | ESP32-Webseite und JSON-Status | Verstehen, welcher Code im Browser und auf dem ESP läuft |
| 20 min | Freigegebene Motorsteuerung und Verbindungsausfall demonstrieren | Timeout und Eingabevalidierung als reale Anforderungen erleben |
| 15 min | Internet-Telemetrie als vorbereitete Erweiterung | Unterschied zwischen Geräte-Webserver und Internetserver verstehen |

Geeignete KI-Aufgabe: „Ergänze eine Statusanzeige, die Taster, RGB-Farbe und Motor-Sollwerte zeigt. Erkläre die Kommunikationswege. Zeige beim Verlust der Verbindung einen veralteten Status an.“ Studierende prüfen die Erklärung am Code und testen absichtlich einen Verbindungsabbruch.

**Einschätzung:** Gute Ausgangshardware für die Lehre, aber die alte Firmware ist noch keine fertige Web-Demo. Zuerst Hardwarezuordnung und reproduzierbaren Build herstellen, dann Status-Webseite und erst anschließend Fahrsteuerung ergänzen.

## Herstellerquellen

- [LILYGO T-Display: Displaytyp und Boarddokumentation](https://wiki.lilygo.cc/products/t-display-series/t-display/)
- [Espressif: Migration Arduino-ESP32 2.x → 3.0, insbesondere LEDC](https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html)

Die Boardidentifikation aus den Fotos bleibt bis zur Bestätigung eine Annahme.
