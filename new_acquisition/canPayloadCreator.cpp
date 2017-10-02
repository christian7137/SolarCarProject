#include "mbed.h"
#include "canPayloadCreator.hh"
#include "can_structs.hh"

canPayloadCreator * CPC;
extern Serial pc;

/* CLASS BEGIN */
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
	return false;
	
}

void canPayloadCreator::updateSensorData( unsigned int sensorIdx, int * pDataNew )
{
    std::list<sensorDataListElem>::iterator it;
    for (it = sensorList.begin(); it != sensorList.end(); ++it){
		if( it->id == sensorIdx){
			for(int a = 0; a< it->size; a++){
				it->pData[a] = pDataNew[a];
			}
			break;
		}
    }
}

void canPayloadCreator::createCanMessage(void)
{
	/* You must define the number of can messages you want to generate per sample*/
	/* compress can messages as you see fit */
	CAN_MSG msg1;
	std::list<sensorDataListElem>::iterator it;
	it = sensorList.begin();
	
	msg1.type = PKT_TYPE_3;
	msg1.extraPayload = 0;
	msg1.data.data3.sensor1Data = (int)(*(int*)(it->pData));
	
	pc.printf("Payload: %d%d%d%d%d%d%d%d\r\n",
		msg1.byte0.data,
		msg1.data.pData[0],
		msg1.data.pData[1],
		msg1.data.pData[2],
		msg1.data.pData[3],
		msg1.data.pData[4],
		msg1.data.pData[5],
		msg1.data.pData[6]); 
}

