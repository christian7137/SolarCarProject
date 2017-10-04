/*!
 * \class canPayloadCreator
 * \brief Defines the class holding, modifying, and using the sensor data captured by the nucleo board.
 *
 * \author Beau Roland
*/

#ifndef CAN_PAYLOAD_CREATOR_HH
#define CAN_PAYLOAD_CREATOR_HH

#include <list>
#include "can_structs.hh"

typedef struct{
    unsigned int id;
    char * pData;
    unsigned int size;  //bytes of data allocated for data per sensor
}sensorDataListElem;

class canPayloadCreator
{
    public:
    
    private:
    std::list<sensorDataListElem> sensorList;
    
    /* constructors, destructors, and member functions*/
    public:
        canPayloadCreator();    //constructor
        ~canPayloadCreator();   //destructor
    
        /* initialize functions */
        bool addSensor( unsigned int sensorId, unsigned int numBytes);
        void Init(void);
        
        /* public access and modify functions */
        void updateSensorData( unsigned int sensorIdx, char * pDataNew );
        void getSensorData(unsigned int sensorIdx);
        std::list<CAN_MSG> createCanMessages(void);
    private:

};

#endif
