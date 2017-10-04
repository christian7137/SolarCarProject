/*!
 * \file canPayloadCreator.hh
 * \class canPayloadCreator
 * \brief Defines the class holding, modifying, and using the sensor data captured by the nucleo board.
 *
 * \author Beau Roland
*/

#ifndef CAN_PAYLOAD_CREATOR_HH
#define CAN_PAYLOAD_CREATOR_HH

#include <list>
#include "can_structs.hh"

/// \brief structure that defines the data elements for each sensor
typedef struct{
    unsigned int id;	///< \brief Sensor ID
    char * pData;		///< \brief pointer to dynamically created memory that will hold the sensor data
    unsigned int size;  ///< \brief Number of bytes of data allocated for data per sensor
}sensorDataListElem;

class canPayloadCreator
{
    public:
    
    private:
    std::list<sensorDataListElem> sensorList;
    
    public:
		/// \brief Constructor
        canPayloadCreator();
		/// \brief Destructor
        ~canPayloadCreator();
    
	public: 
        bool addSensor( unsigned int sensorId, unsigned int numBytes);
        void updateSensorData( unsigned int sensorIdx, char * pDataNew );
        void getSensorData(unsigned int sensorIdx);
        std::list<CAN_MSG> createCanMessages(void);
		
    private:

};

#endif
