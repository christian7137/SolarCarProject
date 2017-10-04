#include "mbed.h"
#include "canPayloadCreator.hh"
#include "canQueue.hh"
#include "can_structs.hh"

extern canPayloadCreator CPC;
extern canQueue CQ;
extern Serial pc;
extern bool bLock;
bool bCanTimerFlag = false;
Ticker canDumpTimer;
DigitalOut led2(LED2);

/*THREAD BEGIN*/
void canTimerCallback(void){
    bCanTimerFlag = true;    
}

void processTimerInterrupt_CAN(){
    led2 = !led2;    
    
    std::list<CAN_MSG> canMsgs;
    canMsgs = CPC.createCanMessages();
    
    std::list<CAN_MSG>::iterator it;
    for ( it = canMsgs.begin(); it != canMsgs.end(); ++it){
        pc.printf("CPC: %#x\r\n",*it); 
        CQ.push(*it);
    }
}

#define CAN_TIMER_SECS  1
void thread_canPayloadCreator_main(void){
    canDumpTimer.attach(&canTimerCallback, CAN_TIMER_SECS);
    while(1)
    {
        if(bCanTimerFlag == true){
            processTimerInterrupt_CAN();
            bCanTimerFlag = false;    
        }
        sleep();
    }
}
