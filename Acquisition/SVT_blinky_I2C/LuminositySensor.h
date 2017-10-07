/************************************
  -
  - file: Luminositysensor.h 
  -
  - purpose: instance of svtSensor for a blinking green LED.
  -
  - author: J. C. Wiley, Sept. 2014
  -      Derived from mbed.org example blinky
.
  -
*************************************/
#ifndef LUMINOSITYSENSOR_H_
#define LUMINOSITYSENSOR_H


#include "svtSensor.h"
#include "TSL2561.h"
class Luminositysensor: public svtSensor {
public:
    /** Luminositysensor - Constructor 
     */
    Luminositysensor(){
    }

    /** ~Luminositysensor - Destructor
     */
    ~Luminositysensor();

    /** init - initializes after construction (overrides)
     */
    virtual int init();  

    /** readSensor - overrides virtual base class function
     */
    virtual int readSensor(); 

    /** command - receive command via CAN message
     *     a command may tell sensor to change internal settings
     *     or initiate an activity
     */
    virtual int command(CANMessage& m);

    /** update - update is called at the requested interval
     */
    virtual void update();

private:
    uint16_t data_read;
};
#endif
