/***************************************************************
   -
   - file:  svtSensor.cpp
   -
   - purpose: base class for F64K sensors
   -
   - author: J. C. Wiley, Sept. 2014
   -
***************************************************************/
#include "svtSensor.h"
/** svtSensor - Constructor
 */

svtSensor::svtSensor(){
   svtcanpt = 0;
   _ss = 0;
   msclockpt = 0; 
}

/** svtSensor - Destructor
 */
svtSensor::~svtSensor(){

}

/** init - initialize sensor if necessary
 */
int svtSensor::init(){   
    return 0;
}

/** sendOutMsg - adds CAN message to outgoing message queue
 */
int svtSensor::sendOutMsg(CANMessage& m){
    if(svtcanpt != 0){
	svtcanpt->sendOutMsg(m);
    }
    return 0;
}

/** prod - ISR for Ticker to set thread signal
 */
void svtSensor::prod(){
    osSignalSet(_threadId, mySignal); 
}

/** update - update is called at the requested interval
 */
void svtSensor::update(){

}


