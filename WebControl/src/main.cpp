#include <Arduino.h>
#include "Turret.h"
#include "MotionSensor.h"
#include "Heading.h"
#include "Navigation.h"
#include "I2cDiagnostics.h"
#include "TankDrive.h"
#include "RgbLed.h"
#include "Button.h"
#include "StatusDisplay.h"
#include "TankWebServer.h"

TankDrive drive;
Turret turret;
RgbLed led;
Button button;
StatusDisplay display;
MotionSensor motion;
Heading heading;
Navigation navigation(drive, heading);
TankWebServer web(drive, led, button, turret, motion, heading, navigation);

void setup() {
  Serial.begin(115200);
  if (!drive.begin()) {
    Serial.println("Motor initialization failed");
    while (true)
      delay(1000);
  }
  if (!turret.begin()) {
    Serial.println("Turret initialization failed");
    while (true)
      delay(1000);
  }
  led.begin();
  led.red();
  button.begin();
  display.begin();
  Serial.println(motion.begin() ? String("IMU detected: ") + motion.model() : "IMU missing; retry every 3 s");
  printI2cDiagnostics(); // Nur beim Start, waehrend die Motoren gesperrt sind.
  if (!web.begin()) {
    drive.arm(false);
    Serial.println("WiFi start failed");
    while (true)
      delay(1000);
  }
  Serial.println("TANK WEB READY http://" + web.ip() + " | motors locked");
}

void loop() {
  button.update();
  turret.update();
  if (button.rawPressed()) {
    navigation.cancel("Taster-Stopp");
    turret.stop();
    drive.arm(false); // Lokaler Stopp, unabhaengig von HTTP-Routen.
  }
  if (button.wasPressed()) {
    // Kleine lokale Demo: stoppen/sperren und LED-Farbe wechseln.
    static int color = 0;
    color = (color + 1) % 3;
    if (color == 0)
      led.red();
    else if (color == 1)
      led.green();
    else
      led.blue();
  }
  motion.update();
  auto driveState = drive.state();
  heading.update(motion.reading(), !driveState.armed && !turret.active());
  navigation.update();
  static uint32_t lastSensorLog = 0;
  if (millis() - lastSensorLog >= 1000) {
    lastSensorLog = millis();
    auto m = motion.reading();
    Serial.printf("IMU connected=%d valid=%d addr=0x%02X id=0x%02X a[g]=%.3f,%.3f,%.3f gyro[dps]=%.2f,%.2f,%.2f button=%d\n",
                  m.connected, m.valid, m.address, m.chipId, m.ax, m.ay, m.az, m.gx, m.gy, m.gz, button.rawPressed());
    Serial.printf("HEADING ready=%d calibrating=%d angle=%.2f rate=%.2f bias=%.3f %s\n",
                  heading.ready(), heading.calibrating(), heading.angle(), heading.rate(),
                  heading.bias(), heading.status());
  }
  web.update(); // Liest HTTP-Anfragen und ruft die passende Route auf.
  display.update(drive, led, button, turret, motion, heading, web.ip());
  delay(1);
}
