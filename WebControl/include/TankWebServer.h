#pragma once
#include <WebServer.h>
#include "TankDrive.h"
#include "RgbLed.h"
#include "Button.h"

class TankWebServer {
public:
  TankWebServer(TankDrive& drive, RgbLed& led, Button& button)
      : drive_(drive), led_(led), button_(button) {
  }

  bool begin();

  void update() {
    server_.handleClient();
  }

  String ip() const;

private:
  void registerRoutes();
  void sendStatus();
  bool allowWrite();
  bool readNumber(const char* name, int min, int max, int& value);
  bool checkController();
  void error(int status, const char* message);
  WebServer server_{80};
  TankDrive& drive_;
  RgbLed& led_;
  Button& button_;
  String owner_;
  int lastSequence_ = 0;
};
