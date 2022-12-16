#include "Turret.h"
#include "Wiring.h"
#include "Move.h"
#include <LedRGB.h>

int pos = 0;
LedRGB statusLED(RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN, 1);
Turret turret(TURRET_PIN);
Move move(L29N_LEFT_ENABLE, L29N_RIGHT_ENABLE, L29N_INPUT_1, L29N_INPUT_2, L29N_INPUT_3, L29N_INPUT_4);

void setup(void) {
  Serial.begin(9600); // For debug

  pinMode(BUTTON_PIN, INPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  turret.turnToDegree(0);
}

void loop() {
  bool buttonIsPressed = digitalRead(BUTTON_PIN);

  if (buttonIsPressed) {  // go from 0-180 degrees
    delay(100);
    pos += 90;
    if(pos > 180) pos = 0;
    Serial.println(pos);
    //statusLED.defaultColor("yellow");
    turret.turnToDegree(pos);    // set the servo position (degrees)
  }
}