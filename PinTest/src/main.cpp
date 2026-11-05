#include <Arduino.h>
#include <TFT_eSPI.h>
// Display/RGB test v2: no command can enable the motors.
constexpr uint8_t motorPins[] = {13,21,15,2,17,22};
constexpr uint8_t rgbPins[] = {27,26,25};
constexpr uint8_t buttonPin = 32;
constexpr uint16_t colors[] = {TFT_RED,TFT_GREEN,TFT_BLUE};
const char* names[] = {"ROT","GRUEN","BLAU"};
TFT_eSPI display;
String command;
int selected=0;
bool commonAnode=false;
int rawButton=LOW, stableButton=LOW;
uint32_t buttonChanged=0, presses=0;
void stopMotors() {for(auto p:motorPins) digitalWrite(p,LOW);}
void drawStatus() {
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE,TFT_BLACK);
  display.drawString("TANK LAB",10,5,2);
  display.setTextColor(TFT_GREEN,TFT_BLACK);
  display.drawRightString("MOTOREN AUS",230,5,2);
  display.drawRoundRect(8,29,164,55,10,TFT_DARKGREY);
  for(int i=0;i<3;i++) {
    int x=36+i*54;
    display.fillCircle(x,56,19,selected==i?colors[i]:TFT_DARKGREY);
    display.drawCircle(x,56,20,selected==i?TFT_WHITE:TFT_DARKGREY);
  }
  display.setTextColor(selected>=0?colors[selected]:TFT_WHITE,TFT_BLACK);
  display.drawString(selected>=0?names[selected]:"AUS",182,48,2);
  display.setTextColor(TFT_WHITE,TFT_BLACK);
  display.drawString("Taster: naechste Farbe",10,91,2);
  display.setTextColor(TFT_LIGHTGREY,TFT_BLACK);
  display.drawString("GPIO32 | Klicks: "+String(presses),10,113,2);
}
void setColor(int channel) {
  selected=channel;
  for(int i=0;i<3;i++) digitalWrite(rgbPins[i],(i==selected)!=commonAnode);
  drawStatus();
  Serial.printf("COLOR %s | motors OFF\n",selected>=0?names[selected]:"AUS");
}
void handle(String s) {
  s.trim();
  if(s=="red") setColor(0);
  else if(s=="green") setColor(1);
  else if(s=="blue") setColor(2);
  else if(s=="off") setColor(-1);
  else if(s=="anode" || s=="cathode") {commonAnode=s=="anode";setColor(-1);}
  else if(s=="stop") {stopMotors();Serial.println("STOP: all motor pins LOW");}
  else if(s=="status") Serial.printf("v2 color=%d button_raw=%d clicks=%lu motors=OFF\n",selected,digitalRead(buttonPin),(unsigned long)presses);
  else Serial.println("Commands: red green blue off anode cathode stop status. Motor tests disabled.");
}
void setup() {
  for(auto p:motorPins) {digitalWrite(p,LOW);pinMode(p,OUTPUT);}
  stopMotors();
  pinMode(33,INPUT); // Servo disconnected.
  for(auto p:rgbPins) {digitalWrite(p,LOW);pinMode(p,OUTPUT);}
  pinMode(buttonPin,INPUT); // External pull-down, active HIGH as in old firmware.
  rawButton=stableButton=digitalRead(buttonPin);
  buttonChanged=millis();
  Serial.begin(115200);
  command.reserve(40);
  display.init();
  display.setRotation(1);
  pinMode(4,OUTPUT);digitalWrite(4,HIGH);
  setColor(0);
  Serial.println("TANK DISPLAY TEST v2. Button cycles RED/GREEN/BLUE. Motors disabled.");
}
void loop() {
  stopMotors();
  int value=digitalRead(buttonPin);
  if(value!=rawButton) {rawButton=value;buttonChanged=millis();}
  if(rawButton!=stableButton && millis()-buttonChanged>=40) {
    stableButton=rawButton;
    Serial.printf("BUTTON raw=%d\n",stableButton);
    if(stableButton==HIGH) {presses++;setColor((selected+1)%3);}
  }
  // Bound serial work so button polling is not starved.
  for(int n=0;n<64 && Serial.available();n++) {
    char c=Serial.read();
    if(c=='\n' || c=='\r') {if(command.length()) {handle(command);command="";}}
    else if(command.length()<32) command+=c;
    else {command="";stopMotors();}
  }
}
