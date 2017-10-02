#include "mbed.h"
#include "canPayloadCreator.hh"

canPayloadCreator * CPC;

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
    std::list<int>::iterator it;
    for (it = sensorList.begin(); it != sensorList.end(); ++it){
		if( it->id == sensorIdx){
			for(int a = 0; a< it->size; a++){
				it->pData[a] = pDataNew[a];
			}
			break;
		}
    }
}