/************************************
  -
  - file: SPIsensor.h 
  -
  - purpose: instance of svtSensor for a blinking green LED.
  -
  - author: J. C. Wiley, Sept. 2014
  -      Derived from mbed.org example blinky
.
  -
*************************************/
#ifndef SPISENSOR_H_
#define SPISENSOR_H


#include "svtSensor.h"
class SPIsensor: public svtSensor {
public:
    /** SPIsensor - Constructor 
     */
    SPIsensor():cs(PD_14), counter(0){
    }

    /** ~SPIsensor - Destructor
     */
    ~SPIsensor();

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
    DigitalOut cs;
    uint32_t   counter;
};
#endif
