#include <Arduino.h>
#include "Pins.h"
#include "TankDrive.h"
#include "RgbLed.h"
#include "Button.h"
#include "StatusDisplay.h"
#include "TankWebServer.h"

TankDrive drive;
RgbLed led;
Button button;
StatusDisplay display;
TankWebServer web(drive, led, button);

void setup() {
  Serial.begin(115200);
  pinMode(Pins::Servo, INPUT); // Servo derzeit nicht angeschlossen.
  if (!drive.begin()) {
    Serial.println("Motor initialization failed");
    while (true)
      delay(1000);
  }
  led.begin();
  led.red();
  button.begin();
  display.begin();
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
  if (button.rawPressed())
    drive.arm(false); // Lokaler Stopp, unabhaengig von HTTP-Routen.
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
  web.update(); // Liest HTTP-Anfragen und ruft die passende Route auf.
  display.update(drive, led, button, web.ip());
  delay(1);
}
