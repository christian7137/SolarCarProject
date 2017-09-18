/***************************************************************
   -
   - file:  svtUSB.cpp
   -
   - purpose: handles connection to Beaglebone through USB.
   -      Construction parallel to svtCAN.
   -
   - author: J. C. Wiley, Sept. 2014 CAN
   -             convert to USB  Nov 2015.
   -
***************************************************************/
#include "svtUSB.h" 

extern int USBaddr;
extern DigitalOut _ledB;

/**
 * destructor
 */
svtUSB::~svtUSB(){
}

/** init - set up function,  initializes usb bus
 */
int svtUSB::init(uint16_t vendor_id, uint16_t product_id,  svtSensorMgr* smgr){               
    udebug.printf("init: begin USB thread \r\n");
    
    hid = new USBK64F(sizeof(USB_Message), sizeof(USB_Message),  vendor_id, product_id);
    
    // final statement  to start thread.
    _thread.signal_set(SIG_USB_START);
    sensorMgrP = smgr;
    return 0;
}

/** threadStarter - starts thread running at end of svtUSB construction
 */
void svtUSB::threadStarter(void const *p) {
    svtUSB *instance = (svtUSB*)p;

    instance->usbloop();
}

/** usbloop - USB worker thread
 * returns from osSignalsWait:   
 *   osOK: no signal received when the timeout value millisec was 0.
 *   osEventTimeout: signal not occurred within timeout
 *   osEventSignal: signal occurred, value.signals contains the signal flags; these signal flags are cleared.
 *   osErrorValue: the value signals is outside of the permitted range.
 *   osErrorISR: osSignalWait cannot be called from interrupt service routines.
 */
void svtUSB::usbloop(){
    osEvent event;
    _threadId = _thread.gettid();
    _thread.signal_wait(SIG_USB_START);
    while(1){
	// wait for any signal or time out.
	event = _thread.signal_wait(0, USB_TIMEOUT);      
        if(event.status == osOK){ // no signal
	    udebug.printf("svtUSB::usbloop: no signal \r\n");
	}
        else if(event.status == osEventTimeout){ // hit time out limit
	   udebug.printf("svtUSB::usbloop: USB timeout %x\r\n", USBaddr);
	   statusTest();
	}
        else if(event.status == osEventSignal){
	    //udebug.printf("sg %x %x\r\n", event.value.signals, USBaddr);
	    // handle particular signals
	   
	    if(event.value.signals & SIG_USB_MSG_READY){
		osSignalClear(_threadId, SIG_USB_MSG_READY);
		//udebug.printf("\r\nsvtUSB:msg rdy\r\n");
		handleInMsg();
	    }
	    else if(event.value.signals & SIG_USB_INQUEUE_NOTEMPTY){ 
		//udebug.printf("svtUSB:Qin !MT\r\n");   
		processInMsg();
	    }
	    else if( (event.value.signals & SIG_USB_OUTQUEUE_NOTEMPTY)  ){
		//udebug.printf("Qout %d \r\n", OUTqueueCnt);
		handleOutMsg();
	    }
	    else if(event.value.signals &SIG_USB_TX_WARNING){ 
		udebug.printf("tx warning\r\n");
	    }     
	    else if(event.value.signals & SIG_USB_RX_WARNING){ 
		udebug.printf("rx warning\r\n");
	    }     
	    else if(event.value.signals & SIG_USB_BUSSOFF){  
		udebug.printf("bus off\r\n");
	    }       
	    else if(event.value.signals & SIG_USB_BIT1ERR){  
		udebug.printf("bit 1 error\r\n");
	    }        
	    else if(event.value.signals & SIG_USB_BIT0ERR){ 
		udebug.printf("bit 0 error\r\n");
	    }          
	    else if(event.value.signals & SIG_USB_ACKERR){ 
		udebug.printf("ack error\r\n");
	    }          
	    else if(event.value.signals & SIG_USB_CRCERR){ 
		udebug.printf("crc error\r\n");
	    }           
	    else if(event.value.signals & SIG_USB_FRMERR){ 
		udebug.printf("frame error\r\n");
	    }           
	    else if(event.value.signals & SIG_USB_STFERR){ 
		udebug.printf("stuff error\r\n");
	    }           
	    else if(event.value.signals & SIG_USB_WAKEUP){ 
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
int svtUSB::statusTest(){
    udebug.printf("svtUSB::statusTest nomsg timeout\r\n");
    return 0;
} 

/** processInMsg() - extracts message from INqueue and processes depending on USB ID
 *                 
 */
int svtUSB::processInMsg(){
    USB_Message msg;
    osEvent evt;
    msg.mesgID  = 0;

    if(USBInQueue_mutex.trylock()){
	evt = INqueue.get(0);
	USBInQueue_mutex.unlock();
    }
    else{
	udebug.printf("USBprocInMsg: lock failed. \r\n");
    }
    if(evt.status == osEventMail) {
	USB_Message *message = (USB_Message*)evt.value.p;
	copymsg(msg, *message);
	if(USBInQueue_mutex.trylock()){
	    INqueue.free(message);
	    USBInQueue_mutex.unlock();
	}
	else{
	    udebug.printf("USBprocInMsg: lock failed 2. \r\n");
	}
    }
    else{
	osSignalClear(_threadId, SIG_USB_INQUEUE_NOTEMPTY);
    }
    // udebug.printf("InMsg %x: %d %d\r\n", msg.id, mgs.data[0], msg.data[1])  
    //   if(sensorMgrP != 0 && msg.id != 0) sensorMgrP->USBcommand(msg);  // give sensors look at message 
    return 0;
}

/** sendOutMsg - put USB message in output queue - used by svtSensors
 *   alloc return:
 *      pointer to memory block that can be filled with mail 
 *      or NULL in case of error. 
 *   return codes on put:    
 *      osOK: the message is put into the queue.
 *      osErrorValue: mail was previously not allocated as memory slot.
 *      osErrorParameter: a parameter is invalid or outside of a permitted range.
 */
int svtUSB::sendOutMsg(USB_Message& m){
    int ret = 0;
    int err;
    USB_Message* msg;
    
    if(UsbOutQueue_mutex.trylock()){  // critical section?
	msg = OUTqueue.alloc();
	UsbOutQueue_mutex.unlock();
    }
    else{
	udebug.printf("svtUSB::sendOutMsg: lock failed\r\n");
    }
    
    if(msg != NULL){
	copymsg(*msg, m);
	if(UsbOutQueue_mutex.trylock()){
            
	    err = OUTqueue.put(msg);
	    UsbOutQueue_mutex.unlock();
	    if(err == osOK){
		OUTqueueCnt++;
		osSignalSet(_threadId, SIG_USB_OUTQUEUE_NOTEMPTY);
	    }
	    else{
		udebug.printf("svtUSB::sendOutMsg: queue.put failed \r\n");
		ret = 1;
	    }
	}
        else{
	    udebug.printf("svtUSB::sendOutMsg: lock failed 2\r\n");
	}
    }
    else{
	ret = 1; // memory allocation failed
	udebug.printf("svtUSB::sendOutMsg: memory allocation failed\r\n");
    }   
    return ret;
}

/** copy for USB_Message  -- utility
 *
 */
void svtUSB::copymsg(USB_Message& toMsg, USB_Message& fromMsg){
    toMsg.sensorID = fromMsg.sensorID;
    toMsg.mesgID = fromMsg.mesgID;
    memcpy(toMsg.data, fromMsg.data, sizeof(toMsg.data));
}

/** handleOutMsg - retrieve USB msg from OUTqueue and send USB_Messag.
 * result codes from mail.get    
 *   osOK: no mail is available in the queue and no timeout was specified
 *   osEventTimeout: no mail has arrived during the given timeout period.
 *   osEventMail: mail received, value.p contains the pointer to mail content.
 *   osErrorParameter: a parameter is invalid or outside of a permitted range.
 */
int svtUSB::handleOutMsg(){
    int err;
    osEvent evt;
    if(_usbmsg.len != 0){ // old unsent message, try again
        err = _k64fcan.write(_usbmsg); // err = 0-sent, 1-not sent
        if(err == 0){ // msg sent
            _usbmsg.len = 0;
            _unsentcnt = 0;
            msgSentCnt++;
        }
        else{
            _unsentcnt++;
	    udebug.printf("k64fcan.write old err %x cnt %d\r\n", err, _unsentcnt);
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
	    USB_Message *msg = (USB_Message*)evt.value.p;
	    if(msg != 0){
		copymsg(_usbmsg, *msg);
		if( OUTqueue_mutex.trylock()){
		    OUTqueue.free(msg); // queue read, so free mem
		    OUTqueue_mutex.unlock();
		    if(OUTqueueCnt > 0 ){
			OUTqueueCnt--;
			// reset signal, because it can be cleared when accessed
			osSignalSet(_threadId, SIG_USB_OUTQUEUE_NOTEMPTY);
		    }  
		}
                else{
		    udebug.printf("handleOut lock failed 2.\r\n");
		} 
	    }                
	}
	else if(evt.status == osOK) { // queue empty - clear flag
	    if(OUTqueueCnt == 0){
		osSignalClear(_threadId, SIG_USB_OUTQUEUE_NOTEMPTY);
	    }
	}
    }
    if(_usbmsg.len != 0 ){ // have msg, try to send
	err = _k64fcan.write(_usbmsg);
	if(err == 0){ // msg sent
	    _usbmsg.len = 0;
            _errcount = 0;
            _unsentcnt = 0;
             msgSentCnt++;
	}
        else{
            _errcount++;
	    udebug.printf("svtUSB.write err %x %d\r\n", err, _errcount);            
	}
    }
    if(OUTqueueCnt > 0){
        osSignalSet(_threadId, SIG_USB_OUTQUEUE_NOTEMPTY);
    }
    return 0;
}

