#include "mbed.h"
#include "canPayloadCreator.hh"
#include "can_structs.hh"

extern canPayloadCreator * CPC;
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
    
    if(CPC != NULL){
        CPC->createCanMessage();
    }
    
}

#define CAN_TIMER_SECS  1
#define SENSOR_ACQ_SECS 0.2
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
