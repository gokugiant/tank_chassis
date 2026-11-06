#include "StatusDisplay.h"
#include "Config.h"
#include "Pins.h"

void StatusDisplay::begin() {
  tft_.init();
  tft_.setRotation(0);
  pinMode(Pins::DisplayBacklight, OUTPUT);
  digitalWrite(Pins::DisplayBacklight, HIGH);
}

void StatusDisplay::update(TankDrive& drive, const RgbLed& led, const Button& button,
                           const String& ip) {
  if (millis() - lastDraw_ < 150)
    return;
  lastDraw_ = millis();
  auto state = drive.state();
  String signature = String(state.armed) + ":" + state.left + ":" + state.right + ":" +
                     button.pressCount() + ":" + led.redValue() + ":" + led.greenValue() + ":" +
                     led.blueValue() + ":" + ip;
  if (signature == last_)
    return;
  last_ = signature;
  tft_.fillScreen(TFT_BLACK);
  tft_.setTextColor(TFT_WHITE, TFT_BLACK);
  tft_.drawCentreString("TANK LAB", 67, 5, 2);
  tft_.setTextColor(state.armed ? TFT_ORANGE : TFT_GREEN, TFT_BLACK);
  tft_.drawCentreString(state.armed ? "FREIGEGEBEN" : "GESPERRT", 67, 30, 2);
  tft_.setTextColor(TFT_CYAN, TFT_BLACK);
  tft_.drawCentreString(Config::WifiName, 67, 58, 2);
  tft_.drawCentreString(ip, 67, 82, 2);
  tft_.setTextColor(TFT_WHITE, TFT_BLACK);
  tft_.drawString("L: " + String(state.left) + " %", 8, 115, 2);
  tft_.drawString("R: " + String(state.right) + " %", 8, 138, 2);
  tft_.drawString("Taster: " + String(button.pressCount()), 8, 168, 2);
  uint16_t color = tft_.color565(led.redValue(), led.greenValue(), led.blueValue());
  tft_.fillCircle(18, 207, 9, color);
  tft_.drawCircle(18, 207, 10, TFT_WHITE);
  tft_.drawString("RGB", 36, 199, 2);
  tft_.drawCentreString("Taster = Sperren", 67, 227, 1);
}
