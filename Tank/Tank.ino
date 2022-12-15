#include <SPI.h>
#include <pwmWrite.h>

#define SERVO_PIN 33 // ESP32 pin GIOP26 connected to servo motor

int pos = 0;
Pwm pwm = Pwm();

void setup(void) {
  Serial.begin(9600); // For debug
}

void loop() {
  for (pos = 0; pos <= 180; pos++) {  // go from 0-180 degrees
    pwm.writeServo(SERVO_PIN, pos);    // set the servo position (degrees)
    delay(25);
  }
  for (pos = 180; pos >= 0; pos--) {  // go from 180-0 degrees
    pwm.writeServo(SERVO_PIN, pos);    // set the servo position (degrees)
    delay(25);
  }
}