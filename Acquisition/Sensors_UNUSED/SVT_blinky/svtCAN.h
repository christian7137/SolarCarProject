/***************************************************************
   -
   - file:  svtCAN.h
   -
   - purpose: handles connection to solar car main CAN bus
   -
   - author: J. C. Wiley, Sept. 2014
   -         uses SEEED_CAN by Sophie Dexter, mbed.org (with modifications)
   -         for thread within class see: https://mbed.org/forum/mbed/topic/4388/
   -
***************************************************************/
#ifndef _SVTCAN_H_
#define _SVTCAN_H_
#include "mbed.h"
#include "rtos.h"

#include "svtSignals.h"
#include "svtSensorMgr.h"

#define SVTCANFORMAT CANStandard
//#define SVTCANFORMAT CANSExtended

#define CAN_TIMEOUT   2000       // ms - test that we are still communicating
// note that the timeout needs to be long compared to the typical interval between
// messages. Especially true if only GPS at 1 Hz is running.

extern RawSerial udebug;  // crutch for testing

/** svtCAN class
 *  
 */
class svtCAN{ 
public:
/** svtCAN - constructor
 */
svtCAN() {
	    OUTqueueCnt = 0;
            sensorMgrP = 0;
	    tcan.start(callback(this,&svtCAN::canloop));
         };

/** svtCAN - destructor
 */
    virtual ~svtCAN();

/** init - initialize bus, sets speed and IDformat
 */
    virtual int init(int canspeed, CANFormat format, uint32_t canaddr, svtSensorMgr* smgr);

/** Set the frequency of the CAN interface
 *
 *  @param hz The bus frequency in hertz
 *
 *  @returns
 *    1 if successful,
 *    0 otherwise
 */
    virtual int frequency(int hz) = 0;

/** Filter - sets upto six input id filters, which when combined with
 *   mask, determine which messages are read from bus.
 *  @param mask  32 bit mask, where a 1 indicates bit will be checked between
 *     incoming-id and filter-id, 0 is pass all. The underlying hardware may support
 *     more complex masking, but here only single global mask used.
 *   @param idfilters - pointer to an array of filters
 *   @param n - number of filters supplied
 *   @return 0 - success, 1 failure
 */
    virtual int Filter(int32_t mask, int32_t* idfilters, int32_t n);

 
/** canloop - thread execution
 */
    void canloop();
        
/** handleInMsg - retrieve msg from MPC2515 and put in queue
 */
    virtual int handleInMsg() = 0;

/** handleOutMsg - retrieve msg from OUTqueue and send CAN message.
 */
    virtual int handleOutMsg() = 0;

/** processInMsg - messages in input pool, deal with one message.
 */
    int processInMsg();

/** statusTest - hit message timeout, test to see if need to restart
 */
    int statusTest();

/** sendOutMsg - sensors can put CAN message in output queue.
 */
    int sendOutMsg(CANMessage& m);

/** RxErrors - returns number of read errors
 *
 */
virtual int RxErrors();

/** TxErrors - returns number of transmit  errors
 *
 */
virtual int TxErrors();

/** copy for CANMessage
 *
 */
void copymsg(CANMessage& toMsg, CANMessage& fromMsg);

/** getCANout
 *
 */
int getCANout(){ return msgSentCnt;}

/** getCANin
*
*/
int getCANin(){return msgRecCnt;}
      
protected:          
    static Thread  tcan; // formally _thread
    osThreadId    _threadId;
    Mail<CANMessage, 16>  INqueue;
    Mail<CANMessage, 16>  OUTqueue;  
    Mutex          OUTqueue_mutex;
    Mutex          INqueue_mutex;  
    CANFormat      _format; 
    uint32_t       msgMask;
    int            OUTqueueCnt;
    svtSensorMgr*  sensorMgrP;
    int            msgSentCnt;  // counts messages sent
    int            msgRecCnt;   // counts messages received
};
#endif

