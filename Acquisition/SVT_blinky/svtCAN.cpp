/***************************************************************
   -
   - file:  svtCAN.cpp
   -
   - purpose: handles connection to solar car main CAN bus
   -
   - author: J. C. Wiley, Sept. 2014
   -
***************************************************************/
#include "svtCAN.h" 

extern int CANaddr;
extern DigitalOut _ledB;

Thread svtCAN:: tcan;

/**
 * destructor
 */
svtCAN::~svtCAN(){
}

/** init - set up function,  initializes can bus
 */
int svtCAN::init(int canspeed, CANFormat format, uint32_t addr, svtSensorMgr* smgr){  
    msgMask = 0x07FF;              
    udebug.printf("init: begin CAN thread %d\r\n", CANaddr);
    
    // final statement  to start thread.
    tcan.signal_set(SIG_CAN_START);
    _format = format;
    sensorMgrP = smgr;
    return 0;
}

/** Filter - sets upto six input id filters, which when combined with
 *   mask, determine which messages are read from bus.
 *  @param mask  32 bit mask, where a 1 indicates bit will be checked between
 *     incoming-id and filter-id, 0 is pass all. The underlying hardware may support
 *     more complex masking, but here only single global mask used.
 *   @param idfilters - pointer to an array of filters
 *   @param n - number of filters supplied
 *   @return 0 - success, 1 failure
 */
int svtCAN::Filter(int32_t mask, int32_t* idfilters, int32_t n){
    return  0;
}

/** canloop - CAN worker thread
 * returns from osSignalsWait:   
 *   osOK: no signal received when the timeout value millisec was 0.
 *   osEventTimeout: signal not occurred within timeout
 *   osEventSignal: signal occurred, value.signals contains the signal flags; these signal flags are cleared.
 *   osErrorValue: the value signals is outside of the permitted range.
 *   osErrorISR: osSignalWait cannot be called from interrupt service routines.
 */
void svtCAN::canloop(){
    osEvent event;
    _threadId = tcan.gettid();
    tcan.signal_wait(SIG_CAN_START);
    while(1){
	// wait for any signal or time out.
	event = tcan.signal_wait(0, CAN_TIMEOUT);      
        if(event.status == osOK){ // no signal
	    udebug.printf("svtCAN::canloop: no signal \r\n");
	}
        else if(event.status == osEventTimeout){ // hit time out limit
	   udebug.printf("svtCAN::canloop: CAN timeout %x\r\n", CANaddr);
	   statusTest();
	}
        else if(event.status == osEventSignal){
	    // udebug.printf("sg %x %x\r\n", event.value.signals, CANaddr);
	    // handle particular signals
	   
	    if(event.value.signals & SIG_CAN_MSG_READY){
		osSignalClear(_threadId, SIG_CAN_MSG_READY);
		//udebug.printf("\r\nsvtCAN:msg rdy\r\n");
		handleInMsg();
	    }
	    else if(event.value.signals & SIG_CAN_INQUEUE_NOTEMPTY){ 
		//udebug.printf("svtCAN:Qin !MT\r\n");   
		processInMsg();
	    }
	    else if( (event.value.signals & SIG_CAN_OUTQUEUE_NOTEMPTY)  ){
		//udebug.printf("Qout %d \r\n", OUTqueueCnt);
		handleOutMsg();
	    }
	    else if(event.value.signals &SIG_CAN_TX_WARNING){ 
		udebug.printf("tx warning\r\n");
	    }     
	    else if(event.value.signals & SIG_CAN_RX_WARNING){ 
		udebug.printf("rx warning\r\n");
	    }     
	    else if(event.value.signals & SIG_CAN_BUSSOFF){  
		udebug.printf("bus off\r\n");
	    }       
	    else if(event.value.signals & SIG_CAN_BIT1ERR){  
		udebug.printf("bit 1 error\r\n");
	    }        
	    else if(event.value.signals & SIG_CAN_BIT0ERR){ 
		udebug.printf("bit 0 error\r\n");
	    }          
	    else if(event.value.signals & SIG_CAN_ACKERR){ 
		udebug.printf("ack error\r\n");
	    }          
	    else if(event.value.signals & SIG_CAN_CRCERR){ 
		udebug.printf("crc error\r\n");
	    }           
	    else if(event.value.signals & SIG_CAN_FRMERR){ 
		udebug.printf("frame error\r\n");
	    }           
	    else if(event.value.signals & SIG_CAN_STFERR){ 
		udebug.printf("stuff error\r\n");
	    }           
	    else if(event.value.signals & SIG_CAN_WAKEUP){ 
		udebug.printf("wakeup\r\n");
	    }           
	    else{   // signal not found
               udebug.printf("no signal match %4x, thread %d \r\n", event.value.signals, _threadId);
	    } 
	}
    }  
}

/** statusTest
 *   No messages in timeout limit, check status, recover if necessary and possible.
 */
int svtCAN::statusTest(){
    udebug.printf("svtCAN::statusTest nomsg timeout\r\n");
    return 0;
} 

/** processInMsg() - extracts message from INqueue and processes depending on CAN ID
 *                 
 */
int svtCAN::processInMsg(){
    CANMessage msg;
    osEvent evt;
    msg.id = 0;

    if(INqueue_mutex.trylock()){
	evt = INqueue.get(0);
	INqueue_mutex.unlock();
    }
    else{
	udebug.printf("procInMsg: lock failed. \r\n");
    }
    if(evt.status == osEventMail) {
	CANMessage *message = (CANMessage*)evt.value.p;
	copymsg(msg, *message);
	if(INqueue_mutex.trylock()){
	    INqueue.free(message);
	    INqueue_mutex.unlock();
	}
	else{
	    udebug.printf("procInMsg: lock failed 2. \r\n");
	}
    }
    else{
	osSignalClear(_threadId, SIG_CAN_INQUEUE_NOTEMPTY);
    }
    // udebug.printf("InMsg %x: %d %d\r\n", msg.id, mgs.data[0], msg.data[1])  
    if(sensorMgrP != 0 && msg.id != 0) sensorMgrP->command(msg);  // give sensors look at message 
    return 0;
}

/** sendOutMsg - put CAN message in output queue - used by svtSensors
 *   alloc return:
 *      pointer to memory block that can be filled with mail 
 *      or NULL in case of error. 
 *   return codes on put:    
 *      osOK: the message is put into the queue.
 *      osErrorValue: mail was previously not allocated as memory slot.
 *      osErrorParameter: a parameter is invalid or outside of a permitted range.
 */
int svtCAN::sendOutMsg(CANMessage& m){
    int ret = 0;
    int err;
    CANMessage* msg;

    // udebug.printf("in svtCAN::senOutMsg\r\n");
    if(OUTqueue_mutex.trylock()){  // critical section?
	msg = OUTqueue.alloc();
	OUTqueue_mutex.unlock();
    }
    else{
	udebug.printf("svtCAN::sendOutMsg: lock failed\r\n");
    }
    
    if(msg != NULL){
	copymsg( *msg, m); // m -> msg
	if(OUTqueue_mutex.trylock()){
            
	    err = OUTqueue.put(msg);
	    OUTqueue_mutex.unlock();
	    if(err == osOK){
		OUTqueueCnt++;
		osSignalSet(_threadId, SIG_CAN_OUTQUEUE_NOTEMPTY);
	    }
	    else{
		udebug.printf("svtCAN::sendOutMsg: queue.put failed \r\n");
		ret = 1;
	    }
	}
        else{
	    udebug.printf("svtCAN::sendOutMsg: lock failed 2\r\n");
	}
    }
    else{
	ret = 1; // memory allocation failed
	udebug.printf("svtCAN::sendOutMsg: memory allocation failed\r\n");
    }   
    return ret;
}

/** RxErrors - returns number of read errors - should be overridden 
 *
 */
int svtCAN::RxErrors(){
    return  0xF;
}

/** TxErrors - returns number of transmit  errors
 *
 */
int svtCAN::TxErrors(){
    return 0xF;
}

/** copy for CANMessage  -- utility
 *
 */
void svtCAN::copymsg(CANMessage& toMsg, CANMessage& fromMsg){
	   toMsg.id = fromMsg.id;
	   toMsg.len = fromMsg.len;
	   toMsg.format = fromMsg.format;
	   toMsg.type   = fromMsg.type;
	   memcpy(toMsg.data, fromMsg.data, fromMsg.len);
}



