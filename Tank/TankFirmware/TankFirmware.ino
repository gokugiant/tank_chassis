#include <ESP32PWM.h>

ESP32PWM turret;

const int actionBtn = 2;
const int turretServo = 3;
const int freq = 1000;

const int motDirLinksA = 8;
const int motDirLinksB = 7;
const int motLinksPWM = 9;

const int motDirRechtsA = 4;
const int motDirRechtsB = 5;
const int motRechtsPWM = 6;

void setup() {
  turret.attachPin(turretServo, freq, 10);
  
  pinMode(actionBtn, INPUT);

  pinMode(motDirLinksA, OUTPUT);
  pinMode(motDirLinksB, OUTPUT);

  pinMode(motDirRechtsA, OUTPUT);
  pinMode(motDirRechtsB, OUTPUT);
  /*
    // initialize device
    Serial.println("Initializing IMU device...");
    CurieIMU.begin();

    // Set the accelerometer range to 250 degrees/second
    CurieIMU.setGyroRange(250);
  */
}

void loop() {
  float gx, gy, gz; //scaled Gyro values

  // read gyro measurements from device, scaled to the configured range
  //CurieIMU.readGyroScaled(gx, gy, gz);

  // display tab-separated gyro x/y/z values
  //Serial.println(gz);

  int btnRead = digitalRead(actionBtn);

  if (btnRead == HIGH) {
    forward();
    delay(2000);
    motorStop();
    delay(500);  
    turnTurrent();
    delay(500);
    backward();
    delay(2000);
    motorStop();
  } else {
    motorStop();
  }
}

void forward() {
  digitalWrite(motDirRechtsA, LOW); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirRechtsB, HIGH);
  analogWrite(motRechtsPWM, 255);

  digitalWrite(motDirLinksA, HIGH); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirLinksB, LOW);
  analogWrite(motLinksPWM, 255); // speed counts from 0 to 255
}

void backward() {
  digitalWrite(motDirRechtsA, HIGH); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirRechtsB, LOW);
  analogWrite(motRechtsPWM, 255);

  digitalWrite(motDirLinksA, LOW); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirLinksB, HIGH);
  analogWrite(motLinksPWM, 255); // speed counts from 0 to 255
}

void turnLeft() {
  digitalWrite(motDirRechtsA, HIGH); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirRechtsB, LOW);
  analogWrite(motRechtsPWM, 255);

  digitalWrite(motDirLinksA, HIGH); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirLinksB, LOW);
  analogWrite(motLinksPWM, 255); // speed counts from 0 to 255
}

void turnRight() {
  digitalWrite(motDirRechtsA, LOW); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirRechtsB, HIGH);
  analogWrite(motRechtsPWM, 255);

  digitalWrite(motDirLinksA, LOW); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirLinksB, HIGH);
  analogWrite(motLinksPWM, 255); // speed counts from 0 to 255
}

void motorStop() {
  digitalWrite(motDirRechtsA, LOW); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirRechtsB, LOW);
  analogWrite(motRechtsPWM, 0);

  digitalWrite(motDirLinksA, LOW); // A = LOW and B = HIGH means the motor will turn left
  digitalWrite(motDirLinksB, LOW);
  analogWrite(motLinksPWM, 0); // speed counts from 0 to 255
}

void turnTurrent(){
  for(int i = 0; i <=150; i++){
    turret.write(i);                  // sets the servo position according to the scaled value
    delay(30);
  }
  for(int i = 150; i >=0; i--){
    turret.write(i);                  // sets the servo position according to the scaled value
    delay(30);
  }  
}
