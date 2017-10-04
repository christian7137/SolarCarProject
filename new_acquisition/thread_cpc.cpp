/*!
 * \file thread_cpc.cpp
 * \author Beau Roland
 * \brief Thread software that controls the creation of CAN Messages and update access to new sensor data
 */
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

/*!
 * \brief CPC Thread's Timer Callback Function that set's a flag indicating that it is time to create CAN Messages based on the state all of the sensor data
 */
void canTimerCallback(void){
    bCanTimerFlag = true;    
}

/*!
 * \brief CPC Thread's private timer function that uses the CPC (canPayloadCreator class) to create the CAN messages and adds them to the CAN Message Output FIFO
 */
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
/*!
 * \brief CPC Thread start function entrance
 * \details On a timely basis, creates CAN Messages, adds them to the CAN Message FIFO, and sleeps till the next timer interrupt.
 */
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
