# Tank WebControl – vom GPIO zur Webseite

Lehrbeispiel für den bestätigten TTGO-Tank. Alle Hardwarezugriffe sind gekapselt, Webrouten rufen lesbare Methoden auf. Der bisherige Hardwaretest bleibt unter `../MotorTest/` erhalten.

## Start

Projektordner **WebControl** in PlatformIO öffnen:

```sh
pio run
pio run -t upload --upload-port /dev/cu.usbserial-01E5F16A
```

Seriellen Monitor vor Upload schließen. WLAN **Tank-Lab**, Demo-Passwort **TankLab2026**, danach **http://192.168.4.1** öffnen. Das WLAN liefert kein Internet. SSID und Passwort stehen in `include/Config.h`. Display zeigt statt der Tank-Lab-Überschrift den Turret-Sollwinkel mit Richtungspfeil (Draufsicht: 90° nach vorn, größere Winkel nach rechts; reale Einbaurichtung noch prüfen). Orange bedeutet aktives Servosignal, Cyan abgeschaltetes Signal. Außerdem zeigt es IP, Freigabe, Sollwerte, Tasterzähler und die IMU-Messwerte im Hochformat; aktuell ist Rotation2 eingestellt, die letzte Sichtprüfung steht noch aus.

Die Firmware startet gesperrt. **Fahren freigeben**, danach Richtungstaste gedrückt halten. Loslassen stoppt. Pfeile links/rechts drehen auf der Stelle (Ketten gegensinnig). Die PWM-Obergrenze steht in `Config.h` und beträgt aktuell 100 %, Standard im Browser 50 %. Das ist keine gemessene Geschwindigkeit.

Licht lässt sich unabhängig von der Motorfreigabe einstellen: Rot/Grün/Blau, Farbwähler oder Aus. Der echte Taster sperrt die Motoren und wechselt lokal die LED-Farbe. Die Webseite zeigt Zustand und Druckzähler; sie simuliert keinen physischen Tastendruck.

Der Webregler **Turret** setzt den Sollwinkel beim Loslassen; **Mitte** sendet 90°. Die Klasse bietet `turret.setAngle(90)`, `turret.center()`, `turret.stop()` und `turret.update()`. Grenzen stehen in `Turret.h`. GPIO 12 ist ein ESP32-Strapping-Pin: Der Servoanschluss darf ihn beim Reset nicht auf HIGH ziehen.

## Dateien und Verantwortung

| Datei                                              | Aufgabe                                                                             |
| -------------------------------------------------- | ----------------------------------------------------------------------------------- |
| `include/Pins.h`                                   | Bestätigte Pinbelegung für Motoren, RGB, Taster und Servo                           |
| `include/Config.h`                                 | WLAN, PWM-Grenze, Zeitgrenzen                                                       |
| `include/TankDrive.h`, `src/TankDrive.cpp`         | `forward`, `backward`, `turnLeft`, `turnRight`, `stop`, `arm`; GPIO/PWM und Timeout |
| `include/RgbLed.h`, `src/RgbLed.cpp`               | `red`, `green`, `blue`, `off`, `setColor(0..255,0..255,0..255)`                     |
| `include/Button.h`, `src/Button.cpp`               | `update`, `isPressed`, `wasPressed`, `pressCount`; Entprellung                      |
| `include/StatusDisplay.h`, `src/StatusDisplay.cpp` | Displaydarstellung                                                                  |
| **`src/TankWebServer.cpp`**                        | **WLAN starten, Routen anmelden, HTTP-Antworten**                                   |
| `include/WebPage.h`                                | HTML/CSS/JavaScript, als Text im ESP32-Flash gespeichert                            |
| `src/main.cpp`                                     | Objekte initialisieren und regelmäßig aktualisieren                                 |

Für TFT_eSPI müssen Displaypins zusätzlich in `platformio.ini` als Buildflags vorliegen. Die Anwendungs-Pinbelegung wird in `Pins.h` gepflegt. Display-Pins bei Änderungen in beiden Dateien abgleichen.

## Unterricht

1. [Minimaler Webserver](examples/01_HelloWebServer/src/main.cpp): nur WLAN, zwei Routen und `handleClient()`, Motoren aus. Eigenständiges PlatformIO-Projekt in `examples/01_HelloWebServer/`, Display nicht angesteuert.
2. [Schrittweise Erklärung](LESSON.md): Browser und ESP32, HTML und JSON, Route zur LED und zur Motorfunktion.
3. Vollständige Anwendung: `TankWebServer.cpp` und `WebPage.h` gemeinsam öffnen.

## HTTP-Schnittstellen

| Methode | Route         | Parameter (Formulardaten) / Ergebnis                                           |
| ------- | ------------- | ------------------------------------------------------------------------------ |
| GET     | `/`           | Webseite als HTML                                                              |
| GET     | `/api/status` | JSON: Freigabe, linke/rechte PWM-Sollwerte, RGB, Taster, Druckzähler, Laufzeit |
| POST    | `/api/turret` | `angle=0..180`; unabhängig von Fahrfreigabe, bei gedrücktem Taster gesperrt    |
| POST    | `/api/led`    | `red`, `green`, `blue`, jeweils 0–255                                          |
| POST    | `/api/arm`    | `enabled=1`, `client`, `seq`; oder `enabled=0` zum globalen Stoppen/Sperren    |
| POST    | `/api/drive`  | `direction=forward/backward/left/right`, `speed=0..100`, `client`, `seq`       |
| POST    | `/api/stop`   | `client`, `seq`; Stopp bei bleibender Freigabe                                 |

Alle POST-Anfragen brauchen Header `X-Tank-Control: 1`. Parameter werden streng geprüft, Zustandsänderungen nicht per GET angeboten, kein CORS. Beispiel für RGB:

```sh
curl -X POST http://192.168.4.1/api/led \
  -H 'X-Tank-Control: 1' -d 'red=0&green=255&blue=0'
```

`client` ist eine zufällige Browserkennung, keine Benutzeranmeldung. Eine aktive Freigabe gehört genau diesem Browser. Jeder Browser darf global sperren. `seq` steigt pro Fahr-/Stoppbefehl: verspätete alte Anfragen können einen neueren Stopp nicht überschreiben. Für eine Vorlesung zuerst LED und JSON erklären; Besitz-/Sequenzprüfung ist der zusätzliche Schutz für die bewegte Hardware.

## Stoppverhalten

- Start ohne Freigabe. Ohne Freigabe kein Fahrbefehl.
- Browser erneuert Fahrbefehle während des Haltens etwa alle 150 ms.
- Keine Erneuerung innerhalb 500 ms: separater ESP-Timer stoppt und sperrt, Prüfung alle 20 ms. Das funktioniert auch bei blockierendem HTTP/Display, solange der Timer-Task läuft. Kein harter Echtzeitnachweis.
- Loslassen, Pointerabbruch: Stoppanfrage. Fenster-/Tabwechsel und STOPP-Schaltfläche: sperren.
- Leerlauf-Freigabe verfällt nach 30 Sekunden. Neuer Start erfordert erneut Freigabe.
- Taster meldet Stopps im Hauptprogramm; während blockierendem HTTP ist seine Reaktion verzögert. Keine unabhängige Not-Aus-Funktion.
- Turret: GPIO 12, Positionsservo mit 50 Hz, nominell 0–180° (90° Mitte). Kein Signal beim Start; nach einem Befehl nach 1 s Signal aus. STOPP & SPERREN und der Taster schalten auch das Servosignal aus. Die Abschaltung läuft in `loop()` und kann bei blockierenden Aufrufen verzögert sein. Ohne Signal keine aktive Haltekraft. Mechanische Mitte, Drehrichtung und Versorgung sind noch am Fahrzeug zu prüfen.
- Zunächst mit frei hängenden Ketten prüfen. USB ist keine Motorversorgung.

Nur für ein lokales Lehr-WLAN. Das Demo-Passwort ist öffentlich im Quellcode. Kein Internet-Fernzugriff, keine Benutzerverwaltung und keine Produktionssicherheitszusage.

## Prüfen

```sh
sh tests/host/run.sh
pio run
```

Hosttests prüfen echte Hardwareklassen mit nachgebildeten GPIOs: bestätigte Richtungen, PWM-Grenze, Stopp, Zeitablauf und millis-Überlauf, Tasterentprellung, RGB und Turret (Grenzen, PWM, Abschaltung und Zeitüberlauf). Sie ersetzen keine elektrischen Messungen oder Tests am Fahrzeug. Den tatsächlichen Validierungsstand dokumentiert `VALIDATION.md`.

Quellen: [Espressif WLAN-API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html), [WebServer-Herstellerbeispiel für Arduino-ESP32 2.0.17](https://github.com/espressif/arduino-esp32/blob/2.0.17/libraries/WebServer/examples/HelloServer/HelloServer.ino).

## Code formatieren

C++ und Header verwenden **clang-format**, HTML/CSS/JavaScript **Prettier**. Prettier formatiert C++ nicht. Regeln liegen in `.clang-format` und `.prettierrc.json`.

VS Code: Den Ordner `WebControl` öffnen. Die lokale `.vscode/settings.json` wählt für C++ die Microsoft-C/C++-Erweiterung (`ms-vscode.cpptools`) und aktiviert Formatieren beim Speichern. Für eigenständige HTML-/JS-Dateien wird die Prettier-Erweiterung verwendet. Falls nötig: „Format Document With…“ → „C/C++“. Tastenkürzel unter macOS: **Shift + Option + F**. Die `.vscode`-Einstellungen sind lokal und durch die Repository-Regeln von Git ausgeschlossen.

Die Webseite liegt als HTML-Raw-String in `include/WebPage.h`. Prettier erkennt den Inhalt eines C++-Headers nicht automatisch. Dafür gibt es den kleinen Helfer:

```sh
npm ci
npm run format:web
```

Er formatiert ausschließlich HTML, CSS und JavaScript innerhalb der Zeichenkette. Dokumentation formatieren: `npm run format:docs`. Node/Prettier sind nur Entwicklungswerkzeuge; zum Firmwarebuild und auf dem ESP32 werden sie nicht benötigt.

## Wichtig: In VS Code das richtige Projekt auswählen

Nur die `platformio.ini` einer anderen Anwendung anzuklicken wechselt nicht zuverlässig das aktive PlatformIO-Projekt. Die unteren Build-/Upload-Symbole verwenden die aktive Projektauswahl.

Für die Vorlesung liegen im Repository zwei getrennte Workspace-Dateien:

- `01-HelloWebServer.code-workspace`: ausschließlich das Minimalbeispiel, Umgebung `hello_webserver`.
- `02-TankWebControl.code-workspace`: vollständige Tank-Websteuerung, Umgebung `tank_webcontrol`.

Die passende Datei über **Datei → Arbeitsbereich aus Datei öffnen…** in VS Code öffnen. Alternativ unter **PlatformIO → Project Tasks** die Aufgabe ausdrücklich unter dem gewünschten Projekt wählen. Beim Hello-Beispiel muss die Buildausgabe `Processing hello_webserver` enthalten, beim vollständigen Tank `Processing tank_webcontrol`. Das Hello-Beispiel enthält keine Ampel-/Displayansteuerung. Ein zuvor angezeigtes Bild kann ohne neue Initialisierung noch sichtbar bleiben; die Webseite `/` muss dann „Hallo vom Tank!“ statt der Tank-Steuerung zeigen.

## Bewegungssensor BMI160 (6. November 2026)

Taster von GPIO32 auf **GPIO36** umstecken; vorhandenen externen Pulldown behalten
(GPIO36 hat keinen internen). RGB und Servo bleiben unverändert.

| Sensor                         | ESP32  |
| ------------------------------ | ------ |
| 3V3                            | 3V3    |
| GND                            | GND    |
| SDA                            | GPIO33 |
| SCL                            | GPIO32 |
| CS                             | 3V3    |
| SA0                            | GND    |
| VIN, INT1, INT2, SCX, SDX, OCS | frei   |

`MotionSensor` erkennt den am Fahrzeug bestätigten **BMI160** (Adresse 0x68,
CHIP_ID=0xD1) sowie LSM6DS3 an deren jeweiligen Adressen und Chipkennungen.
Die Platinenbeschriftung „160 / 6DS3“ allein identifiziert den Chip nicht.
BMI160: I²C mit 100 kHz, 100-Hz-Sensorbetrieb (±2 g / ±250 °/s),
16384 LSB/g und 131,2 LSB/(°/s). Die PMU startet nacheinander, ohne blockierende
Warteschleife: Accel-Kommando, 5 ms warten, Gyro-Kommando, 100 ms warten, PMU prüfen.
LSM6DS3 bleibt als alternative Bestückung unterstützt (104 Hz).
Abfrage alle 20 ms, Wiederverbindung alle 3 s; alte Daten nach 300 ms ungültig.
Das Display zeigt die sechs Achsenwerte; die Webseite bekommt sie über `imu` in
`GET /api/status` mit `model`, `connected`, `valid`, `address`, `chipId`, `ageMs`, `ax/ay/az` (g)
und `gx/gy/gz` (°/s). Ungültige Werte sind JSON `null` und werden als fehlend angezeigt.
Messwerte enthalten Schwerkraft bzw. Gyro-Nullpunktfehler; keine Kalibrierung,
kein Kompasswinkel und keine Position. Die Motorregelung liegt separat in `Navigation`.
Bei flacher, ruhender Platine sollte die Beschleunigungsnorm ungefähr 1 g sein;
Drehraten sollten nahe null liegen. Achsen und Vorzeichen gelten für die Platine.

Quelle: [ST LSM6DS3-Datenblatt](https://docs.arduino.cc/resources/datasheets/LSM6DS3-datasheet.pdf).

BMI160-Quelle: [Bosch-Datenblatt](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi160-ds000.pdf).
Einmalige Adress-/ID-Diagnose beim Start liegt separat in `I2cDiagnostics.cpp`.

## Relativer Drehwinkel: erster Schritt zur Spurstabilisierung

`Heading` integriert die Z-Drehrate nach Nullpunktabgleich mit der Trapezregel.
Kein Kompass und keine Positionsmessung. Grundlage der unten beschriebenen Richtungsregelung.
Der Sensor muss flach mit Z nach oben montiert sein. Vorzeichen am Fahrzeug prüfen.

1. STOPP & SPERREN, Servo ohne Signal, Tank auf ebener Fläche still halten.
2. **Stillstand kalibrieren (2 s)**: mindestens 80 neue Messwerte, Schwerkraft-/Lage-
   und Drehratenprüfung. Gleichmäßige langsame Handdrehung kann damit nicht sicher
   als Bewegung erkannt werden; während des Abgleichs wirklich still halten.
3. Webseite/Display zeigen den relativen Winkel, die Webseite außerdem die
   korrigierte Drehrate. Die ursprüngliche IMU-Tabelle zeigt weiter Rohwerte.
4. Tank von Hand etwa 90° nach links drehen und Vorzeichen/Größenordnung prüfen.
   Nutzer bestätigt: Nase nach links ergibt +90°. Links ist damit positiv.

Schreibende Routen (Header X-Tank-Control: 1): `POST /api/heading/calibrate`,
`POST /api/heading/zero`. Nur bei gesperrtem Antrieb und ohne aktives Servosignal.
Während des Abgleichs keine Fahrfreigabe oder Servoaktion.
`GET /api/status` liefert `heading`: ready, calibrating, angle, rate, bias, status.
Bei Sensorausfall, Messlücke >150 ms oder deutlichem Kippen wird der Winkel
ungültig und muss neu kalibriert werden. Kein automatischer Bewegungsstart.

## Richtung halten und 90°-/180°-/360°-Drehungen

`Navigation` trennt die Fahrzeugregelung von Sensor, HTTP und GPIO. Regler-Startwerte
stehen in `Config.h`; die reale Genauigkeit und das Anlauf-PWM müssen am Boden
abgestimmt werden. Kein automatischer Start beim Einschalten oder Kalibrieren.

- Nach jedem Neustart im Stillstand kalibrieren, dann Fahren freigeben.
- **Richtung bei Vor/Zurück halten** ist standardmäßig aus. Bei Aktivierung wird
  beim ersten gehaltenen Vor-/Zurück-Befehl der aktuelle Winkel gespeichert.
  PD-Regelung: Korrektur = Kp * Winkelfehler - Kd * Drehrate; links = Basis -
  Korrektur, rechts = Basis + Korrektur. Maximal 12 PWM-Punkte, bei niedriger
  Geschwindigkeit höchstens halber Basisbetrag (keine Kettenumkehr).
  An den Grenzen wird der gemeinsame PWM-Anteil verschoben, sodass beide
  laufenden Ketten im Bereich 50–100 % bleiben und ihre Differenz erhalten bleibt.
- Links-/Rechts-Fahrtasten bleiben manuell; Loslassen stoppt die Regelung.
- **↶/↷ 90°/180°** startet per Klick eine relative Drehung, nur bei Freigabe,
  kalibriertem/frischem Winkel und stillstehenden Ketten. Links ist positiv.
  Gegenläufige Ketten, 50–100 % PWM, begrenzt durch den PWM-Regler, vor dem Ziel langsamer, jedoch nie unter 50 %. Bei gewählten 50 % bleibt die Dreh-PWM daher konstant bis zum Stopp. Stopp beim Eintritt
  vorausschauend nach Drehrate (anfangs 80 ms geschätzter Nachlauf), dann
  Stillstand abwarten und Restfehler durch kurze 50-%-Impulse vor/zurück korrigieren.
  Erfolg erst bei gemessenem Stillstand innerhalb ±1°; Freigabe bleibt bestehen.
  ±1° ist ein Software-Zielfenster, keine gemessene/garantierte absolute Genauigkeit.
- Browser sendet während der Drehung alle 150 ms einen Heartbeat. Kein Heartbeat
  seit 500 ms, ungültiger/über 100 ms alter Winkel, falsche Drehrichtung (>5°),
  fehlender Fortschritt (<2° in 1,5 s), Gesamtdauer 12 s: Stopp und Sperren.
  Geradeausregelung bricht auch bei mehr als 20° Abweichung ab.
- Der unabhängige Antriebstimer verwendet weiterhin den Zeitpunkt des letzten
  Browserbefehls; lokale Regelschritte verlängern diese Frist ausdrücklich nicht.
  Physischer Taster und STOPP & SPERREN brechen jede Aktion ab. Tabwechsel,
  Fokusverlust, Seitenende und Pollingfehler stoppen Browser-Heartbeats.

Routen: `POST /api/drive` zusätzlich `stabilize=0/1`;
`POST /api/turn` mit `direction=left/right`, `degrees=90/180/360`, `speed=50..100` (ohne speed: 50);
`POST /api/turn/heartbeat`. Alle mit X-Tank-Control:1, Controller-ID und streng
steigender `seq` wie die bisherigen Fahrbefehle. Status enthält `navigation`
(mode/target/error/status). GET-Statusabfragen verlängern keine Bewegungsfrist.

Erster physischer Drehversuch: niedrige feste PWM, freie Fläche auf dem Boden,
STOPP griffbereit. Aufgebockt kann der Tank seine Nase nicht drehen; dann ist
"Keine Drehbewegung erkannt" nach 1,5 s das erwartete Verhalten. Zunächst nur 90°
testen und gemessenen Endwinkel/Überschwingen berichten, erst danach 180°.

### Kippverdacht bei Fahrbeschleunigung

Die alte Einzelwertprüfung (|aX/aY| >0,4 g oder aZ <0,7 g) wurde ersetzt:
Beschleunigungsrichtung mit 250-ms-Zeitkonstante glätten; nur Messungen mit
Gesamtbetrag 0,8–1,2 g als Lagehinweis verwenden. Erst bei mehr als 50°
Kippverdacht über mindestens 300 ms wird der Winkel verworfen. Die Reaktionszeit
enthält zusätzlich die Filterverzögerung. Die strenge Stillstandskalibrierung
bleibt bestehen. Fahrbeschleunigung und Schwerkraft lassen sich mit diesem
Beschleunigungsfilter allein nicht eindeutig trennen; dies ist eine heuristische
Kippprüfung, keine bestätigte absolute Neigungsmessung während der Fahrt.

Grundlage: [Analog Devices AN-1057](https://www.analog.com/en/resources/app-notes/an-1057.html).

### Mindestleistung nach Fahrzeugtest (7. November 2026)

Nutzer: Unter 50 % haben die Motoren zu wenig Anlaufkraft; 90°-Drehungen mit
22–35 % kommen nicht zuverlässig zustande. `Config::MinSpeed=50`, MaxSpeed=100.
Der Webregler beginnt bei 50 % und begrenzt auch die automatische Drehung.
Vor dem Ziel wird von der gewählten Obergrenze höchstens bis 50 % reduziert;
bei 50 % gibt es keine weitere PWM-Absenkung, nur den Stopp am Ziel.
API lehnt Fahrwerte 1–49 ab (0 bleibt STOPP). Die Hardwareklasse begrenzt jeden
nichtnulligen Kettenwert zusätzlich auf mindestens 50 % und höchstens 100 %.
Die Gegenläufigkeit beider Ketten bei Drehung ist im Code und im Hosttest geprüft.
Physische Drehgenauigkeit und Nachlauf sind mit der höheren Leistung erneut zu prüfen.

### Präzisere Drehungen / Nachlauf (8. November 2026)

Der bisherige Stop deaktiviert Enable (L298: freies Ausrollen). Deshalb kann der
Tank auch nach Abschaltung weiterdrehen. Siehe [ST L298-Datenblatt](https://www.st.com/resource/en/datasheet/l298.pdf).
Die neue Drehung arbeitet in drei Phasen:

1. Annähern mit der bisherigen winkelabhängigen PWM (mindestens 50 %), Abschaltpunkt
   zusätzlich um Drehrate * TurnCoastSeconds vorziehen (Startwert 0,08 s, max. 12°).
2. Motoren aus, mindestens 250 ms warten und Drehrate mindestens 160 ms unter
   2,5 °/s beobachten. Kein Richtungswechsel während noch erheblicher Drehung.
3. Restfehler außerhalb ±1° durch kurze 50-%-Impulse in die benötigte Richtung
   korrigieren (20–60 ms), danach erneut ausrollen/messen. Höchstens sechs Impulse;
   Gesamtdauer weiterhin 12 s. Ausbleibender Stillstand oder Nichterreichen des
   Zielbereichs führt zu Fehler/Stopp/Sperren.

Die Impulszeiten sind Sollzeiten der loop()-Regelung. I/O-Verzögerungen können sie
verlängern; der unabhängige Browser-Watchdog bleibt aktiv. Nachlaufkonstante,
Impulsdauer und Toleranz stehen in Config.h und müssen auf dem echten Untergrund
abgestimmt werden. Die Beschränkung 50–100 % bleibt erhalten, STOPP weiterhin 0 %.
Keine aktive elektrische L298-Bremse hinzugefügt; Regler beurteilt den gemessenen Nachlauf.

Nur erfolgreicher Abschluss behält die Fahrfreigabe, sodass die nächste Drehung
per Klick starten kann. Fehler, physischer Stopptaster und STOPP & SPERREN sperren
weiterhin. Die bestehende Leerlauffreigabe läuft nach 30 s aus. Verspätete
Heartbeats nach erfolgreichem Abschluss sind idempotent und starten keine Bewegung.

## Neue Fahrzeugvorderseite, Kurven und 360° (8. November 2026)

`Config::FrontReversed=true`: Das bisherige Heck ist die neue Nase. Die Verkabelung
bleibt unverändert. In Pins.h werden logische Seiten getauscht und Richtungen
umgekehrt: neue linke Kette Enable13/Vorwärts2/Rückwärts15; neue rechte Kette
Enable21/Vorwärts22/Rückwärts17. Positive Z-Gier bleibt links; die Winkelregelung
braucht kein umgekehrtes Gyrovorzeichen. Historische Pin-/Motor-Testprojekte
behalten ihre damalige Orientierung; diese Einstellung gilt für WebControl.

TFT-Rotation ist jetzt 2 statt 0 (weiter 135x240). Der nominelle Servopfeil wird
in die neue Fahrzeugansicht transformiert; Servo-Mitte und Stellwinkel werden
mechanisch nicht automatisch verändert. Die Sensor-Rohachsen beziehen sich
weiterhin auf die Sensorplatine, nicht auf eine umbenannte Fahrzeugachse.

Das 3x3-Pad hat acht Richtungen und STOPP & SPERREN in der Mitte. Diagonalen
fahren eine Kurve solange die Taste gehalten wird, keine gemessene 45°-Aktion:

| Taste / API-direction | Logisch links | Logisch rechts |
| --------------------- | ------------: | -------------: |
| ↖ / forward-left      |         +50 % |         +100 % |
| ↗ / forward-right     |        +100 % |          +50 % |
| ↙ / backward-left     |         -50 % |         -100 % |
| ↘ / backward-right    |        -100 % |          -50 % |

Das feste 2:1-PWM-Verhältnis ist nur mit 100/50 innerhalb der verlangten
50–100-%-Grenze möglich. Deshalb ignorieren Kurven den PWM-Regler; die Webseite
nennt das ausdrücklich. Das ist ein PWM-Verhältnis, keine gemessene Kettengeschwindigkeit
oder garantierte Kurvengeometrie. Rückwärts zeigt der diagonale Pfeil die
Kurvenrichtung des Hecks an. Geradeauskorrektur wird beim Kurvenfahren beendet.
Loslassen, Seitenwechsel, Watchdog und Stopptaster funktionieren wie bei gerader Fahrt.
Ein explizites `speed=0` an /api/drive bleibt auch bei Kurven STOPP.

Die Drehbuttons bieten zusätzlich ±360° (vollständiger Kreis mit unaufgerolltem
Gyrowinkel). Maximaldauer für 360°: 24 s, für 90°/180° unverändert 12 s;
Heartbeat-Frist unverändert 500 ms. Nachlaufmessung, Feinkorrektur und bleibende
Freigabe nach Erfolg gelten auch für volle Kreise.

Ein virtueller Joystick ist noch nicht implementiert; die gekapselte differentielle
Ansteuerung und der Browser-Watchdog können dafür weiterverwendet werden.
