/*!
 * \file sensorManager.cpp
 * \class sensorManager
 *
 * \author Beau Roland
*/
#include "mbed.h"
#include "sensorManager.hh"
#include "can_structs.hh"
#include "BNO055.h"

sensorManager sensorMan;
Mutex mutex_sensor_states;
extern Serial pc;

/* CLASS BEGIN */

sensorManager::sensorManager()
{
	sysTime.reset();
}
//TODO: Consider deleting all of the dynamically created memory in the sensor list
sensorManager::~sensorManager()
{
}

void sensorManager::startTimer(){
	sysTime.start();	
}
/*!
 *	\brief Public function that adds a sensor to the list of sensors that data is collected and sent out via the CAN bus
 *	\param Sensor ID
 *	\param Number of bytes needed to store the sensor data. Will create memory dynamically for this size.
 *	\return Boolean whether we had the size to add the sensor to the list
 */
bool sensorManager::addSensor( unsigned int sensorId, unsigned int numBytes)
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
/*!
 *	\brief Public function that updates a given sensor's data
 *	\param The Sensor ID that we wish to update its data
 *	\param A pointer to the new data we wish to update
 */
void sensorManager::updateSensorData( unsigned int sensorIdx, char * pDataNew )
{
    std::list<sensorDataListElem>::iterator it;
    for (it = sensorList.begin(); it != sensorList.end(); ++it){
        if( it->id == sensorIdx){
            for(int a=0; a<it->size; a++){
            	it->pData[a] = pDataNew[a];
            }
        }
    }
}
/*!
 *	\brief Public function that creates CAN Message(s) based on the current state of all of the sensor data
 *	\return A list of CAN_MSG(s) dictating the current state of all of the sensor data
 */
std::list<CAN_MSG> sensorManager::createCanMessages(void)
{
    /* You must define the number of can messages you want to generate per sample*/
    /* compress can messages as you see fit and add them to the list of can messages to be returned*/
    std::list<CAN_MSG> can_messages;
    CAN_MSG msg1;
    std::list<sensorDataListElem>::iterator it;
    it = sensorList.begin();
    
    memset(&msg1, 0, 8);	// clear out the 8 bytes of can message
    msg1.payload.type = PKT_TYPE_2;
    BNO055_VECTOR_DATA * pStored = (BNO055_VECTOR_DATA *) (it->pData);
    msg1.payload.data.type2.accX = pStored->fields.x;
    msg1.payload.data.type2.accY = pStored->fields.y;
    msg1.payload.data.type2.accZ = pStored->fields.z;
    pc.printf("0x");
    for(int a=0; a<8; a++){
    	pc.printf("%2.2x", msg1.raw_msg[a]);
    }
    pc.printf("\r\n");
    can_messages.push_back(msg1);
    
    return can_messages;
}



