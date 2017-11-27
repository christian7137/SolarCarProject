/***************************************************************
   -
   - file:  svtSensorMgr.cpp
   -
   - purpose: handles collection of svtSensors
   -
   - author: J. C. Wiley, Sept. 2014
   -
***************************************************************/
#include "svtSensorMgr.h"
#include "svtSensor.h"

extern RawSerial udebug;
extern int CANaddr;
Thread  svtSensorMgr::tsensmgr;

/** ~svtSensorMgr - Destructor
 */
svtSensorMgr::~svtSensorMgr(){
}

/** addSensor - accepts svtSensors
 *     @param s - pointer to svtSensor
 *     generates a signal for each sensor
 */
void svtSensorMgr::addSensor(svtSensor* s){
    int n = sensors.size();
    if( n < 32){             // maximum of 32 sensors/signals
	sensors.push_back(s);
	s->setSignal(n);
	s->setThreadId(_threadId); // for Signal call back
    }
}

/** listSensors
 *      lists sensors attached to this processor
 */
void svtSensorMgr::listSensors(){
    int n = sensors.size();
    for(int i=0; i<n; i++){
	udebug.printf("sensor canbase %x\r\n", sensors[i]->getCANbase());
    }
}

/** loop - thread execution
 */
void svtSensorMgr::loop(){
    osEvent event;

    //tsensmgr.signal_wait(SIG_START);
    _threadId = tsensmgr.gettid();

    while(true){
	// wait for any signal or time out.
	Thread::wait(400);
	event = tsensmgr.signal_wait(0, SENSOR_TIMEOUT);
	if(event.status == osEventTimeout){

	}
	else if(event.status == osEventSignal){  
            int32_t n = 0;
            while( !(event.value.signals & (1<<n)) && n < 32) n++;
            if( (1<<n) & event.value.signals){
				sensors[n]->update();
                osSignalClear(_threadId, event.value.signals);
	    }
            else{  // no signal match
		udebug.printf("no signal match %4x, thread %d, threadcount %d\r\n", 
                                         event.value.signals, _threadId, sensors.size());
	    }
	}
    }
}

/** command - give sensors chance to process incomming messages.
 *            Each sensor sees all incomming messages.
 */
void svtSensorMgr::command(CANMessage& m){
    vector<svtSensor*>::iterator s;
    for(s = sensors.begin(); s != sensors.end(); s++){
	(*s)->command(m); 
    }
}
