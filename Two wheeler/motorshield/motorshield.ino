#include <MotorDriver.h>

const int BUTTON = 2;

MotorDriver m;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {

  bool buttonIsPressed = ! digitalRead(BUTTON);  // read and invert the button to match var description

  if(buttonIsPressed){
    m.motor(3,FORWARD,255);
    m.motor(4,FORWARD,255);  
  }else{
    m.motor(3,RELEASE, 0);
    m.motor(4,RELEASE, 0); 
  }
}
