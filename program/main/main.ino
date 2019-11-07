/*
** Author: xKang
** Date: 15/10/2019
** hall effect sensor is working now
* 
** todo:
** 1. make emergince stop work
*/
#include <LiquidCrystal.h>
#include <L298N.h>
#include <AnalogKeypad.h>

#include <Wire.h>
#include <TimeLib.h>
#include <Eeprom24C32_64.h>
#include <DS1307RTC.h>



#include "headers.h";
#include "settingConf.h";

AnalogKeypad _kp(A0);
L298N _motor(11, 12, 10);
LiquidCrystal _lcd(8, 9, 4, 5, 6, 7);
//L298N _motor(EN, IN1, IN2);
//LiquidCrystal _lcd(rs, en, d4, d5, d6, d7);

status _status = timeOpen;
mode _mode = programming; //!
String _lcdText[2];
tmElements_t tm;
int _turns = 0;

void setup() {
  Serial.begin(9600);
  _lcd.begin(16, 2);
  //_lcd.autoscroll();

  if(_kp.getKey() == AnalogKeypad::select){
  	_mode = programming;
  }
}

void loop() {
  switch(_mode){
  	case programming:{
  		pinMode(10,OUTPUT); //!
  		digitalWrite(10,HIGH); //!
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
** statuses
*/
void timeOpenFun() {
  setText("waiting timer", 0);
  setText("manually open", 1);
  //I am not going to wait for that stupid timmer, I just puase 3s.  comment next 3 line to enable it
  delay(3000);
  _status = lightOpen;
  return;
  while (true) {
    if (RTC.read(tm)) {
      if (tm.Hour > TIME_TO_OPEN) {
        _status = lightOpen;
        return;
      }
    }
    if (_kp.getKey() == AnalogKeypad::up) {
		_status = opening;
		return;
    }
  }
}

void lightOpenFun() {
  setText("waiting light", 0);
  setText("manually open", 1);
  while (true) {
    if (analogRead(LDR_PIN) > LIGHT_TO_OPEN){
      _status = opening;
      return;
    }
    if (_kp.getKey() == AnalogKeypad::up) {
      _status = opening;
      return;
    }
  }
}

void openingFun() {
  setText("opening", 0);
  setText("emergency stop", 1);
  _motor.setSpeed(225);
  _motor.forward();
  while (digitalRead(REED_TOP_PIN) == HIGH && _turns < 5) {} //!
  _motor.fastStop();
  delay(500);
  _motor.disable();
  _status = timeClose;
}

void timeCloseFun() {
  setText("waiting timer", 0);
  setText("manually close", 1);
  delay(3000); //same as time open function
  _status = lightClose;
  return;
}
void lightCloseFun() {
  setText("waiting light", 0);
  setText("manually close", 1);
  while (true) {
    if (analogRead(LDR_PIN) < LIGHT_TO_CLOSE){
      _status = closing;
      return;
    }
    if(_kp.getKey() == AnalogKeypad::down) {
      _status = closing;
      return;
    }
  }
}
void closingFun() {
  setText("closing", 0);
  setText("emergency stop", 1);
  _motor.setSpeed(225);
  _motor.backward();
  while (digitalRead(REED_BOTTOM_PIN) == HIGH && _turns!= 0) {} //!
  _motor.fastStop();
  delay(500);
  _motor.disable();
  _status = timeOpen;
}
void overheatFun() {
  setText("over heat", 0);
  setText("pls clear jam and reset", 1);
  while(true){
    //waiting for reset
    delay(1000);
  }

}
/*
void eStopFun() {
  setText("emergency stop", 0);
  setText("continue", 1);
  while(true){
    if(_kp.getKey() == AnalogKeypad::select){
      while(_kp.getKey() == AnalogKeypad::select){}
      return;
    }
  }
}
*/
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

  //store settings //!
  setText("setting finish", 0);
  setText("please reboot", 1);
  while(true){delay(10000);}
}
/*
** other functions
*/
//for interput
void overHeatAction() {
  _motor.disable();
  _status = overheat;
}
//for interput
void HESAction(){
  if(_status == opening){
    _turns++;
  }else if(_status == closing){
    _turns--;
  }
  Serial.println(_turns);
}
/*
void checkEStop() {
  //return false; //!!error!!
  if (_kp.getKey() == AnalogKeypad::select) {
    _motor.fastStop();
    while(_kp.getKey() == AnalogKeypad::select){}
    _motor.disable();
    eStopFun();
  }
}
*/
void setText(String text, int row) {
  if (text.equals(_lcdText[row])) {
    return;
  }
  if (row == 0) {
    _lcd.clear();
  } else {
    _lcd.setCursor(0, row);
  }
  _lcd.print(text);
}

void runningSetup(){
	pinMode(INTERPUT_PIN, INPUT);
	//attachInterrupt(digitalPinToInterrupt(INTERPUT_PIN), overHeatAction, LOW);
	pinMode(HES_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(HES_PIN), HESAction, RISING);


	pinMode(LDR_PIN, INPUT);
	pinMode(REED_TOP_PIN, INPUT);
	pinMode(REED_BOTTOM_PIN, INPUT);
}



settingStore::settingStore(){
  //this._eeprom(0x50);
  this->_eeprom.initialize();  
}

void settingStore::read(int baseAddress = 0){
  int count = 6;
  char buffer[count] = { 0 };
  this->_eeprom.readBytes(baseAddress, count, buffer);
  _defaultMode = (mode)buffer[0];
    _max_HES_turns = buffer[1];
  _lightOn = buffer[2] * 25;
  _lightOff = buffer[3] * 25;
  _timeOn = buffer[4];
  _timeOff = buffer[5];
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
  buffer[0] = _defaultMode;
  buffer[1] = _max_HES_turns;
  buffer[2] = _lightOn / 25;
  buffer[3] = _lightOff / 25;
  buffer[4] = _timeOn;
  buffer[5] = _timeOff;
  this._eeprom.writeBytes(baseAddress, count, inputBytes);
}
