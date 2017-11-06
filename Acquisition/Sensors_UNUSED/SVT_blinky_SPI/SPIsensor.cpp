/************************************
  -
  - file: SPIsensor.cpp  
  -
  - purpose: instance of svtSensor for a blinking green LED.
  -
  - author: J. C. Wiley, Sept. 2014
  -      Derived from mbed.org example blinky
  -
*************************************/
#include "SPIsensor.h"

extern RawSerial udebug;
extern SPI spi(PA_7, PA_6, PA_5);
extern DigitalOut cs(PD_14); 

/** SPIsensor - Constructor in header
 */

/** ~SPIsensor - Destructor
 */
SPIsensor:: ~SPIsensor(){
}

/** init - initializes after construction (overrides)
 */
int SPIsensor::init(){
    //_ledG.write(1);
    return 0;
}

/** readSensor - overrides virtual base class function
 */
int SPIsensor::readSensor(){
    cs = 0;
    int data = spi.write(0x55);
    udebug.printf("in SPI::readSensor \%d\r\n", data);
    CANMessage msg;
    msg.id     = CANbase + 1;
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = SVTCANFORMAT;
    msg.data[0] =  1;//_ledG.read();
    msg.data[1] =  counter++;
    msg.data[2] =  2;
    msg.data[3] =  3; 
    msg.data[4] =  4;
    msg.data[5] =  5;
    msg.data[6] =  0xde;
    msg.data[7] =  0xad;
    cs = 1;
    if(sendOutMsg(msg) != 0){
     	udebug.printf("SPIsensor::readSensor error in sendOutMsg 1 \r\n"); 
    }
    return 0;
}

/** command - receive command via CAN message
 *     a command may tell sensor to change internal settings
 *     or initiate an activity
 */
int SPIsensor::command(CANMessage& m){
    switch( m.id & 0x7FF ){
    case 0x471:
    	//udebug.printf("GREEN: %x G_LED from Motor, cnt %x \r\n", m.id, m.data[1]);
    	break;
    case 0x371:
        //udebug.printf("GREEN: %x G_LED from TexsunOne, cnt %x \r\n", m.id, m.data[1]);
        break;
    case 0x411:
        udebug.printf("GREEN: %x gap msg, cnt %x \r\n", m.id, m.data[3]);
    	break;
    default:
        break;
    }  
    return 0;
}

/** update - update is called at the requested interval
 */
void SPIsensor::update(){
    //_ledG = !_ledG;
    readSensor();
}
