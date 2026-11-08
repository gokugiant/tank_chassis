# Validierungsstand

## Aktuell – 8. November 2026

Neueste Firmware erfolgreich gebaut und aufgespielt: neue Vorderseite (bisheriges
Heck), TFT-Rotation2, acht Fahrtasten mit 100/50-%-Kurven sowie ±360° zusätzlich
zu Kalibrierung, Richtungskorrektur und Dreh-Feinkorrektur. Host- und Browserlogiktests bestehen.

**Aktuell blockiert: Sensorverbindung.** Letzter USB-Befund auch nach Neustart:
SDA33/SCL32 HIGH, I²C-Suche `devices=0`, `connected=0`, `valid=0`. Zuvor war BMI160
an 0x68 mit Chip-ID0xD1 und gültigen Messwerten bestätigt. Pins sind unverändert.
Die neue Orientierung, Kurven, 360° und reale Feinkorrektur sind noch physisch zu prüfen.

Maßgeblich für den Wiedereinstieg: [START_HERE.md](../docs/START_HERE.md).
Die folgenden Einträge sind **chronologisch**; frühere Aussagen wie „noch nicht
hochgeladen“ oder alte Pin-/Funktionsstände gelten für den jeweiligen damaligen Schritt.

## Chronologischer Verlauf

- Vollständige WebControl-Firmware und eigenständiges Hello-Webserver-Beispiel erfolgreich gebaut.
- WebControl aufgespielt; serielle Startmeldung mit IP 192.168.4.1 und gesperrten Motoren bestätigt.
- Der Besitzer hat anschließend gemeldet: „Ok das beispiel funktioniert.“ Eine separate vollständige Aufzeichnung aller HTTP-/Abbruchtests liegt nicht vor.
- Hosttests der Hardwareklassen bestehen: bestätigte Richtungen, PWM-Grenze, Stopp, Befehls-/Freigabe-Timeout, millis-Überlauf, Tasterentprellung und RGB.
- Nach der reinen Formatierung: Hosttests und Build erneut erfolgreich. Keine neue Firmware für die Formatierung aufgespielt.

Browser-/Verbindungsabbrüche und längerer Fahrbetrieb bleiben gesondert am aufgebockten Fahrzeug zu prüfen. Die vorherige bestätigte Hardwarebasis steht unter `../docs/VALIDATION.md`.

## Turret-Erweiterung

GPIO 12 laut Nutzerangabe; noch kein Upload und kein physischer Servotest. Nomineller Bereich 0–180°, nominelle Mitte 90°. Drehrichtung, reale Winkel und mechanische Anschläge sind unbestätigt.

Hosttests inklusive Turret-Grenzen, Mittelimpuls, Stopp und Abschaltung über millis-Überlauf bestanden. PlatformIO-Build `tank_webcontrol` erfolgreich. Weboberfläche noch nicht am Gerät geprüft.

Display: vergrößerter Richtungspfeil (46 statt 22 Pixel Länge), linke/rechte PWM gemeinsam in einer Zeile. Sichtprüfung am Fahrzeug noch offen. Turret-Endpunkte 0°/180° zusätzlich im Hosttest geprüft.

## IMU-Erweiterung, 6. November 2026

- Nutzer bestätigt Taster an GPIO36 und Sensorverkabelung SDA33/SCL32.
- Hosttests: Messwertskalierung, Vorzeichen, Frische, kurze I²C-Antwort,
  Ausfall/Wiederverbindung, zweite Adresse und Ablehnung unbekannter Chip-ID bestanden.
- Bestehende Antrieb-/Taster-/RGB-/Turret-Hosttests bestanden (Tastertest nutzt Pins::Button).
- PlatformIO-Build und Upload über /dev/cu.usbserial-01E5F16A erfolgreich.
- USB-Diagnose: IMU connected=0, valid=0, WHO_AM_I nicht lesbar; I²C-Zeitüberschreitungen (Error 263). Noch keine physischen Sensormesswerte bestätigt.
- Taster meldet im Ruhezustand button=0; Druck/Loslassen auf GPIO36 noch am Fahrzeug prüfen.
- Sensoranzeige im echten Browser und Display noch nicht visuell bestätigt.
- Keine Motor- oder Servo-Bewegungstests für diese Erweiterung.

## I²C-Identifikation nach Verkabelungskorrektur

Einmaliger Scan auf SDA33/SCL32: genau ein Gerät an **0x68**. Register 0x00 liefert
**0xD1**, entsprechend Bosch BMI160. Register 0x0F liefert 0x00.
Damit ist die bisherige LSM6DS3-Annahme widerlegt. Der vorhandene Messwerttreiber
unterstützt bislang nur LSM6DS3 und muss vor Messwertanzeige um BMI160 ergänzt werden.
Die aktuell vorhandene Versorgung/Verkabelung ermöglicht Kommunikation; kein
zusätzlicher VIN-Anschluss für diese Identifikation erforderlich. Motoren beim
Scan gesperrt, keine Fahr-/Servobefehle gesendet. Build, Upload und Hosttests bestanden.

## BMI160-Messwerttreiber erfolgreich aufgespielt

- BMI160-Unterstützung in MotionSensor ergänzt, LSM6DS3-Alternative erhalten.
- Hosttests prüfen zusätzlich BMI160-PMU-Einschaltzeiten, Skalierung/Vorzeichen,
  0x68/0x69, Datenfrische und fehlgeschlagenen PMU-Start; alle Hosttests bestanden.
- PlatformIO-Build und Upload erfolgreich. JavaScript-Syntax und git diff --check bestanden.
- USB zeigt sieben aufeinanderfolgende gültige Messungen: connected=1, valid=1,
  addr=0x68, id=0xD1. Beispiel a[g]=-0.058,-0.114,1.062;
  gyro[Grad/s]=-0.14,0.70,-0.09. Rohwerte noch nicht kalibriert.
- JSON enthält imu.model; Browser zeigt den tatsächlichen Modellnamen statt LSM6DS3.
  TFT-Beschriftung zeigt ebenfalls das Modell. Sichtprüfung dieser Anzeigen und
  physischer Dreh-/Kipptest durch Nutzer noch offen.
- Keine Fahr-/Servo-Bewegungstests oder automatische Motorregelung hinzugefügt.

## Relativer Winkel / Nullpunktabgleich (erste Stufe)

- Separate Klasse Heading; Abgleich per Webroute nur bei gesperrtem Antrieb und
  ohne aktives Servosignal. Noch keine automatischen Motoraktionen.
- Hosttests: Bias, Integration von 90/180 Grad und negativem Winkel, doppelte
  Samples, Messlücken, Kalibrierbewegung/Kippen und millis-Überlauf bestanden.
- Bestehende Hardware-/Sensor-Hosttests, JavaScript-Syntax, diff-Check und
  PlatformIO-Build/Upload bestanden.
- USB nach Upload: BMI160-Messwerte gültig; Winkel wartet erwartungsgemäß auf
  explizite Kalibrierung. Manuelles Kippen am Sensor in Rohwerten sichtbar.
- Physischer Winkel-/Vorzeichentest nach Kalibrierung steht noch aus. Diese
  Bestätigung ist Voraussetzung für den ersten geregelten Motordrehversuch.

## Navigation / Richtung halten und relative Drehungen

- Nutzer bestätigt physisch: Nase von oben nach links gedreht ergibt etwa +90°.
- Navigation mit optionaler PD-Geradeausregelung und relativen ±90°/±180°-Drehungen
  implementiert; Grenzen/Gewinne in Config.h. Noch nicht am Boden abgestimmt.
- Hosttests bestanden: Drehrichtungen, 90°/180°-Zielabschluss mit Sperren,
  Browser-Frist, unabhängiger Timer ohne loop()-Fortschritt, Stillstand/Stall,
  falsche Richtung, veralteter Winkel, Vor-/Rückwärtskorrektur, Abbruch und Gesamtdauer.
- Browserlogik unter nachgebildetem DOM/HTTP getestet: Freigabe-/Kalibriergrenzen,
  Drehrequest, Heartbeat-Sequenz, STOPP und später Timer-Callback nach Abbruch.
  Das ersetzt keine Sicht-/Netzwerkprüfung im echten Browser.
- Sämtliche Hardware-/IMU-/Heading-Hosttests, diff-Check, Firmwarebuild und Upload bestanden.
- Nach Upload physisch per USB: BMI160 gültig, Winkel wartet auf erneute Kalibrierung.
- Keine Fahr-/Dreh-/Servobefehle vom Agenten gesendet. Reale Zielgenauigkeit,
  Mindest-PWM, Nachlauf und Geradeausverbesserung stehen als Fahrzeugtests aus.

## Fehlalarm beim Anfahren mit Richtungskorrektur

- Nutzer meldet wiederholten Kippalarm beim Anfahren. Codeprüfung: bisheriger
  Abbruch schon bei einem einzigen horizontalen Beschleunigungswert >0,4 g.
- Neue geglättete, zeitbestätigte Prüfung statt einzelner Achsenschwellwerte.
- Regressionstests: 0,6 g Anfahrbeschleunigung über 600 ms, kurze starke
  Quer-/Vertikalspitzen, kurze 60°-Neigung und Rückkehr: kein Abbruch.
  Länger bestehende 60°-Neigung wird weiterhin verworfen.
- Alle Hardware-, IMU-, Heading-, Navigation-Hosttests und Browserlogiktest bestanden.
- Der tatsächliche Anfahrversuch nach diesem Fix steht noch aus; keine Bewegung
  vom Agenten gestartet.

## PWM-Bereich 50–100 % (7. November 2026)

- Nutzer bestätigt: Richtungskorrektur funktioniert grundsätzlich; automatische
  Drehungen scheitern vermutlich an zu wenig Anlaufleistung. Die bisherigen
  22–35 % liegen unter dem brauchbaren Bereich. Vorgabe: nur 50–100 % beim Fahren.
- UI startet bei 50 %, API lehnt 1–49 ab; 0 bleibt STOPP. Hardwareklasse begrenzt
  beide nichtnulligen Kettenwerte auf 50–100 %, unabhängig vom Aufrufer.
- Drehungen nutzen den gewählten PWM-Wert als Obergrenze (ohne Parameter 50 %),
  reduzieren vor dem Ziel nie unter 50 %. Beide Ketten weiterhin gegenläufig.
- Geradeauskorrektur verschiebt den gemeinsamen PWM-Anteil an den Grenzen, damit
  die Regeldifferenz bei 50/100 % erhalten bleibt, ohne eine Kette unter 50 % zu setzen.
- Hosttests und Browserlogiktest bestanden; zusätzliche Tests für Minimum,
  Maximum, STOPP=0, beide Drehrichtungen, Drehleistung und Reglergrenzen vor/rück.
- Firmwarebuild und diff-Check bestanden. Noch NICHT hochgeladen: USB-Inventar
  zeigt nur Bluetooth-Incoming-Port und debug-console, keinen ESP32.
- Höhere Drehleistung und mögliche Überschwinger müssen nach Upload physisch geprüft werden.

## Nachtrag 8. November 2026: PWM-Version hochgeladen

ESP32 wieder an /dev/cu.usbserial-01E5F16A erkannt. Firmware mit 50–100 % PWM
erfolgreich hochgeladen, Flash-Hashes verifiziert. USB danach: BMI160 connected=1,
valid=1; Winkel wartet erwartungsgemäß auf erneute Stillstandskalibrierung.
Keine Bewegungsbefehle gesendet. Physischer 90°-Test mit neuer Leistung bleibt offen.

## Drehgenauigkeit / Freigabe nach Erfolg (8. November 2026)

- Nutzer bestätigt: neue PWM funktioniert sehr gut; beobachtet einige Grad Nachlauf.
  Wünscht frühes Abbremsen, gegebenenfalls Gegenkorrektur und keine erneute Sperre nach Erfolg.
- Ansatz: rateabhängiger Vorhalt, Ausrollen und gemessenen Stillstand abwarten,
  maximal sechs kurze Korrekturimpulse bei mindestens 50 %. Zielfenster ±1°
  relativ zum Gyrowinkel, keine zugesicherte absolute mechanische Genauigkeit.
- Navigation-Tests mit einfachen Trägheitsmodellen (50/100/150 ms Zeitkonstante)
  bestanden: 90°, -180°, Gegenkorrektur nach Überschwingen, begrenzte erfolglose
  Korrekturversuche, Erfolg mit bleibender Freigabe und zweite Drehung ohne Arm-Aufruf.
- Sensor-/Leasing-/Watchdog-/Timeout-Fehler sperren weiterhin. Später Heartbeat
  nach Erfolg bleibt harmlos, verlängert aber die 30-s-Leerlauffreigabe nicht.
- Browserlogiktest bestätigt unmittelbare nächste Drehung ohne Freigabe-Aufruf,
  Timerabbruch und unveränderte Aktionsgeneration nach manueller Fahrantwort.
- Alle bestehenden Hosttests, Browserlogiktest, diff-Check und Firmwarebuild bestanden.
- Diese Tests bestätigen Softwareverhalten mit simulierten Sensordaten;
  Präzision und Nachlaufparameter müssen nach Upload real geprüft werden.

Upload dieser Präzisionsversion am 8. November erfolgreich, Flash-Hashes verifiziert.
USB danach: BMI160 valid=1; erneute Kalibrierung erwartungsgemäß erforderlich.
Keine Bewegungsbefehle gesendet; realer Endwinkeltest steht aus.

## Neue Fahrzeugausrichtung, Kurvenpad, 360° (8. November 2026)

- Nutzer möchte bisheriges Heck als Vorderseite und TFT um 180° drehen.
- FrontReversed=true tauscht logische Kettenseiten und invertiert Fahrpole;
  keine Kabeländerung. Hosttests prüfen explizit neue GPIO-Polung und Enable-Zuordnung.
- TFT-Rotation 2; nomineller Servopfeil in die neue Fahrzeugansicht transformiert,
  mechanische Servomitte unverändert. Sichtprüfung am Fahrzeug noch offen.
- Vier Kurventasten mit festen 100/50 % PWM, Vor-/Rückwärts, logischen Seiten und
  identischen Halten-/Loslassen-/Stoppmechanismen. Keine automatische 45°-Aktion.
- ±360° mit unverändertem unaufgerollten Gyrowinkel, bis zu 24 s Gesamtdauer.
  Hosttest bestätigt ±360°-Abschluss, bleibende Freigabe und mehr als 12 s erlaubte
  Zeit bei langsamer voller Drehung; normale Fehler/Watchdog-Grenzen bleiben aktiv.
- Alle Hosttests und Browserlogiktest bestanden; Browsertest prüft acht Richtungen,
  Kurvenrequest/Loslassen, zentralen Stoppbutton und 360°-Request.
- Firmwarebuild und diff-Check bestanden. Keine Bewegung vom Agenten ausgelöst;
  physische Prüfung der neuen Vorderseite, Kurven und vollen Drehung steht aus.
- Optionaler virtueller Joystick ausdrücklich noch nicht implementiert.

Upload dieser Version erfolgreich, Flash-Hashes verifiziert. Anschließende USB-
Diagnose zeigt unerwartet keinen Sensor: connected=0, valid=0. Neustart mit
Adresssuche bestätigt SDA33/SCL32, beide Buspegel HIGH, devices=0. Sensorpins
sind unverändert. WLAN/HTTP startet, Antrieb beim Start gesperrt. Die neue
Winkelsteuerung kann ohne wiederhergestellte Sensorverbindung nicht physisch
geprüft werden. Es wurden keine Bewegungsbefehle gesendet.
