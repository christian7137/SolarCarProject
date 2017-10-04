#include "mbed.h"
#include "canQueue.hh"
#include "can_structs.hh"

extern canQueue CQ;
extern Serial pc;
DigitalOut led3(LED3);

/*THREAD BEGIN*/
void sendCanMessage(){
    led3 = !led3;    
	if(CQ.queueEmpty() == false){
		CAN_MSG nextMsg = CQ.getNextCanMsg();

		/* print the can message for debugging purposes */
		pc.printf("Canager: %#x\r\n",nextMsg); 
		/* TODO: keep sending this message out of the can bus until it was successful */
		
	}
    
}

void thread_canager_main(void){
    while(1)
    {
		sendCanMessage();
		//wait(0.1); // TODO: remove at some point to increase bandwidth
    }
}
