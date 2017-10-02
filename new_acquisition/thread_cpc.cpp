#include "mbed.h"
#include "canPayloadCreator.hh"

extern canPayloadCreator * CPC;
bool bCanTimerFlag = false;
Ticker canDumpTimer;
DigitalOut led2(LED2);

/*THREAD BEGIN*/
void canTimerCallback(void){
    bCanTimerFlag = true;    
}

void processTimerInterrupt_CAN(){
    led2 = !led2;    
    
    while(lock != AVAIL){}
    lock = LOCKED;
    //first let's just print the data in the buffer
    for(int a=0; a< dataBufferSize; a++){
        pc.printf("id:%d data:%d\r\n", a , pDataBuffer[a] );
    }
    
    lock = AVAIL;
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
