/*!
	\file sensorManager.cpp
	\class sensorManager

	\author Beau Roland
*/
#include "mbed.h"
#include "sensorManager.h"
#include "can_structs.h"
#include "config.h"

#include "BNO055.h"
#include "TSL2561.h"
#include "simGpsBps.h"

sensorManager sensorMan;

/// \brief external access to print messages to the pc terminal
extern Serial pc;

/* CLASS BEGIN */
/// \brief constructor
sensorManager::sensorManager()
{
	sysTime.reset();
}
/// \brief destructor
/// \warning Consider deleting all of the dynamically created memory in the sensorList
sensorManager::~sensorManager()
{
}

/*!
	\brief function that resets the system timer
 */
void sensorManager::resetTimer(){
	sysTime.reset();
}

/*!
	\brief function that starts the system timer
 */
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
    std::list<sensorDataListElem>::iterator it;
    it = sensorList.begin();

#ifdef CONFIG_1
    CAN_MSG msg1,msg2;
    //it points to the first sensor in the sensor list, i.e. accelerometer data
    memset(&msg1, 0, 8);	// clear out the 8 bytes of can message
    msg1.payload.type = SENSOR_ID_ACC;
    BNO055_VECTOR_DATA * pStored = (BNO055_VECTOR_DATA *) (it->pData);
    msg1.payload.data.acc.accX = pStored->fields.x;
    msg1.payload.data.acc.accY = pStored->fields.y;
    msg1.payload.data.acc.accZ = pStored->fields.z;
    
	#ifdef EN_PRINTF
	    pc.printf("0x");
	    for(int a=0; a<8; a++){
	    	pc.printf("%2.2x", msg1.raw_msg[a]);
	    }
	    pc.printf("\r\n");
	#endif
	
	it++;	//next data, i.e. luminosity sensor, is in the next sensor in the sensor list
	memset(&msg2, 0, 8);	// clear out the 8 bytes of can message
    msg2.payload.type = SENSOR_ID_LIGHT;
    TSL2561_DATA * pStored2 = (TSL2561_DATA *) (it->pData);
    msg2.payload.data.light.lumens = pStored2->fields.lumens;
	#ifdef EN_PRINTF
	    pc.printf("0x");
	    for(int a=0; a<8; a++){
	    	pc.printf("%2.2x", msg2.raw_msg[a]);
	    }
	    pc.printf("\r\n");
	#endif

    can_messages.push_back(msg1);
    can_messages.push_back(msg2);
    
    #ifdef CONFIG_2
    	it++;	//next data, simulated data
    #endif
#endif

#ifdef CONFIG_2
    CAN_MSG msgLat,msgLon;
    //it points to the first sensor in the sensor list, i.e. simulated GPS BPS sensor data
    memset(&msgLat, 0, 8);	// clear out the 8 bytes of can message
    msgLat.payload.type = SENSOR_ID_GPS;
    SIM_GPS_BPS_DATA * pStored3 = (SIM_GPS_BPS_DATA *) (it->pData);
    
    msgLat.payload.data.lat.lat = pStored3->fields.lat;
	
	memset(&msgLon, 0, 8);	// clear out the 8 bytes of can message
    msgLon.payload.type = SENSOR_ID_GPS + 1;
    
    msgLon.payload.data.lon.lon = pStored3->fields.lon;

	#ifdef EN_PRINTF
	    pc.printf("0x");
	    for(int a=0; a<8; a++){
	    	pc.printf("%2.2x", msgLat.raw_msg[a]);
	    }
	    pc.printf("\r\n");
		pc.printf("0x");
	    for(int a=0; a<8; a++){
	    	pc.printf("%2.2x", msgLon.raw_msg[a]);
	    }
	    pc.printf("\r\n");
	#endif
	
    can_messages.push_back(msgLat);
    can_messages.push_back(msgLon);
#endif
    
    return can_messages;
}