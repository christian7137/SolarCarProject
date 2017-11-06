/***************************************************************
   -
   - file:  svtSTM32CAN.cpp
   -
   - purpose: rtos interface to STM32 CAN bus
   -
   - author: J. C. Wiley, March 2017, from svtK64FCAN
   -
***************************************************************/
#include "svtSTM32CAN.h" 

/** Destructor
 *
 */
svtSTM32CAN::~svtSTM32CAN(){

}

/** init - set up function
 */
int svtSTM32CAN::init(int canspeed, CANFormat format, uint32_t addr, svtSensorMgr* sM){
    _stm32can.frequency(canspeed);
    _format = format;
    //_stm32can.open(canspeed, format, addr, _threadId); 
    svtCAN::init(canspeed, format, addr, sM);
    _unsentcnt = 0;
    msgSentCnt = 0;
    msgRecCnt  = 0;
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
int svtSTM32CAN::Filter(int32_t mask, int32_t* idfilters, int32_t n){
    return  0; //_stm32can.filter(mask, idfilters, n);
}

/**
 *  frequency -  set CAN bus frequency
 */
int svtSTM32CAN::frequency(int hz){
   return _stm32can.frequency(hz);
}

/** handleInMsg - retrieves msg  and puts in local IN queue
 *              
 */
int svtSTM32CAN::handleInMsg(){
    CANMessage canMsg;
    CANMessage *message;
    message = NULL;
    if(_stm32can.read(canMsg) == 0){
	//udebug.printf("handleInMsg:\r\n");        
	if(INqueue_mutex.trylock()){
	    message = INqueue.alloc();
	    INqueue_mutex.unlock();
	}
        else{
	    udebug.printf("handleInmsg: lock failed\r\n");
	}
	if(message != NULL){
	    message->len    = canMsg.len;
	    message->type   = canMsg.type;
	    message->format = canMsg.format;
	    message->id     = canMsg.id;
	    memcpy(message->data, canMsg.data, canMsg.len);
	    if(INqueue_mutex.trylock()){
		INqueue.put(message);
		INqueue_mutex.unlock();
	        osSignalSet(_threadId, SIG_CAN_INQUEUE_NOTEMPTY);
                msgRecCnt++;
	    }
            else{
		udebug.printf("handleInmsg: lock failed 2.\r\n");
	    }

	}
    }
    else{
	udebug.printf("_stm32can.read error \r\n");
    }   
    return 0;
}

/** handleOutMsg - retrieve CAN msg from OUTqueue and send STM32_CANMessag.
 * result codes from mail.get    
 *   osOK: no mail is available in the queue and no timeout was specified
 *   osEventTimeout: no mail has arrived during the given timeout period.
 *   osEventMail: mail received, value.p contains the pointer to mail content.
 *   osErrorParameter: a parameter is invalid or outside of a permitted range.
 */
int svtSTM32CAN::handleOutMsg(){
    int err;
    osEvent evt;
    if(_canmsg.len != 0){ // old unsent message, try again
        err = _stm32can.write(_canmsg); // err = 0-fail, 1-sent
        if(err == 1){ // msg sent
            _canmsg.len = 0;
            _unsentcnt = 0;
            msgSentCnt++;
        }
        else{
            _unsentcnt++;
	    udebug.printf("stm32can.write old err %x cnt %d\r\n", err, _unsentcnt);
	}
    }
    else{   // read next message from queue
	if( OUTqueue_mutex.trylock()){
	    evt = OUTqueue.get(0); // no waiting
	    OUTqueue_mutex.unlock();
	}
	else{
	    udebug.printf("handleOut lock failed.\r\n");
	}
	if(evt.status == osEventMail){
	    CANMessage *msg = (CANMessage*)evt.value.p;
	    if(msg != 0){
		copymsg(_canmsg, *msg);
		if( OUTqueue_mutex.trylock()){
		    OUTqueue.free(msg); // queue read, so free mem
		    OUTqueue_mutex.unlock();
		    if(OUTqueueCnt > 0 ){
			OUTqueueCnt--;
			// reset signal, because it can be cleared when accessed
			osSignalSet(_threadId, SIG_CAN_OUTQUEUE_NOTEMPTY);
		    }  
		}
                else{
		    udebug.printf("handleOut lock failed 2.\r\n");
		} 
	    }                
	}
	else if(evt.status == osOK) { // queue empty - clear flag
	    if(OUTqueueCnt == 0){
		osSignalClear(_threadId, SIG_CAN_OUTQUEUE_NOTEMPTY);
	    }
	}
    }
    if(_canmsg.len != 0 ){ // have msg, try to send
	err = _stm32can.write(_canmsg);
	if(err == 1){ // msg sent
	    _canmsg.len = 0;
            _errcount = 0;
            _unsentcnt = 0;
             msgSentCnt++;
	}
        else{
            _errcount++;
	    udebug.printf("stm32can.write err %x %d\r\n", err, _errcount);            
	}
    }
    if(OUTqueueCnt > 0){
        osSignalSet(_threadId, SIG_CAN_OUTQUEUE_NOTEMPTY);
    }
    return 0;
}

/** RxErrors - returns number of read errors
 *
 */
int svtSTM32CAN::RxErrors(){
    return  _stm32can.rderror();
}

/** TxErrors - returns number of transmit  errors
 *
 */
int svtSTM32CAN::TxErrors(){
    return (_stm32can.tderror());
}






