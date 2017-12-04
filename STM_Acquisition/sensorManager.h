/*!
	\file sensorManager.h
	\class sensorManager
	\brief Defines the class holding, modifying, and using the sensor data captured by the nucleo board.

	\author Beau Roland
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
		Timer sysTime;	///< \brief System timer
	
    private:
		std::list<sensorDataListElem> sensorList;	///< \brief list of sensors that memory has been allocated to store sensor data
    
    public:
		/// \brief Constructor
        sensorManager();
		/// \brief Destructor
        ~sensorManager();
    
	public: 

	
		void resetTimer();													///< \brief helper function that resets the system timer
		void startTimer();													///< \brief helper function that starts the system timer
        bool addSensor( unsigned int sensorId, unsigned int numBytes);		///< \brief function that adds a sensor to the sensorList
        void updateSensorData( unsigned int sensorIdx, char * pDataNew );	///< \brief function that updates a sensor's data
        std::list<CAN_MSG> createCanMessages(void);							///< \brief function that creaets can messages based on the data stored in the sensorList
		
		//Accessor Functions
		/// \brief returns the system time in seconds (float)
		float getTimeSec()	{ return sysTime.read(); };
		/// \brief returns the system time in ms (int)
		int getTimeMs()		{ return sysTime.read_ms(); };
		/// \brief returns the system time in us (int)
		int getTimeUs()		{ return sysTime.read_us(); };
		
    private:

};

#endif