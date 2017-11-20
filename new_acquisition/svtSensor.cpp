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

timeout_state timeoutState;
Timeout sensorReadTimout;
void read_timeout_handler(){
	if(timeoutState == started)
		timeoutState = stalled;
}
/*! \brief Constructor
 *	\param Pointer to sensorManager object that holds the static system timer
 */
svtSensor::svtSensor(int32_t sensorID, int periodMs, float timeoutDur){	
	_sensorID = sensorID;
	lastSampleTimestamp = 0;
	samplePeriodMs = periodMs;
	timeout_durationS = timeoutDur;
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
void svtSensor::readSensor(timeout_state* pToState, char* pData){
}

int svtSensor::getSizeOfData(){
	return 0;
}

void svtSensor::sampleSensor(char * pData){
	
	lastSampleTimestamp = sensorMan.getTimeMs();
	/*
	//update time stamp
	lastSampleTimestamp = sensorMan.getTimeMs();
	timeoutState = started;
	sensorReadTimout.attach(&read_timeout_handler, 0.1);
	//read sensor data
	*/
	this->readSensor(&timeoutState, pData);
	
	//pc.printf("state:%d\r\n", timeoutState);
	//sensorReadTimout.detach();
}

bool svtSensor::timeToSample(){
	//.printf("lt:%d, s:%d, time:%d\r\n", lastSampleTimestamp, samplePeriodMs, sensorMan.getTimeMs());
	if( (lastSampleTimestamp + samplePeriodMs) < sensorMan.getTimeMs() ){
		return true;
	}else
	{
		return false;
	}
}
