# Tank WebControl – vom GPIO zur Webseite

Lehrbeispiel für den bestätigten TTGO-Tank. Alle Hardwarezugriffe sind gekapselt, Webrouten rufen lesbare Methoden auf. Der bisherige Hardwaretest bleibt unter `../MotorTest/` erhalten.

## Start

Projektordner **WebControl** in PlatformIO öffnen:

```sh
pio run
pio run -t upload --upload-port /dev/cu.usbserial-01E5F16A
```

Seriellen Monitor vor Upload schließen. WLAN **Tank-Lab**, Demo-Passwort **TankLab2026**, danach **http://192.168.4.1** öffnen. Das WLAN liefert kein Internet. SSID und Passwort stehen in `include/Config.h`. Display zeigt SSID, IP, Freigabe, Sollwerte und Tasterzähler im bestätigten Hochformat.

Die Firmware startet gesperrt. **Fahren freigeben**, danach Richtungstaste gedrückt halten. Loslassen stoppt. Pfeile links/rechts drehen auf der Stelle (Ketten gegensinnig). Die PWM-Obergrenze steht in `Config.h` und beträgt aktuell 100 %, Standard im Browser 40 %. Das ist keine gemessene Geschwindigkeit.

Licht lässt sich unabhängig von der Motorfreigabe einstellen: Rot/Grün/Blau, Farbwähler oder Aus. Der echte Taster sperrt die Motoren und wechselt lokal die LED-Farbe. Die Webseite zeigt Zustand und Druckzähler; sie simuliert keinen physischen Tastendruck.

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
| POST    | `/api/led`    | `red`, `green`, `blue`, jeweils 0–255                                          |
| POST    | `/api/arm`    | `enabled=1`, `client`, `seq`; oder `enabled=0` zum globalen Stoppen/Sperren    |
| POST    | `/api/drive`  | `direction=forward/backward/left/right`, `speed=0..100`, `client`, `seq`        |
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
- Servo deaktiviert. Zunächst mit frei hängenden Ketten prüfen. USB ist keine Motorversorgung.

Nur für ein lokales Lehr-WLAN. Das Demo-Passwort ist öffentlich im Quellcode. Kein Internet-Fernzugriff, keine Benutzerverwaltung und keine Produktionssicherheitszusage.

## Prüfen

```sh
sh tests/host/run.sh
pio run
```

Hosttests prüfen echte Hardwareklassen mit nachgebildeten GPIOs: bestätigte Richtungen, PWM-Grenze, Stopp, Zeitablauf und millis-Überlauf, Tasterentprellung und RGB. Sie ersetzen keine elektrischen Messungen oder Tests am Fahrzeug. Den tatsächlichen Validierungsstand dokumentiert `VALIDATION.md`.

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
