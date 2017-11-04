/************************************
  -
  - file: Luminositysensor.cpp  
  -
  - purpose: instance of svtSensor for a blinking green LED.
  -
  - author: J. C. Wiley, Sept. 2014
  -      Derived from mbed.org example blinky
  -
*************************************/
#include "mbed.h"
#include "LuminositySensor.h"
#include "TSL2561.h"

#define LM75_REG_TEMP (0x00) // Temperature Register
#define LM75_REG_CONF (0x01) // Configuration Register
#define LM75_ADDR     (0x90) // LM75 address

extern RawSerial udebug;
I2C i2c(I2C_SDA, I2C_SCL);
TSL2561 test(I2C_SDA, I2C_SCL);

/** Luminositysensor - Constructor in header
 */

/** ~Luminositysensor - Destructor
 */
Luminositysensor:: ~Luminositysensor(){
}

/** init - initializes after construction (overrides)
 */
int Luminositysensor::init(){
    test.enable();
    //udebug.printf("LuminositySensor Reading 22:%d\r\n", test.getLuminosity(0));
    return 0;
}

/** readSensor - overrides virtual base class function
 */
int Luminositysensor::readSensor(){    
    //data_read = test.getLuminosity(0);
    
    udebug.printf("LuminositySensor Reading :%d\r\n", test.getLuminosity(0));
    CANMessage msg;
    msg.id     = CANbase + 1;
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = SVTCANFORMAT;
    msg.data[0] =  0;
    msg.data[1] =  1;
    msg.data[2] =  2;
    msg.data[3] =  3; 
    msg.data[4] =  4;
    msg.data[5] =  5;
    msg.data[6] =  0xde;
    msg.data[7] =  0xad;
    if(sendOutMsg(msg) != 0){
     	udebug.printf("Luminositysensor::readSensor error in sendOutMsg 1 \r\n"); 
    }
    return 0;
}

/** command - receive command via CAN message
 *     a command may tell sensor to change internal settings
 *     or initiate an activity
 */
int Luminositysensor::command(CANMessage& m){
    switch( m.id & 0x7FF ){
    case 0x471:
    	//udebug.printf("GREEN: %x G_LED from Motor, cnt %x \r\n", m.id, m.data[1]);
    	break;
    case 0x371:
        //udebug.printf("GREEN: %x G_LED from TexsunOne, cnt %x \r\n", m.id, m.data[1]);
        break;
    case 0x411:
        udebug.printf("LUMIN: %x gap msg, cnt %x \r\n", m.id, m.data[3]);
    	break;
    default:
        break;
    }  
    return 0;
}

/** update - update is called at the requested interval
 */
void Luminositysensor::update(){
    readSensor();
    //Thread::wait(400);
}
