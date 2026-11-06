#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "Pins.h"
#include "Config.h"

WebServer server(80);

void setup() {
  // Dieses Minimalbeispiel darf die Motoren beim Start nicht einschalten.
  for (auto pin : {Pins::LeftEnable, Pins::RightEnable, Pins::LeftForward, Pins::LeftBackward,
                   Pins::RightForward, Pins::RightBackward}) {
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
  }
  pinMode(Pins::Servo, INPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(Config::WifiName, Config::WifiPassword)) {
    Serial.println("WLAN konnte nicht starten");
    return;
  }
  // URL -> Funktion -> HTTP-Antwort. Diese zwei Routen sind der Lehrkern.
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html; charset=utf-8",
                "<h1>Hallo vom Tank!</h1><p>Diese Seite kommt direkt vom ESP32.</p><a "
                "href='/api/hello'>JSON ansehen</a>");
  });
  server.on("/api/hello", HTTP_GET, []() {
    server.send(200, "application/json", "{\"message\":\"Hallo\",\"device\":\"ESP32\"}");
  });
  server.begin();
  Serial.println("http://" + WiFi.softAPIP().toString());
}

void loop() {
  server.handleClient();
  delay(1);
}
