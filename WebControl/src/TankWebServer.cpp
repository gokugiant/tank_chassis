#include "TankWebServer.h"
#include "Config.h"
#include "WebPage.h"
#include <WiFi.h>

// VORLESUNG 1: Der ESP32 erzeugt sein eigenes WLAN, ohne Hochschulnetz.
bool TankWebServer::begin() {
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1),
                         IPAddress(255, 255, 255, 0)))
    return false;
  if (!WiFi.softAP(Config::WifiName, Config::WifiPassword))
    return false;
  const char* headers[] = {"X-Tank-Control"};
  server_.collectHeaders(headers, 1);
  registerRoutes();
  server_.begin(); // Ab jetzt lauscht HTTP auf Port 80.
  return true;
}

String TankWebServer::ip() const {
  return WiFi.softAPIP().toString();
}

void TankWebServer::error(int status, const char* message) {
  server_.send(status, "application/json", String("{\"error\":\"") + message + "\"}");
}

bool TankWebServer::allowWrite() {
  // Eigener Header verhindert fremde HTML-Formularaufrufe; kein CORS aktiviert.
  if (server_.header("X-Tank-Control") != "1") {
    error(403, "Control header missing");
    return false;
  }
  return true;
}

bool TankWebServer::readNumber(const char* name, int minimum, int maximum, int& value) {
  String text = server_.arg(name);
  if (text.isEmpty() || text.length() > 9) {
    error(400, "Invalid number");
    return false;
  }
  long number = 0;
  for (auto c : text) {
    if (c < '0' || c > '9') {
      error(400, "Invalid number");
      return false;
    }
    number = number * 10 + c - '0';
  }
  if (number < minimum || number > maximum) {
    error(400, "Number out of range");
    return false;
  }
  value = number;
  return true;
}

bool TankWebServer::checkController() {
  if (owner_.isEmpty() || server_.arg("client") != owner_) {
    error(409, "Another browser or no controller");
    return false;
  }
  int sequence;
  if (!readNumber("seq", 1, 999999999, sequence))
    return false;
  if (sequence <= lastSequence_) {
    error(409, "Old command");
    return false;
  }
  lastSequence_ = sequence;
  return true;
}

// VORLESUNG 2: Eine Route verbindet URL und Hardwarefunktion.
void TankWebServer::registerRoutes() {
  server_.on("/", HTTP_GET, [this]() {
    server_.sendHeader("Cache-Control", "no-store");
    server_.send_P(200, "text/html; charset=utf-8", WebPage);
  });
  server_.on("/api/status", HTTP_GET, [this]() {
    sendStatus();
  });
  server_.on("/api/arm", HTTP_POST, [this]() {
    if (!allowWrite())
      return;
    int enabled;
    if (!readNumber("enabled", 0, 1, enabled))
      return;
    if (!enabled) {
      drive_.arm(false);
      owner_ = "";
      lastSequence_ = 0;
      sendStatus();
      return;
    }
    String client = server_.arg("client");
    if (client.length() < 8 || client.length() > 64) {
      error(400, "Invalid client");
      return;
    }
    if (button_.rawPressed()) {
      error(409, "Release physical button");
      return;
    }
    if (drive_.state().armed && owner_ != client) {
      error(409, "Another browser controls the tank");
      return;
    }
    int sequence;
    if (!readNumber("seq", 1, 999999999, sequence))
      return;
    if (owner_ == client && sequence <= lastSequence_) {
      error(409, "Old command");
      return;
    }
    owner_ = client;
    lastSequence_ = sequence;
    drive_.arm(true);
    sendStatus();
  });
  server_.on("/api/drive", HTTP_POST, [this]() {
    if (!allowWrite() || !checkController())
      return;
    if (button_.rawPressed()) {
      drive_.arm(false);
      error(409, "Physical stop pressed");
      return;
    }
    int speed;
    if (!readNumber("speed", 0, Config::MaxSpeed, speed))
      return;
    String direction = server_.arg("direction");
    bool ok = false;
    if (direction == "forward")
      ok = drive_.forward(speed);
    else if (direction == "backward")
      ok = drive_.backward(speed);
    else if (direction == "left")
      ok = drive_.turnLeft(speed);
    else if (direction == "right")
      ok = drive_.turnRight(speed);
    else {
      drive_.stop();
      error(400, "Unknown direction");
      return;
    }
    if (!ok) {
      error(409, "Arm the tank first");
      return;
    }
    sendStatus();
  });
  server_.on("/api/stop", HTTP_POST, [this]() {
    if (!allowWrite() || !checkController())
      return;
    drive_.stop();
    sendStatus();
  });
  server_.on("/api/led", HTTP_POST, [this]() {
    if (!allowWrite())
      return;
    int red, green, blue;
    if (!readNumber("red", 0, 255, red) || !readNumber("green", 0, 255, green) ||
        !readNumber("blue", 0, 255, blue))
      return;
    led_.setColor(red, green, blue);
    sendStatus();
  });
  server_.onNotFound([this]() {
    error(404, "Route not found");
  });
}

// VORLESUNG 3: JSON transportiert Daten; hier wird kein HTML erzeugt.
void TankWebServer::sendStatus() {
  auto s = drive_.state();
  String json = "{\"armed\":" + String(s.armed ? "true" : "false") + ",\"left\":" + s.left +
                ",\"right\":" + s.right +
                ",\"buttonPressed\":" + (button_.isPressed() ? "true" : "false") +
                ",\"buttonCount\":" + button_.pressCount() + ",\"uptimeMs\":" + millis() +
                ",\"red\":" + led_.redValue() + ",\"green\":" + led_.greenValue() +
                ",\"blue\":" + led_.blueValue() + ",\"maxSpeed\":" + Config::MaxSpeed + "}";
  server_.sendHeader("Cache-Control", "no-store");
  server_.send(200, "application/json", json);
}
