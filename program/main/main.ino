/*
** Author: xKang
** Date: 15/10/2019
*/
#include <LiquidCrystal.h>
#include <L298N.h>
#include <AnalogKeypad.h>

#include <Wire.h>
#include <TimeLib.h>
#include <Eeprom24C32_64.h>
#include <DS1307RTC.h>
#include "headers.h";

static Eeprom24C32_64 _eeprom(EEPROM_ADDRESS);
AnalogKeypad _kp(A0);
//L298N _motor(EN, IN1, IN2);
L298N _motor(11, 12, 10);
//LiquidCrystal _lcd(rs, en, d4, d5, d6, d7);
LiquidCrystal _lcd(8, 9, 4, 5, 6, 7);

mode _mode = combine;
settingStore _conf;
tmElements_t tm;
int _turns = 0;

void setup() {
  Serial.begin(9600);
  _lcd.begin(16, 2);
  if(_kp.getKey() == AnalogKeypad::select){
  	_mode = programming;
  }else{
    runningSetup();
  }
}

void loop() {
  switch(_mode){
  	case programming:{
      programmingMode();
  		break;
  	}
  	case lightSensor:{
  		lightOpenFun();
  		openingFun();
  		lightCloseFun();
  		closingFun();
  		break;
  	}
  	case timmer:{
  		timeOpenFun();
  		openingFun();
  		timeCloseFun();
  		closingFun();
  		break;
  	}
  	case combine:
  	default:{
  		timeOpenFun();
  		lightOpenFun();
  		openingFun();
  		timeCloseFun();
  		lightCloseFun();
  		closingFun();
  		break;
  	}
  }
}
/*
** other functions
*/
void setText(String text, int row,int col=0){
  if (row == 0) {
    _lcd.clear();
  }
  _lcd.setCursor(col, row);
  _lcd.print(text);
}

/*
** statuses
*/
void timeOpenFun() {
  setText("waiting timer", 0);
  setText(("/"+String(_conf._timeOn)), 1, 14);
  while (true) {
    if (RTC.read(tm)) {
      if (tm.Hour > _conf._timeOn) {
        return;
      }
    }
    if (_kp.getKey() == AnalogKeypad::up) {
  	  return;
    }  
    setText(String(tm.Hour,DEC),1);
    delay(500);
  }
}

void lightOpenFun() {
  setText("waiting light", 0);
  setText("/"+String(_conf._lightOn), 1, 16-4);
  int luxReading;
  while (true) {
    luxReading = analogRead(LDR_PIN);
    if (luxReading > _conf._lightOn){
      return;
    }
    if (_kp.getKey() == AnalogKeypad::up) {
      return;
    }
    setText(String(luxReading,DEC), 1);
    delay(500);
  }
}

void openingFun() {
  setText("opening", 0);
  setText("/"+String(_conf._max_HES_turns), 1, 16-3);
  _turns = 0;
  _motor.setSpeed(225);
  _motor.forward();
  while (digitalRead(REED_TOP_PIN) == HIGH && _turns < _conf._max_HES_turns) {
    setText(String(_turns,DEC), 1);
    delay(500);
   }
  _motor.fastStop();
  delay(500);
  _motor.disable();
}

void timeCloseFun() {
  setText("waiting light", 0);
  setText("/"+String(_conf._lightOff), 1, 16-4);
  int luxReading;
  while (true) {
    luxReading = analogRead(LDR_PIN);
    if (luxReading < _conf._lightOff){
      return;
    }
    if (_kp.getKey() == AnalogKeypad::down) {
      return;
    }
    setText(String(luxReading,DEC), 1);
    delay(500);
  }
}
void lightCloseFun() {
  setText("waiting light", 0);
  setText("/"+String(_conf._lightOff), 1, 16-4);
  int luxReading;
  while (true) {
    luxReading = analogRead(LDR_PIN);
    if (luxReading < _conf._lightOff){
      return;
    }
    if (_kp.getKey() == AnalogKeypad::down) {
      return;
    }
    setText(String(luxReading,DEC), 1);
    delay(500);
  }
}
void closingFun() {
  setText("closing", 0);
  setText("/"+String(_conf._max_HES_turns), 1, 16-3);
  _turns = 0;
  _motor.setSpeed(225);
  _motor.backward();
  while (digitalRead(REED_BOTTOM_PIN) == HIGH && _turns < _conf._max_HES_turns) {
    setText(String(_turns,DEC), 1);
    delay(500);
   }
  _motor.fastStop();
  delay(500);
  _motor.disable();
}

/* 
 * initialize function
*/
void runningSetup(){
  pinMode(INTERPUT_PIN, INPUT);
  //attachInterrupt(digitalPinToInterrupt(INTERPUT_PIN), overHeatAction, LOW);
  pinMode(HES_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(HES_PIN), HESAction, RISING);

  pinMode(LDR_PIN, INPUT);
  pinMode(REED_TOP_PIN, INPUT);
  pinMode(REED_BOTTOM_PIN, INPUT);
  _conf.read();
}

void programmingMode(){
	mode defaultMode = combine;
  int max_HES_turns = 1,lightOn = 150,lightOff = 150,timeOn = 8, timeOff = 16;
  setText("programming mode", 0);
  setText("", 1);
  delay(1000);

//section 
  setText("default mode", 0);
  while(_kp.getKey() != AnalogKeypad::select){
    setText(modeToString(defaultMode) + " mode       ", 1);
    if(_kp.getKey() == AnalogKeypad::up && defaultMode != 0){
      defaultMode = defaultMode - 1;
    }
    if(_kp.getKey() == AnalogKeypad::down && defaultMode != 2){
      defaultMode = defaultMode + 1;
    }
    delay(500);
  }
  while(_kp.getKey() == AnalogKeypad::select){}


  //section 
  setText("door hight", 0);
  while(_kp.getKey() != AnalogKeypad::select){
    setText(String(max_HES_turns) + "     ", 1);
    if(_kp.getKey() == AnalogKeypad::up && max_HES_turns != 99){
      max_HES_turns += 1;
    }
    if(_kp.getKey() == AnalogKeypad::down && max_HES_turns != 1){
      max_HES_turns -= 1;
    }
    delay(500);
  }
  while(_kp.getKey() == AnalogKeypad::select){}

  //section 
  setText("light to open", 0);
  while(_kp.getKey() != AnalogKeypad::select){
    setText(String(lightOn) + "     ", 1);
    if(_kp.getKey() == AnalogKeypad::up && lightOn != 1000){
      lightOn += 25;
    }
    if(_kp.getKey() == AnalogKeypad::down && lightOn != 0){
      lightOn -= 25;
    }
    delay(500);
  }
  while(_kp.getKey() == AnalogKeypad::select){}

  //section 
  setText("light to close", 0);
  while(_kp.getKey() != AnalogKeypad::select){
    setText(String(lightOff) + "     ", 1);
    if(_kp.getKey() == AnalogKeypad::up && lightOff != 1000){
      lightOff += 25;
    }
    if(_kp.getKey() == AnalogKeypad::down && lightOff != 0){
      lightOff -= 25;
    }
    delay(500);
  }
  while(_kp.getKey() == AnalogKeypad::select){}

  //section 
  setText("time to open", 0);
  setText("        (24 hrs)", 1);
  while(_kp.getKey() != AnalogKeypad::select){
    setText(String(timeOn) + ":00  ", 1);
    if(_kp.getKey() == AnalogKeypad::up && timeOn != 9){
      timeOn += 1;
    }
    if(_kp.getKey() == AnalogKeypad::down && timeOn != 0){
      timeOn -= 1;
    }
    delay(500);
  }
  while(_kp.getKey() == AnalogKeypad::select){}

  //section 
  setText("time to close", 0);
  setText("        (24 hrs)", 1);
  while(_kp.getKey() != AnalogKeypad::select){
    setText(String(timeOff) + ":00", 1);
    if(_kp.getKey() == AnalogKeypad::up && timeOff != 23){
      timeOff += 1;
    }
    if(_kp.getKey() == AnalogKeypad::down && timeOff != 13){
      timeOff -= 1;
    }
    delay(500);
  }
  while(_kp.getKey() == AnalogKeypad::select){}
  
  _conf.write(defaultMode,max_HES_turns,lightOn,lightOff,timeOn,timeOff,0);
  setText("setting finish", 0);
  setText("please reboot", 1);
  while(true){delay(10000);}
}

/*
 * for interput
*/
void overHeatAction() {
  _motor.disable();
  setText("over heat", 0);
  setText("pls clear jam and reset", 1);
  //waiting for reset
  while(true){
    delay(10000);
  }
}
void HESAction(){
  Serial.println("!!");
  _turns++;
}

/*
 * class settingStore
 * I really should put it to another file
*/
settingStore::settingStore(){
  _eeprom.initialize();  
}

void settingStore::read(int baseAddress = 0){
  int count = 6;
  char buffer[count] = { 0 };
  _eeprom.readBytes(baseAddress, count, buffer);
  
  this->_defaultMode = (mode)buffer[0]; 
  this->_max_HES_turns = buffer[1];
  this->_lightOn = buffer[2] * 25;
  this->_lightOff = buffer[3] * 25;
  this->_timeOn = buffer[4];
  this->_timeOff = buffer[5];
}
void settingStore::write(
  mode defaultMode, 
  int max_HES_turns,
  int lightOn,
  int lightOff,
  int timeOn,
  int timeOff,
  int baseAddress = 0
){
  int count = 6;
  char buffer[count]  = { 0 };
  buffer[0] = defaultMode;
  buffer[1] = max_HES_turns;
  buffer[2] = lightOn / 25;
  buffer[3] = lightOff / 25;
  buffer[4] = timeOn;
  buffer[5] = timeOff;
  _eeprom.writeBytes(baseAddress, count, buffer);
}
