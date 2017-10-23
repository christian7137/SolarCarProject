/*!
 * \file thread_canager.cpp
 * \author Beau Roland
 * \brief Thread software that manages the out going CAN bus messages
 *
 */
 
#include "mbed.h"
#include "canQueue.h"
#include "can_structs.h"
#include "CAN.h"

extern canQueue CQ;
extern Serial pc;
DigitalOut led3(LED3);
int CANaddress = 1337;
CAN MainCANbus(PD_0, PD_1);

/*THREAD BEGIN*/
/*!
 * \brief Thread function that checks whether we have any CAN messages to sendCanMessage
 * and sends it on the CAN Bus.
 */
void sendCanMessage(){
    led3 = !led3;    
	if(CQ.queueEmpty() == false){
		CAN_MSG nextMsg = CQ.getNextCanMsg();

		/* print the can message for debugging purposes */
		pc.printf("Out: 0x");
	    for(int a=0; a<8; a++){
	    	pc.printf("%2.2x", nextMsg.raw_msg[a]);
	    }
	    pc.printf("\r\n");
		
		/* TODO: keep sending this message out of the can bus until it was successful */
		//CANMessage canMessage(CANaddress, nextMsg.raw_msg, sizeof(nextMsg.raw_msg));
		CANMessage canMessage(CANaddress, nextMsg.raw_msg, 8);
		testCANbus.write(canMessage);
	}
    
}
void setup_can_bus()
{
    MainCANbus.frequency(100000);
}
/*!
 * \brief Thread function that defines the start of the Canager Thread.
 * \details In this function, it continually calls function that sends the next CAN Message
 * if there are any in the FIFO queue.
 */
void thread_canager_main(void){
	setup_can_bus();
    while(1)
    {
		sendCanMessage();
    }
}

