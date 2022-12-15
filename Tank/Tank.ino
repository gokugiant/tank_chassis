#include <SPI.h>
#include <LedRGB.h>
#include <ESP32Servo.h>

const int TURRET_PIN = 33; // ESP32 pin GIOP26 connected to servo motor
const int BUTTON_PIN = 32;

const int RED_LED_PIN = 27;
const int GREEN_LED_PIN = 26;
const int BLUE_LED_PIN = 25;

int pos = 0;
Servo turrent;

LedRGB statusLED(RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN, 1);

void setup(void) {
  Serial.begin(9600); // For debug

  turrent.attach(TURRET_PIN); // attaches the servo on pin 18 to the servo object

  pinMode(BUTTON_PIN, INPUT);

  pinMode(BLUE_LED_PIN, OUTPUT);
  centerTurret();
}

void loop() {
  bool buttonIsPressed = digitalRead(BUTTON_PIN);

  digitalWrite(BLUE_LED_PIN, true);

  Serial.println(buttonIsPressed);
  if (buttonIsPressed) {  // go from 0-180 degrees
    statusLED.defaultColor("yellow");
    //turrent.write(pos);    // set the servo position (degrees)
  }else{
    statusLED.defaultColor("white");
    //centerTurret();
  }
}

void centerTurret(){
  turrent.write(90); // Set turrent to center
}