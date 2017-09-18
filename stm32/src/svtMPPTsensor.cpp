/**

   File: svtMPPTsensor.cpp

   Purpose - Uses I2C interface to manage tire pressure sensor.
 
   Author - J. C. Wiley,  Feb. 2015

*/
#include "svtMPPTsensor.h"

#define MBED_CAN_HELPER_H 1

extern RawSerial udebug;

#define SEEEDSPEED 125000

/**
   Constructor -- in header
*/

/**
   destructor
*/
svtMPPTsensor::~svtMPPTsensor(){ 

};

/** init
    -
*/
int svtMPPTsensor::init(){
    if(_can.open(SEEEDSPEED) == 0){
	udebug.printf("seeed can open failed \r\n");
    }
    else{
         udebug.printf("seeed can opened \r\n");
         if(_can.frequency(SEEEDSPEED) == 1){
	     udebug.printf("speed set %d \r\n", SEEEDSPEED);
	 }
    }
    _can.setinterrupts(SEEED_CAN::RxAny); // tells MCP2515 to use interrupts - no error flag
    return 0;
}

/** reset - 
 */
void svtMPPTsensor::reset(){
    // hardware line not implimented yet.
}

/**  update -- process 
 */
void svtMPPTsensor::update(){ 
    if(msgReadyFlag){
	_can.read(_inmsg);
        msgReadyFlag = false; // clear flag set by ISR

        // check that mesg is response from MPPT
        if( (_inmsg.id & 0x7F0) == 0x770){
	    readSensor(); // send msg to Beaglebone
	}
    }
    else{ // request next set of readings
        SendRTR();
    }
}

/** readSensor - overrides virtual base class function
 *  data for CAN msgs in just arrived in message
 *  puts message on CAN output queue.
 */
int svtMPPTsensor::readSensor(){
    CAN_Message msg;    
    msg.id     = CANbase  + (_inmsg.id & 0x000F); // offset by MPPT number 1-15
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = SVTCANFORMAT;
    memcpy(msg.data, _inmsg.data, msg.len);
    if(sendOutMsg(msg) != 0){
	udebug.printf(" mem fail in sendOutMsg 1 \r\n"); 
    }
    memset(_inmsg.data, 0, 8); // clear input
    return 0;
}

void svtMPPTsensor::SendRTR(){
    SEEED_CANMessage rtr;
    unsigned char err;
    rtr.id = 0x710 + RTRtable[lastRTR];
    rtr.len = 0;
    rtr.type = CANRemote;
    rtr.format = SVTCANFORMAT;
    if(_can.write(rtr) == 0){
        udebug.printf("mppt rtr msg failed \r\n");
        if(_can.errors() != 0){
	    err = _can.errorFlags();
	    udebug.printf("mppt rtr flag %x \r\n",err);
	}
    }
    lastRTR = (lastRTR + 1)%lenRTR;
}

/** isr - handles all interrupts from MCP2515, sets/clears thread signals 
 *         note signature is (), not (void).
 */
void svtMPPTsensor::isr(){
    msgReadyFlag = true;
}

/** command - responds to Beaglebone commands.
 * @param m - CAN message from Beaglebone. First byte is command.
 */
int svtMPPTsensor::command(CAN_Message& m){
    switch(m.data[0]){
    case 0: 

	break;
    case 1:  

	break;
    case 2:  


    break;
    case 3:  

	break;
    case 4:
	break;
    default:

	break;
    }
    return 0;
}


