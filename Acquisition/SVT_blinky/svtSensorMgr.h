/***************************************************************
   -
   - file:  svtSensorMgr.h
   -
   - purpose: handles collection of svtSensors
   -
   - author: J. C. Wiley, Sept. 2014
   -
***************************************************************/
#ifndef _SVTSENSORMGR_H_
#define _SVTSENSORMGR_H_

#include "mbed.h"
#include "rtos.h"
#include <vector>

#define SIG_START 1
#define SENSOR_TIMEOUT 1000 // ms
#define SENSOR_SIGNAL 2   // dummy signal

#define SIG_SENSOR_00 0x0001
#define SIG_SENSOR_01 0x0002
#define SIG_SENSOR_02 0x0004
#define SIG_SENSOR_03 0x0008
#define SIG_SENSOR_04 0x0010
#define SIG_SENSOR_05 0x0020
#define SIG_SENSOR_06 0x0040
#define SIG_SENSOR_07 0x0080
#define SIG_SENSOR_08 0x0100
#define SIG_SENSOR_09 0x0200
#define SIG_SENSOR_10 0x0400
#define SIG_SENSOR_11 0x0800
#define SIG_SENSOR_12 0x1000
#define SIG_SENSOR_13 0x2000

class svtSensor;

class svtSensorMgr{
    public:
         /** svtSensorMgr - Constructor
         */
         svtSensorMgr(){
	     // not sure of order with change
           tsensmgr.signal_set(SIG_START);
	   tsensmgr.start(callback(this, &svtSensorMgr::loop));
         };

         /** ~svtSensorMgr - Destructor
         */
        ~svtSensorMgr();

         /** addSensor - accepts svtSensors
          *     @param s - pointer to svtSensor
         */
         void addSensor(svtSensor* s);
 
          /** threadStarter - thread starter function to insure thread is started 
           * after class fully intialized. Must be static.
          */
          static void threadStarter(void const *p);

          /** loop - thread execution
          */
          void loop();

          /** command - give sensors chance to process incomming messages.
	   */
          void command(CANMessage& m);
	  
          /** listSensor canbase
	   */
	  void listSensors();

    private:
	 static Thread  tsensmgr; // was _thread
         osThreadId    _threadId;
         vector<svtSensor*> sensors;

};

#endif
