/**

   File: svtMPPTsensor.h

   Purpose - Uses CAN interface .
 
   Author - J. C. Wiley,  Feb. 2015

*/
#ifndef svtMPPTsensor_H
#define svtMPPTsensor_H
#include "svtSensor.h"


#include <stdint.h>

// F64K spi uses same pins as Arduino
#define Arduino_ncs  PTD0
#define Arduino_mosi PTD2
#define Arduino_miso PTD3
#define Arduino_sck  PTD1
#define Arduino_int PTB9

// Set low for testing, SPI can exceed logic analyzer no error speed

/** MPPTsensor class.
 * Uses SeeedCan interface   
 */
class svtMPPTsensor: public svtSensor {
public:
svtMPPTsensor(PinName ncsPin, PinName intPin, PinName mosiPin, PinName misoPin, PinName sckPin, int speed): 
    _can(ncsPin, intPin, mosiPin, misoPin, sckPin, speed),
	_interrupt(intPin) {
	    _interrupt.fall(this, &svtMPPTsensor::isr);  // new CAN message on falling edge
	    lastRTR = 0;
	    msgReadyFlag = false;
	    RTRtable[0] = 1;  // switch settings on MPPT [1...15]
	    RTRtable[1] = 2;
	    RTRtable[2] = 3;
	    lenRTR = 3; 
	};
    ~svtMPPTsensor();

    int init();
    virtual int readSensor();
    virtual int command(CAN_Message& m);
    virtual void update();
    /** isr - handles all interrupts from MPC2515
     */
    void isr();

private:
    void SendRTR();
    void reset();
    SEEED_CAN  _can;
    InterruptIn _interrupt;
    SEEED_CANMessage _outmsg;
    SEEED_CANMessage _inmsg;
    bool msgReadyFlag;
    int lastRTR;
    uint8_t RTRtable[3];
    int lenRTR;
};
#endif
