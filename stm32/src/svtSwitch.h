/***************************************************
 -
 - File: svtSwitch.h
 -
 - Purpose: defines class for monitoring a digital pin as switch with debounce
 -
 - Author: J. C. Wiley, July 2015
 -
 - Reference:
 -      1. A Guide to Debouncing, Jack G. Ganssle, jack@ganssle.com
 -
 ****************************************************/
#include "mbed.h"

class svtSwitch: public DigitalIn {
public:
    svtSwitch(PinName apin, PinMode amode): DigitalIn(apin, amode){
	DownState = 0;
	UpState = 0;
    };
   svtSwitch(PinName apin): DigitalIn(apin){
	DownState = 0;
	UpState = 0;
    };
    ~svtSwitch();
    bool Down(void);
    bool Up(void);
private:
    uint16_t UpState;   // Current debounce status
    uint16_t DownState; // Current debounce status
};
