/*
 *  i2ckeypad.pde - keypad/I2C expander interface example for Arduino
 *
 *  Copyright (c) 2009 Angel Sancho <angelitodeb@gmail.com>
 *  All rights reserved.
 *
 *
 *  LICENSE
 *  -------
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  EXPLANATION
 *  -----------
 *  This example was tested with PCF8574P and SPARKFUN 4x3 keypad wired
 *  as you can see in the image i2ckeypad_example_schema.png, and with
 *  default pin mapping of i2ckeypad library
 *
 *  R1, R2 and R3 are 10K
 */

#include <Wire.h>
#include <i2ckeypad.h>

int interruptPin = 2;
i2ckeypad kpd = i2ckeypad(0x20, 4, 4);

void setup()
{
  Serial.begin(9600);

  Wire.begin();

  kpd.init();
  
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), inter, FALLING);
  
  Serial.println("Testing keypad");
}

void loop()
{
  /*
  char key = kpd.get_key();
  
  if(key != '\0') {
    Serial.print("from loop: ");
    Serial.println(key);  
  }  
  */
}

void inter(){
  Serial.println("inter"); 
  char key = kpd.get_key();
  
  //if(key != '\0') 
  {
      Serial.println(key);  
  }
}
