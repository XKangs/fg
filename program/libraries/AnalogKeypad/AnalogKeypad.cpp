#include "AnalogKeypad.h"
AnalogKeypad::AnalogKeypad(int pin) {
	_pin = pin;
 pinMode(_pin, INPUT);
}

AnalogKeypad::KeyVal AnalogKeypad::getKey() {
	int val = analogRead(_pin);  // read the input pin
	if (val > 1000) {
		return nullVal;
	}
	else if (val > 600) {
		return select;
	}
	else if (val > 400) {
		return left;
	}
	else if (val > 250) {
		return down;
	}
	else if (val > 90) {
		return up;
	}
	else {
		return right;
	}
}
