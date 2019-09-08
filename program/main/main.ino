#include <LiquidCrystal.h>
#include <L298N.h>

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define INTERPUT_PIN 2 //over current interput
#define LDR_PIN 5
#define REED_PIN 8
#define BUTTON_PIN 9

#define TIME_TO_OPEN 13
#define LIGHT_TO_OPEN 500

#define TIME_TO_CLOSE 13
#define LIGHT_TO_CLOSE 500

tmElements_t tm;


enum status { //waiting for...
  timeOpen,
  lightOpen,
  opening,
  timeClose,
  lightClose,
  closing,
  overheat,
  eStop
};

L298N motor(11, 12, 13);
LiquidCrystal LCD(9, 10, 4, 5, 6, 7);
//L298N motor(EN, IN1, IN2);
//LiquidCrystal LCD(rs, en, d4, d5, d6, d7);
status _status = timeOpen;
char _LCDText[2][16];

void setup() {
  //interput...
  pinMode(INTERPUT_PIN, INPUT); 
  attachInterrupt(digitalPinToInterrupt(INTERPUT_PIN), overHeatAction, LOW);
  
  pinMode(LDR_PIN, INPUT);
  pinMode(REED_PIN, INPUT);

}

void loop() {
  switch(_status){
    case timeOpen:{
      timeOpenFun();
      break;
    }
    case lightOpen:{
      lightOpenFun();
      break;
    }
    case opening:{
      openingFun();
      break;
    }
    case timeClose:{
      timeCloseFun();
      break;
    }
    //...
    case overheat:{
      overheatFun();
    }
    case eStop:{
      eStopFun();
    }
  }
  //read button
  //update screen

}

void timeOpenFun(){
  if (RTC.read(tm)) {
    if(tm.Hour > TIME_TO_OPEN){
      _status = lightOpen;
    }else{
      setText("waiting timer", 0);
      setText("manual open", 1);
    }
  }
}

void lightOpenFun(){
  if(analogRead(LDR_PIN) > LIGHT_TO_OPEN){
    _status = opening;
  }else{
    setText("waiting light", 0);
    setText("manual open", 1);
  }
}
void openingFun(){ //it would not jump back to main switch, so I have to deal with button press
  setText("opening", 0);
  setText("emergency stop", 1);
  motor.setSpeed(225);
  motor.forward();
  while(digitalRead(REED_PIN) == HIGH){
    if(checkEStop()){
      _status = eStop;
      return;
    }
  }
  while(digitalRead(REED_PIN) ==  LOW){
    if(checkEStop()){
      _status = eStop;
      return;
    }
  }
  motor.fastStop();
  delay(500);
  motor.disable();
  _status = lightClose;

}
void timeCloseFun(){

}
void lightCloseFun(){

}
void closeingFun(){

}
void overHeatAction(){
  motor.disable();
  _status = overheat;
}
void overheatFun(){
  setText("over heat", 0);
  setText("pls clear jam and reset", 1);
  //waiting for reset
}
void eStopFun(){
  //just waiting for continue
}

bool checkEStop(){
  if(BUTTON_PIN > 0 && BUTTON_PIN < 200){
    motor.fastStop();
    delay(500);
    motor.disable();
    setText("emergency stop", 0);
    setText("continue", 1);
    return true;
  }
  return false;
}
void setText(String abc, int line){
  //strncpy(*_LCDText[line], text, sizeof(_LCDText[line]));
}
