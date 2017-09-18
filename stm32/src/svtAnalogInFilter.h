/***************************************************
 -
 - File: svtAnalogInFilter.h
 -
 - Purpose: smooths analogIn input
 -
 - Author: J. C. Wiley, July 2015
 -
 - Reference:
 -
 ****************************************************/
#include "mbed.h"

class svtAnalogInFilter: public AnalogIn {
public:
svtAnalogInFilter(PinName apin): AnalogIn(apin){
	oldx = 0;
    };
    uint16_t readu16Smooth();

private:
    uint16_t oldx;   

};
