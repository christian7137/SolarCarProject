/***************************************************
 -
 - File: svtAnalogInFilter.cpp
 -
 - Purpose: smooths analogIn input
 -
 - Author: J. C. Wiley, July 2015
 -
 - Reference:
 -
 ****************************************************/
#include "svtAnalogInFilter.h"

uint16_t svtAnalogInFilter::readu16Smooth(){
    uint16_t del, x;
    x = read_u16();
    if(x > oldx){
        del = x - oldx;
	oldx = oldx + (del>>4);
    }
    else{
        del = oldx - x;
        oldx = oldx - (del>>4); 
    }
    return oldx;
}
