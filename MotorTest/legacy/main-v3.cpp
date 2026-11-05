#include <Arduino.h>
#include <TFT_eSPI.h>
#include <esp_timer.h>
// v3: explicitly armed, one-shot motor tests only; button always cancels.
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
esp_timer_handle_t motorTimer=nullptr;
volatile bool motorActive=false;
bool armed=false, wasActive=false;
uint32_t armedAt=0;
String lastTest="KEIN TEST";
void stopMotors() {
  ledcWrite(6,0);ledcWrite(7,0);
  for(auto p:motorPins) digitalWrite(p,LOW);
  motorActive=false;
}
void timeoutMotor(void*) {stopMotors();}

void drawStatus() {
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE,TFT_BLACK);
  display.drawString("TANK LAB",10,5,2);
  display.setTextColor(motorActive?TFT_ORANGE:TFT_GREEN,TFT_BLACK);
  display.drawRightString(motorActive?"TEST 750ms":"MOTOREN AUS",230,5,2);
  display.drawRoundRect(8,29,164,55,10,TFT_DARKGREY);
  for(int i=0;i<3;i++) {
    int x=36+i*54;
    display.fillCircle(x,56,19,selected==i?colors[i]:TFT_DARKGREY);
    display.drawCircle(x,56,20,selected==i?TFT_WHITE:TFT_DARKGREY);
  }
  display.setTextColor(selected>=0?colors[selected]:TFT_WHITE,TFT_BLACK);
  display.drawString(selected>=0?names[selected]:"AUS",182,48,2);
  display.setTextColor(TFT_WHITE,TFT_BLACK);
  display.drawString("Taster: STOP / Farbe",10,91,2);
  display.setTextColor(TFT_LIGHTGREY,TFT_BLACK);
  display.drawString(lastTest+" | Klicks: "+String(presses),10,113,2);
}
void setColor(int channel) {
  selected=channel;
  for(int i=0;i<3;i++) digitalWrite(rgbPins[i],(i==selected)!=commonAnode);
  drawStatus();
  Serial.printf("COLOR %s\n",selected>=0?names[selected]:"AUS");
}
void handle(String s) {
  s.trim();
  if(s=="arm") {
    stopMotors();armed=true;armedAt=millis();
    Serial.println("ARMED: one pulse, expires in 5 seconds");return;
  }
  if(s=="p13a+" || s=="p13a-" || s=="p13b+" || s=="p13b-" ||
     s=="p21a+" || s=="p21a-" || s=="p21b+" || s=="p21b-") {
    bool allowed=armed && millis()-armedAt<5000 && digitalRead(buttonPin)==LOW;
    armed=false;stopMotors();esp_timer_stop(motorTimer);
    if(!allowed) {Serial.println("REFUSED: arm first; release button");return;}
    uint8_t highPin=s[3]=='a'?(s[4]=='+'?15:2):(s[4]=='+'?17:22);
    uint8_t channel=s.startsWith("p13")?6:7;
    lastTest=s;motorActive=true;
    // Independent timer ends pulse even if rendering or serial handling stalls.
    if(esp_timer_start_once(motorTimer,750000)!=ESP_OK) {
      stopMotors();Serial.println("REFUSED: timer error");return;
    }
    digitalWrite(highPin,HIGH);
    ledcWrite(channel,128); // 50% duty, one enable only.
    Serial.println("PULSE "+s+" 750ms PWM128/255");
    drawStatus();return;
  }
  if(motorActive) {stopMotors();armed=false;esp_timer_stop(motorTimer);}

  if(s=="red") setColor(0);
  else if(s=="green") setColor(1);
  else if(s=="blue") setColor(2);
  else if(s=="off") setColor(-1);
  else if(s=="anode" || s=="cathode") {commonAnode=s=="anode";setColor(-1);}
  else if(s=="stop") {stopMotors();armed=false;esp_timer_stop(motorTimer);drawStatus();Serial.println("STOP: all motor outputs OFF");}
  else if(s=="status") Serial.printf("v3 color=%d button_raw=%d clicks=%lu motor_active=%d armed=%d\n",selected,digitalRead(buttonPin),(unsigned long)presses,motorActive,armed);
  else Serial.println("Commands: red green blue off anode cathode stop status. arm + p13a+/p13a-/p13b+/p13b-/p21a+/p21a-/p21b+/p21b-.");
}
void setup() {
  for(auto p:motorPins) {digitalWrite(p,LOW);pinMode(p,OUTPUT);}
  ledcSetup(6,1000,8);ledcSetup(7,1000,8);
  ledcWrite(6,0);ledcWrite(7,0);
  ledcAttachPin(13,6);ledcAttachPin(21,7);
  stopMotors();
  esp_timer_create_args_t timerArgs={};
  timerArgs.callback=&timeoutMotor;timerArgs.name="motorStop";
  ESP_ERROR_CHECK(esp_timer_create(&timerArgs,&motorTimer));
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
  Serial.println("TANK MOTOR TEST v3. Default STOP. Pulses require arm. Button stops.");
}
void loop() {
  if(armed && millis()-armedAt>=5000) {armed=false;Serial.println("DISARMED");}
  if(digitalRead(buttonPin)==HIGH) {stopMotors();armed=false;}
  if(wasActive && !motorActive) {Serial.println("AUTO/LOCAL STOP");drawStatus();}
  wasActive=motorActive;
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
    else {command="";stopMotors();armed=false;}
  }
}
