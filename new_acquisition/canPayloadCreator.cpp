#include "mbed.h"
#include "canPayloadCreator.hh"
#include "can_structs.hh"

canPayloadCreator CPC;
bool bLock = false;
extern Serial pc;

/* CLASS BEGIN */

canPayloadCreator::canPayloadCreator()
{
}
canPayloadCreator::~canPayloadCreator()
{
}
void canPayloadCreator::Init(void){
    // add any additional initialization
}

bool canPayloadCreator::addSensor( unsigned int sensorId, unsigned int numBytes)
{
    sensorDataListElem elem;
    elem.id = sensorId;
    elem.pData = new char[numBytes];
    elem.size = numBytes;
    
    if(elem.pData != NULL){
        for(int a=0; a<numBytes; a++)
            elem.pData[a] = 0;
        sensorList.push_back(elem);
        return true;
    }
    pc.printf("failed to create memory for sensor\r\n");
    return false;
    
}

void canPayloadCreator::updateSensorData( unsigned int sensorIdx, char * pDataNew )
{
    std::list<sensorDataListElem>::iterator it;
    for (it = sensorList.begin(); it != sensorList.end(); ++it){
        if( it->id == sensorIdx){
            char * pDestPtr = it->pData;
            *(pDestPtr) = (*pDataNew & (0xff));
            pDestPtr++;
            pDataNew++;
            *(pDestPtr) = (*pDataNew & (0xff));
            pDestPtr++;
            pDataNew++;
            *(pDestPtr) = (*pDataNew & (0xff));
            pDestPtr++;
            pDataNew++;
            *(pDestPtr) = (*pDataNew & (0xff));
            break;
        }
    }
}

std::list<CAN_MSG> canPayloadCreator::createCanMessages(void)
{
    /* You must define the number of can messages you want to generate per sample*/
    /* compress can messages as you see fit and add them to the list of can messages to be returned*/
    std::list<CAN_MSG> can_messages;
    CAN_MSG msg1;
    std::list<sensorDataListElem>::iterator it;
    
    it = sensorList.begin();
    msg1.type = PKT_TYPE_3;
    msg1.data.type3.sensor1Data = *((int *)(it->pData));
    
    can_messages.push_back(msg1);
    
    return can_messages;
}

