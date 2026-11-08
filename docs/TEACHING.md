# Einsatz in der Vorlesung

1. Einführung mit [Präsentation](../Presentation/README.md): Arduino/ESP32/MKR,
   WLAN-Access-Point, lokale Webseite gegenüber externem Backend/MQTT.
2. `01-HelloWebServer.code-workspace`: Umgebung `hello_webserver`. Minimales
   `setup()`/`loop()`, `server.begin()`, Route `/`, JSON-Route `/api/hello` zeigen;
   eigene Route ergänzen. Das ist ein eigenständiges PlatformIO-Projekt.
3. `02-TankWebControl.code-workspace`: Umgebung `tank_webcontrol`. Zuerst LED/Taster,
   dann Klassen/Dateigrenzen, Browser-JavaScript, Status-Polling und POST-Routen erklären.
4. Hardware-Demo: neue Vorderseite beachten, 50–100 % PWM, Freigabe und Halten/
   Loslassen, optional Kurven 100/50. Bei Bewegungen bewusster Einzeltest mit Stoppmöglichkeit.
5. Sensor-/Regelungsdemo erst nach Wiederherstellung der aktuell fehlenden
   BMI160-Verbindung: Kalibrierung, relativer Winkel, Geradeauskorrektur, kontrollierte
   Drehung. Genauigkeit und Modelltests klar von physischer Validierung unterscheiden.

Die Präsentation entstand vor den letzten Hardwareerweiterungen. Ihre frühere
Pinbelegung/Displayausrichtung nicht als heutigen Verdrahtungsplan verwenden;
maßgeblich sind [HARDWARE.md](HARDWARE.md) und `WebControl/include/Pins.h`.

Die Webseite wird vom ESP ausgeliefert; ihr JavaScript läuft im Browser.
PWM ist ein Sollwert, keine gemessene Geschwindigkeit. Diagonaltasten sind
Kurven mit PWM-Verhältnis 2:1 und keine gemessenen 45°-Aktionen. Sensorwinkel sind
relativ, driften und sind keine absolute Positionsmessung.

Internet-Telemetrie per MQTT/FastAPI bleibt Architektur-Ausblick. Ein virtueller
Joystick ist ein optionaler nächster Schritt und nicht im aktuellen Code enthalten.
