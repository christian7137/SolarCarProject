/***************************************************
 -
 - File: svtSwitch.cpp
 -
 - Purpose: defines class for monitoring a digital pin as switch with debounce
 -
 - Author: J. C. Wiley, July 2015
 -
 - Reference:
 -      1. A Guide to Debouncing, Jack G. Ganssle, jack@ganssle.com
 -
 ****************************************************/
#include "svtSwitch.h"
#include "RawSerial.h"
extern RawSerial udebug;

/******************************************************************************
 -
 -   Destructor
 -
*******************************************************************************/
svtSwitch::~svtSwitch(){

}

/******************************************************************************
 -
 -   Down() - returns true when switch first pressed
 -
*******************************************************************************/
bool svtSwitch::Down(void){
    //udebug.printf("sw %d %x\r\n", read(), DownState);
    DownState = (DownState << 1) | (read() & 0x1) | 0xe000;
    if(DownState == 0xefff) return true;
    return false;
}

/******************************************************************************
 -
 -   Up() - returns true when switch first released
 -
*******************************************************************************/
bool svtSwitch::Up(void){
    UpState = (UpState << 1) | read() | 0xe000;
    if(UpState == 0xefff) return true;
    return false;
}
