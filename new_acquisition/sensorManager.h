/*!
 * \file sensorManager.hh
 * \class sensorManager
 * \brief Defines the class holding, modifying, and using the sensor data captured by the nucleo board.
 *
 * \author Beau Roland
*/

#ifndef CAN_PAYLOAD_CREATOR_HH
#define CAN_PAYLOAD_CREATOR_HH

#include <list>
#include "can_structs.h"

/// \brief structure that defines the data elements for each sensor
typedef struct{
    unsigned int id;	///< \brief Sensor ID
    char * pData;		///< \brief pointer to dynamically created memory that will hold the sensor data
    unsigned int size;  ///< \brief Number of bytes of data allocated for data per sensor
}sensorDataListElem;

class sensorManager
{
    public:
		Timer sysTime;
	
    private:
		std::list<sensorDataListElem> sensorList;
    
    public:
		/// \brief Constructor
        sensorManager();
		/// \brief Destructor
        ~sensorManager();
    
	public: 
	
		void startTimer();
        bool addSensor( unsigned int sensorId, unsigned int numBytes);
        void updateSensorData( unsigned int sensorIdx, char * pDataNew );
        void getSensorData(unsigned int sensorIdx);
        std::list<CAN_MSG> createCanMessages(void);
		
		//Accessor Functions
		float getTimeSec()	{ return sysTime.read(); };
		int getTimeMs()		{ return sysTime.read_ms(); };
		int getTimeUs()		{ return sysTime.read_us(); };
		
    private:

};

#endif

