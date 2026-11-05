#include <Arduino.h>
#include <TFT_eSPI.h>
#include <esp_timer.h>
constexpr uint8_t motorPins[]={13,21,15,2,17,22};
constexpr uint8_t rgbPins[]={27,26,25};
constexpr uint8_t buttonPin=32;
constexpr uint32_t pulseUs=750000;
constexpr uint8_t duty=128;
struct Test {const char* name;uint8_t channel;uint8_t highPin;bool left;bool forward;};
// Owner verified all four directions after correcting left polarity; see docs/HARDWARE.md.
const Test tests[]={
 {"LINKS VOR",7,17,true,true},
 {"LINKS ZURUECK",7,22,true,false},
 {"RECHTS VOR",6,15,false,true},
 {"RECHTS ZURUECK",6,2,false,false}
};
TFT_eSPI tft;
esp_timer_handle_t timerHandle=nullptr;
volatile bool active=false;
bool shownActive=false;
int nextStep=0,lastStep=-1;
int raw=LOW,stable=LOW;
bool readyForPress=false;
uint32_t changedAt=0;
String command;
void stopMotors(){
 ledcWrite(6,0);ledcWrite(7,0);
 for(auto p:motorPins) digitalWrite(p,LOW);
 active=false;
}
void timeout(void*){stopMotors();}
void arrow(int x,int y,bool forward,uint16_t color){
 int tip=forward?y-20:y+20,tail=forward?y+20:y-20;
 tft.fillRect(x-3,min(tip,tail),7,abs(tip-tail),color);
 if(forward)tft.fillTriangle(x,tip-5,x-11,tip+9,x+11,tip+9,color);
 else tft.fillTriangle(x,tip+5,x-11,tip-9,x+11,tip-9,color);
}
void draw(){
 tft.fillScreen(TFT_BLACK);
 tft.setTextColor(TFT_WHITE,TFT_BLACK);
 tft.drawCentreString("TANK TEST",67,3,2);
 tft.setTextColor(active?TFT_ORANGE:TFT_GREEN,TFT_BLACK);
 tft.drawCentreString(active?"MOTORTEST":"STOP",67,23,2);
 int step=lastStep<0?nextStep:lastStep;
 const Test& test=tests[step];
 tft.setTextColor(TFT_WHITE,TFT_BLACK);
 tft.drawCentreString(String(step+1)+"/4 "+test.name,67,47,1);
 tft.setTextColor(TFT_CYAN,TFT_BLACK);
 tft.drawCentreString("VORNE",67,68,2);
 tft.drawRoundRect(17,96,35,67,4,TFT_DARKGREY);
 tft.drawRoundRect(83,96,35,67,4,TFT_DARKGREY);
 tft.drawRect(55,101,25,57,TFT_DARKGREY);
 arrow(test.left?34:100,129,test.forward,active?TFT_ORANGE:TFT_WHITE);
 tft.setTextColor(TFT_WHITE,TFT_BLACK);
 tft.drawCentreString("L",34,166,2);tft.drawCentreString("R",100,166,2);
 tft.setTextColor(TFT_LIGHTGREY,TFT_BLACK);
 tft.drawCentreString("750 ms / PWM 50%",67,190,1);
 tft.setTextColor(TFT_CYAN,TFT_BLACK);
 tft.drawCentreString("Naechster: "+String(nextStep+1)+"/4",67,205,1);
 tft.setTextColor(TFT_WHITE,TFT_BLACK);
 tft.drawCentreString(active?"Taster = STOP":"Taster = Start",67,222,1);
 for(int i=0;i<3;i++)digitalWrite(rgbPins[i],i==(active?2:0));
}
void startStep(){
 stopMotors();esp_timer_stop(timerHandle);
 lastStep=nextStep;
 const Test& test=tests[lastStep];
 active=true;
 if(esp_timer_start_once(timerHandle,pulseUs)!=ESP_OK){stopMotors();draw();return;}
 digitalWrite(test.highPin,HIGH);
 ledcWrite(test.channel,duty);
 nextStep=(nextStep+1)%4;
 Serial.printf("TEST %d %s highGPIO=%d enableGPIO=%d 750ms PWM128\n",lastStep+1,test.name,test.highPin,test.channel==6?13:21);
 shownActive=true;draw();
}
void setup(){
 for(auto p:motorPins){digitalWrite(p,LOW);pinMode(p,OUTPUT);}
 ledcSetup(6,1000,8);ledcSetup(7,1000,8);
 ledcWrite(6,0);ledcWrite(7,0);
 ledcAttachPin(13,6);ledcAttachPin(21,7);stopMotors();
 esp_timer_create_args_t args={};args.callback=&timeout;args.name="stopMotor";
 ESP_ERROR_CHECK(esp_timer_create(&args,&timerHandle));
 pinMode(33,INPUT);
 for(auto p:rgbPins){digitalWrite(p,LOW);pinMode(p,OUTPUT);}
 pinMode(buttonPin,INPUT);
 raw=stable=digitalRead(buttonPin);changedAt=millis();
 // Require a stable release after boot; held button must not start a motor.
 Serial.begin(115200);command.reserve(32);
 tft.init();tft.setRotation(0);pinMode(4,OUTPUT);digitalWrite(4,HIGH);
 draw();Serial.println("TANK BUTTON TEST v4 | stopped | first press: LEFT FORWARD");
}
void loop(){
 int value=digitalRead(buttonPin);
 if(value!=raw){raw=value;changedAt=millis();}
 if(millis()-changedAt>=40){
  if(raw==LOW)readyForPress=true;
  if(raw!=stable){
   stable=raw;
   if(stable==HIGH && readyForPress){
    readyForPress=false;
    if(active){stopMotors();esp_timer_stop(timerHandle);Serial.println("BUTTON STOP");draw();}
    else startStep();
   }
  }
 }
 if(shownActive && !active){shownActive=false;Serial.println("STOPPED");draw();}
 for(int n=0;n<32 && Serial.available();n++){
  char c=Serial.read();
  if(c=='\n'||c=='\r'){
   if(command=="stop"){stopMotors();esp_timer_stop(timerHandle);draw();Serial.println("STOP");}
   else if(command=="status")Serial.printf("v4 active=%d next=%d button=%d\n",active,nextStep+1,digitalRead(buttonPin));
   else if(command=="reset" && !active){nextStep=0;lastStep=-1;draw();Serial.println("SEQUENCE RESET");}
   command="";
  }else if(command.length()<24)command+=c;
  else{command="";stopMotors();}
 }
}
