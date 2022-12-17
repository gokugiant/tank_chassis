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
  statusLED.off();
}

void loop() {
  bool buttonIsPressed = digitalRead(BUTTON_PIN);

  if (buttonIsPressed) {  // go from 0-180 degrees
    pos += 1;
    if(pos > 2) {pos = 0; statusLED.off();}
    Serial.println(pos);
    statusLED.add(10, 10, 10);
    turret.turnToDegree(pos);    // set the servo position (degrees)
    delay(500);
    if(pos ==1){
      move.forward(100);
      Serial.println("Forward"); 
    }else if(pos==2){
      move.backward(100);
      Serial.println("Backward"); 
    }else{
      move.stop();
    }
  }
}