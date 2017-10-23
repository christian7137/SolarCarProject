/***************************************************************
   -
   - file:  svtSensor.cpp
   -
   - purpose: base class for F64K sensors
   -
   - author: J. C. Wiley, Sept. 2014
   -
***************************************************************/
#include "svtSensor.h"
extern Serial pc;

extern sensorManager sensorMan;

/*! \brief Constructor
 *	\param Pointer to sensorManager object that holds the static system timer
 */
svtSensor::svtSensor(int32_t sensorID, int periodMs){
	pc.printf("b");
	
	_sensorID = sensorID;
	lastSampleTimestamp = 0;
	samplePeriodMs = periodMs;
}

/*! \brief Destructor*/
svtSensor::~svtSensor(){
}

/*! \brief initialize sensor if necessary
 */
void svtSensor::init(){   
}

void svtSensor::readSensor(char * pData){
}

int svtSensor::getSizeOfData(){
	return 0;
}

void svtSensor::sampleSensor(char * pData){
	//update time stamp
	lastSampleTimestamp = sensorMan.getTimeSec();
	//read sensor data
	this->readSensor(pData);
}
bool svtSensor::timeToSample(){
	if( (lastSampleTimestamp + (samplePeriodMs/1000)) < sensorMan.getTimeSec() ){
		return true;
	}else
	{
		return false;
	}
}