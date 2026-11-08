#include "StatusDisplay.h"
#include "Config.h"
#include "Pins.h"
#include <cmath>

void StatusDisplay::begin() {
  tft_.init();
  tft_.setRotation(Config::FrontReversed ? 2 : 0);
  pinMode(Pins::DisplayBacklight, OUTPUT);
  digitalWrite(Pins::DisplayBacklight, HIGH);
}

void StatusDisplay::update(TankDrive& drive, const RgbLed& led, const Button& button,
                           const Turret& turret, const MotionSensor& motion, const Heading& heading, const String& ip) {
  if (millis() - lastDraw_ < 150)
    return;
  lastDraw_ = millis();
  auto state = drive.state();
  String signature = String(state.armed) + ":" + state.left + ":" + state.right + ":" +
                     button.pressCount() + ":" + led.redValue() + ":" + led.greenValue() + ":" +
                     led.blueValue() + ":" + turret.angle() + ":" + turret.active() + ":" + ip;
  auto m = motion.reading();
  signature += ":" + String(m.valid) + ":" + String(m.connected) + ":" + String(m.sampledAt);
  if (signature == last_)
    return;
  last_ = signature;
  tft_.fillScreen(TFT_BLACK);
  tft_.setTextColor(TFT_WHITE, TFT_BLACK);
  tft_.drawString("Turret " + String(turret.angle()) + " deg", 8, 0, 2);
  tft_.setTextColor(turret.active() ? TFT_ORANGE : TFT_CYAN, TFT_BLACK);
  tft_.drawString("Soll", 8, 22, 1);
  // Nominelle Servomitte bleibt mechanisch unveraendert. Bei neuer Vorderseite
  // zeigt sie nach hinten. Pfeil um 180 Grad transformieren, Zeichenbereich erhalten.
  const float radians = (turret.angle() - Turret::CenterAngle) * 3.14159265f / 180.0f +
                        (Config::FrontReversed ? 3.14159265f : 0.0f);
  const int pivotY = Config::FrontReversed ? 31 : 77;
  const int tipX = 67 + lroundf(46 * sinf(radians));
  const int tipY = pivotY - lroundf(46 * cosf(radians));
  tft_.drawLine(67, pivotY, 67, Config::FrontReversed ? 24 : 31, TFT_DARKGREY);
  tft_.drawLine(67, pivotY, tipX, tipY, turret.active() ? TFT_ORANGE : TFT_CYAN);
  tft_.fillTriangle(tipX, tipY, tipX + lroundf(-10 * sinf(radians) - 5 * cosf(radians)),
                    tipY + lroundf(10 * cosf(radians) - 5 * sinf(radians)),
                    tipX + lroundf(-10 * sinf(radians) + 5 * cosf(radians)),
                    tipY + lroundf(10 * cosf(radians) + 5 * sinf(radians)),
                    turret.active() ? TFT_ORANGE : TFT_CYAN);
  tft_.setTextColor(state.armed ? TFT_ORANGE : TFT_GREEN, TFT_BLACK);
  tft_.drawCentreString(state.armed ? "FREIGEGEBEN" : "GESPERRT", 67, 88, 2);
  tft_.setTextColor(TFT_WHITE, TFT_BLACK);
  tft_.drawCentreString(ip, 67, 110, 1);
  tft_.drawCentreString("L:" + String(state.left) + "% R:" + String(state.right) + "%", 67, 123, 1);
  tft_.drawString("Taster " + String(button.pressCount()), 5, 136, 1);
  tft_.fillCircle(116, 140, 5, tft_.color565(led.redValue(), led.greenValue(), led.blueValue()));
  tft_.setTextColor(m.valid ? TFT_CYAN : TFT_ORANGE, TFT_BLACK);
  tft_.drawString(m.valid ? String(motion.model()) + " g / deg/s" : (m.connected ? "IMU: warte..." : "IMU: fehlt"), 5, 152, 1);
  if (m.valid) {
    tft_.drawString("aX " + String(m.ax, 2) + " Y " + String(m.ay, 2), 5, 166, 1);
    tft_.drawString("aZ " + String(m.az, 2) + " g", 5, 178, 1);
    tft_.drawString("gX " + String(m.gx, 1) + " Y " + String(m.gy, 1), 5, 190, 1);
    tft_.drawString("gZ " + String(m.gz, 1) + " deg/s", 5, 202, 1);
  }
  tft_.setTextColor(heading.ready() ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  tft_.drawString(heading.ready() ? "Winkel " + String(heading.angle(), 1) + " deg" :
                    (heading.calibrating() ? "2s still halten..." : "Winkel: kalibrieren"), 5, 215, 1);
  tft_.setTextColor(TFT_WHITE, TFT_BLACK);
  tft_.drawCentreString("Taster = Sperren", 67, 231, 1);
}
