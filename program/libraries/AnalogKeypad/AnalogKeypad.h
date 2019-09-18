#ifndef AnalogKeypad_h
#define AnalogKeypad_h
#endif

#include "Arduino.h"

class AnalogKeypad {
public:
	typedef enum {
		select,
		up,
		down,
		left,
		right,
		nullVal
	} KeyVal;
	AnalogKeypad(int pin);
	KeyVal getKey();
private:
	int _pin;
};
