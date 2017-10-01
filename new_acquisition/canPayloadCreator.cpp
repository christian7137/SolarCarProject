#include "mbed.h"
#include "canPayloadCreator.hh"

bool bCanTimerFlag = false;
Ticker canDumpTimer;
DigitalOut led2(LED2);
canPayloadCreator * CPC;

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

/* CLASS BEGIN */
void canPayloadCreator::Init(void){
    // create the memory for the data buffer based on the number of sensors in the tmpList
    pDataBuffer = new int[ sensorIdList.size() ];
    pIdsToIdx = new int[ sensorIdList.size() ];
    dataBufferSize = sensorIdList.size();
    std::list<int>::iterator it;
    int storeIdx=0;
    for (it = sensorIdList.begin(); it != sensorIdList.end(); ++it){
        pDataBuffer[storeIdx] = 0;
        pIdsToIdx[storeIdx] = *it;  //store the id into the array of ids
        storeIdx++;
    }
}

int canPayloadCreator::getSensorIdxFromSensorId(int sensorId)
{
    int sensorIdx = dataBufferSize;
    for(int a=0; a< dataBufferSize; a++){
        if( pIdsToIdx[a] == sensorId)
        {
            return a;
        }   
    }
    return sensorIdx;    
}

void canPayloadCreator::updateSensorData( unsigned int sensorIdx, int * pData )
{
     if(sensorIdx < dataBufferSize )
    {
        pDataBuffer[sensorIdx] = *pData;
    }   
}