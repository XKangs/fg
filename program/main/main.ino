/*
** Author: xKang
** Date: 21/09/2019
** todo:
** 1. adding code for hall effect sensor
*/
#include <LiquidCrystal.h>
#include <L298N.h>
#include <AnalogKeypad.h>

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define INTERPUT_PIN 2 //over current interput
#define LDR_PIN A1
#define REED_TOP_PIN 13
#define REED_BOTTOM_PIN A3
#define HES_PIN A2
#define BUTTON_PIN A0

#define TIME_TO_OPEN 13
#define LIGHT_TO_OPEN 150

#define TIME_TO_CLOSE 13
#define LIGHT_TO_CLOSE 150

enum status { //waiting for...
  timeOpen,
  lightOpen,
  opening,
  timeClose,
  lightClose,
  closing,
  overheat
  //eStop
};
AnalogKeypad _kp(A0);
L298N _motor(11, 12, 10);
LiquidCrystal _lcd(8, 9, 4, 5, 6, 7);
//L298N _motor(EN, IN1, IN2);
//LiquidCrystal _lcd(rs, en, d4, d5, d6, d7);

status _status = timeOpen;
String _lcdText[2];
tmElements_t tm;

void setup() {
  Serial.begin(9600);

  pinMode(INTERPUT_PIN, INPUT);
  //attachInterrupt(digitalPinToInterrupt(INTERPUT_PIN), overHeatAction, LOW);

  pinMode(LDR_PIN, INPUT);
  pinMode(REED_TOP_PIN, INPUT);
  pinMode(REED_BOTTOM_PIN, INPUT);

  _lcd.begin(16, 2);
  //_lcd.autoscroll();

}

void loop() {
  switch (_status) {
    case timeOpen: {
        timeOpenFun();
        break;
      }
    case lightOpen: {
        lightOpenFun();
        break;
      }
    case opening: {
        openingFun();
        break;
      }
    case timeClose: {
        timeCloseFun();
        break;
      }
    case lightClose:{
      lightCloseFun();
      break;
    }
    case closing:{
      closingFun();
      break;
    }
    case overheat: {
        overheatFun();
      }
    /*
    case eStop: {
        eStopFun();
      }
    */
  }
}
/*
** statuses
*/
void timeOpenFun() {
  setText("waiting timer", 0);
  setText("manually open", 1);
  while (true) {
    if (RTC.read(tm)) {
      if (tm.Hour > TIME_TO_OPEN) {
        _status = lightOpen;
        return;
      }
      if (_kp.getKey() == AnalogKeypad::up) {
        _status = opening;
        return;
      }
    }
  }
}

void lightOpenFun() {
  setText("waiting light", 0);
  setText("manually open", 1);
  while (true) {
    //Serial.println(analogRead(LDR_PIN));
    if (analogRead(LDR_PIN) > LIGHT_TO_OPEN || _kp.getKey() == AnalogKeypad::up) {
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
  while (digitalRead(REED_TOP_PIN) == HIGH) {
    if (checkEStop()) {
      eStopFun();
    }
  }
  _motor.fastStop();
  delay(500);
  _motor.disable();
  _status = timeClose;
}

void timeCloseFun() {
  //todo
  delay(3000);

}
void lightCloseFun() {
  setText("waiting light", 0);
  setText("manually close", 1);
  while (true) {
    //Serial.println(analogRead(LDR_PIN));
    if (analogRead(LDR_PIN) < LIGHT_TO_CLOSE || _kp.getKey() == AnalogKeypad::down) {
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
  while (digitalRead(REED_BOTTOM_PIN) == HIGH) {
    if (checkEStop()) {
      eStopFun();
    }
  }
  _motor.fastStop();
  delay(500);
  _motor.disable();
  _status = lightOpen;
}
void overheatFun() {
  setText("over heat", 0);
  setText("pls clear jam and reset", 1);
  while(true){
    //waiting for reset
    delay(1000);
  }

}
void eStopFun() {
  setText("emergency stop", 0);
  setText("continue", 1);
  while(_kp.getKey() == AnalogKeypad::nullVal){
    return;
  }
}


/*
** other functions
*/
//for interput
void overHeatAction() {
  _motor.disable();
  _status = overheat;
}

bool checkEStop() {
  //return false; //!!error!!
  if (_kp.getKey() != AnalogKeypad::nullVal) {
    _motor.fastStop();
    _status = eStop;
    delay(500);
    _motor.disable();
    return true;
  }
  return false;
}

void setText(String text, int row) {
  if (text.equals(_lcdText[row])) {
    return;
  }
  if (line == 0) {
    _lcd.clear();
  } else {
    _lcd.setCursor(0, row);
  }
  _lcd.print(text);
}
