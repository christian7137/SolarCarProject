/***************************************************************
   -
   - file:  svtSensor.h
   -
   - purpose: base class for F64K sensors
   -
   - author: J. C. Wiley, Sept. 2014
   -
***************************************************************/
#ifndef _SVTSENSOR_H_
#define _SVTSENSOR_H_

#include "mbed.h"
#include "rtos.h"
#include "svtCAN.h"
#include "svtSysState.h"

#define PROCESSORMASK 0x0700
#define SENSORMASK   0x07F0


#include "svtSensorID.h"
//typedef enum _svtSensorType { GreenLedSensorID =  0x0C,
//			      FXOS8700CQID     =  0x0B,
//                              svtGPSsensorID   =  0x08,
//                              svtTPMSsensorID  =  0x02,
//                              svtMPPTsensorID  =  0x07,
//                              svtIMUsensorID   =  0x04,
//                              svtDASHsensorID  =  0x05,
//                              svtBPSsensorID   =  0x06,
//                              svtCNTRLsensorID =  0x0A,
//                              svtGAPsensorID   =  0x01,
//                              svtNGMsensorID   =  0x09
//}svtSensorType_t;


class svtSensor{
public:
    /** svtSensor - Constructor
     */
    svtSensor();

    /** svtSensor - Destructor
     */
    ~svtSensor();

    /** attachCAN - connect sensor to CAN bus
     * @param aCANBUS - pointer to svtCAN 
     */

    void attachCAN(svtCAN* aCANBUS){
	svtcanpt = aCANBUS;
    }

    /** setCANBASE - CAN address for processor and sensor (upper 7 bits), message bits free (low order 4)
     *
     */
    void setCANBASE(int b){
        CANbase = b & SENSORMASK;
    }

    /** init - initialize sensor if necessary
     */
    virtual int init();

    /** readSensor - read particular sensor
     *  requests sensor to send its report via CAN message(s)
     *  @return error code
     */
    virtual int readSensor() = 0;

    /** command - receive command via CAN message
     *     a command may tell sensor to change internal settings
     *     or initiate an activity
     *    @param m - CAN message containing command
     *    @return error code
     */
    virtual int command(CANMessage& m) = 0;

    /** prod - ISR function, which generates signal for update.
     */
    void prod();

    /** getPeriod - 
	@return updatePeriod -  in ms
    */
    int getPeriod(){return updatePeriod;}
   
    /** setPeriod - sets update period (ms) 
	@param  p - update period in ms
    */
    void setPeriod(int32_t p){
	updatePeriod = p;  // ms
        float s_period = updatePeriod/1000.0;
        myTicker = new Ticker();
        if(myTicker != NULL){
	    myTicker->attach(callback(this, &svtSensor::prod), s_period);
	}
    }

    /** setSignal - sets signal for update call back, used only by svtSensorMgr.
     * @param n - integer less than 32
     */
    void setSignal(int n){
        mySignal = (1 << n);
    }

    /** setThreadId - ID of manager thread - Used only by svtSensorMgr.
     */
    void  setThreadId(osThreadId tid){
	_threadId = tid;
    }

/** \name	attachSysState
 *
 */
    void attachSysState(svtSysState* s){
	_ss = s;
    }

/** setClock
 *
 */
    void attachClock(uint32_t* clock){
	msclockpt = clock;
    }

    /** update - update is called on specified period
     */
    virtual void update();
    svtSysState* _ss;

/** getCANbase
 *
 */
    int getCANbase(){
	return CANbase;
    }

protected:
    /** sendOutMsg  - adds CAN message to outgoing message queue;
	@param - a CANMessage 
    */
    int sendOutMsg(CANMessage& m);
    int CANbase;  					///< processor (3 bits)[8]  + sensor part( 4 bits)[16] + message ID (4 bits)[16]  
    uint32_t* msclockpt; 			///< pointer to clock value
private:
    int updatePeriod; 				///< ms - update sensor at this rate, may be same as report rate or sample rate.
    svtCAN* svtcanpt;
    Ticker* myTicker;
    uint32_t mySignal;
    osThreadId _threadId;  			///< manager thread
};   

#endif
