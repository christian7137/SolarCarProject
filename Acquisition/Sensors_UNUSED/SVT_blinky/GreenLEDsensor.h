/************************************
  -
  - file: GreenLEDsensor.h 
  -
  - purpose: instance of svtSensor for a blinking green LED.
  -
  - author: J. C. Wiley, Sept. 2014
  -      Derived from mbed.org example blinky
.
  -
*************************************/
#ifndef GREENLEDSENSOR_H_
#define GREENLEDSENSOR_H


#include "svtSensor.h"
#define LED_GREEN LED1
class GreenLEDsensor: public svtSensor {
public:
    /** GreenLEDsensor - Constructor 
     */
    GreenLEDsensor():_ledG(LED_GREEN), counter(0){
    }

    /** ~GreenLEDsensor - Destructor
     */
    ~GreenLEDsensor();

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
    DigitalOut _ledG;
    uint32_t   counter;
};
#endif
