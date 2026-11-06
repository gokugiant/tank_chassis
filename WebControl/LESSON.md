# Lehrablauf: vom Browser zum GPIO

## 1. Wo läuft was?

```text
Browser                         ESP32                    Hardware
HTML/CSS/JavaScript  -- HTTP --> Webserver -- Methode --> GPIO/PWM
                    <-- JSON --          <-- Zustand -- Taster
```

Die HTML-Datei liegt im Flash des ESP32. Nach dem Abruf führt der Browser ihr JavaScript aus. `TankDrive.cpp` läuft dagegen auf dem ESP32 und schaltet den Motortreiber.

## 2. Erst nur einen Webserver zeigen

Das eigenständige Projekt `examples/01_HelloWebServer/` öffnen und `src/main.cpp` erklären. Motoren bleiben aus.

```cpp
WebServer server(80);
WiFi.softAP(Config::WifiName, Config::WifiPassword);
server.on("/", HTTP_GET, []() {
  server.send(200, "text/html", "<h1>Hallo vom ESP32!</h1>");
});
server.begin();
```

`on()` meldet an, welche Funktion zu einer URL gehört. `begin()` startet den Server. `loop()` ruft laufend `server.handleClient()` auf. `/api/hello` liefert anschließend JSON statt HTML. Beide Antworten lassen sich direkt im Browser öffnen.

## 3. Eine Hardwareklasse unabhängig vom Web verstehen

In der vollständigen Anwendung zuerst `Pins.h`, `RgbLed.h` und `RgbLed.cpp` öffnen:

```cpp
led.green();
led.setColor(255, 0, 128);
led.off();
```

Die Anwendung braucht dafür keine GPIO-Nummern zu kennen. Dasselbe Prinzip gilt für `drive.forward(40)` und `button.wasPressed()`. `setup()` initialisiert, `loop()` verarbeitet Ereignisse. Ein gehaltener Taster soll als genau ein Druck zählen.

## 4. Eine Route ruft die Klasse auf

In `TankWebServer.cpp` den Handler `/api/led` zeigen. Sein Ablauf:

1. HTTP-Eingabe lesen.
2. Werte prüfen: Zahlen zwischen 0 und 255.
3. `led_.setColor(red, green, blue)` aufrufen.
4. Mit dem neuen Zustand als JSON antworten.

Der folgende Ausschnitt ist die didaktische Kurzfassung; die ausführbare Route enthält zusätzliche Eingabeprüfung:

```cpp
server.on("/api/led", HTTP_POST, [&]() {
  led.setColor(0, 255, 0);
  server.send(200, "application/json", "{\"ok\":true}");
});
```

## 5. JavaScript sendet die Anfrage

In `WebPage.h` nach `setColor` suchen. Der Kern ist:

```js
fetch('/api/led', {
  method: 'POST',
  headers: {
    'X-Tank-Control': '1',
    'Content-Type': 'application/x-www-form-urlencoded',
  },
  body: new URLSearchParams({ red: 0, green: 255, blue: 0 }),
});
```

Die Route wird auf demselben ESP32 aufgerufen, von dem die Webseite geladen wurde. Kein Cloudserver beteiligt. Status wird unabhängig davon regelmäßig über `GET /api/status` gelesen. Mit dem echten Taster drücken und beobachten, wie der Zähler ohne Seitenneuladen steigt.

## 6. Erst dann fahren

`TankDrive.h` zeigt die lesbare Schnittstelle, `TankDrive.cpp` kapselt bestätigte Richtungspins und PWM. In der Webroute steht beispielsweise `drive_.forward(speed)`.

Bei Motoren reicht ein einmaliger Startbefehl nicht: Loslassen muss stoppen; ein verlorener Browser muss ebenfalls stoppen. Deshalb Freigabe, Befehlsauffrischung und geräteseitiges Timeout zeigen. Erst mit aufgebockten Ketten demonstrieren. Browser-Netzwerkansicht öffnen und POST-Anfragen während des Haltens beobachten.

## Aufgaben mit KI

- Neue LED-Voreinstellung „Lila“ ergänzen und erklären, welcher Code wo läuft.
- Laufzeit aus dem JSON auf der Webseite anzeigen.
- Für den Taster eine sichtbare Zählerkarte gestalten.
- Eigene Route für eine Lichtfunktion hinzufügen; GPIOs nicht im Webhandler verwenden.
- Verbindungsabbruch testen und Beobachtung mit dem Timer-Code vergleichen.

Keine KI-Änderung an Pins oder Richtungen ungeprüft übernehmen. Der bestätigte Hardwaretest unter `../MotorTest/` bleibt die Referenz.
