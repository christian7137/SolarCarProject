#include "mbed.h"
#include "canQueue.hh"
#include "can_structs.hh"

extern canQueue CQ;
DigitalOut led3(LED3);

/*THREAD BEGIN*/

void sendCanMessage(){
    led3 = !led3;    
    
    if(CQ != NULL){
		if(CQ.queueEmpty() == false){
			CAN_MSG nextMsg = CQ.getNextCanMsg();

			/* print the can message for debugging purposes */
			pc.printf("Count: %d \t", msg1.data.type3.sensor1Data);
			pc.printf("Payload: %#x\r\n",msg1); 
			/* TODO: keep sending this message out of the can bus until it was successful */
			
		}
    }
    
}

#define CAN_TIMER_SECS  1
#define SENSOR_ACQ_SECS 0.2
void thread_canPayloadCreator_main(void){
    while(1)
    {
		sendCanMessage();
    }
}
