#pragma once
#include <WebServer.h>
#include "TankDrive.h"
#include "RgbLed.h"
#include "Button.h"
#include "Turret.h"
#include "MotionSensor.h"
#include "Heading.h"
#include "Navigation.h"

class TankWebServer {
public:
  TankWebServer(TankDrive& drive, RgbLed& led, Button& button, Turret& turret, MotionSensor& motion,
                Heading& heading, Navigation& navigation)
      : drive_(drive), led_(led), button_(button), turret_(turret), motion_(motion),
        heading_(heading), navigation_(navigation) {
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
  Turret& turret_;
  MotionSensor& motion_;
  Heading& heading_;
  Navigation& navigation_;
  String owner_;
  int lastSequence_ = 0;
};
