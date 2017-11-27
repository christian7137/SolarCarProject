/*!
	\file svtSensor.cpp
	\class svtSensor
	\brief Defines the parent sensor class that allows generic access to sample the sensor.

	\author J. C. Wiley, Sept. 2014<br>
	Beau Roland, Oct. 2017
*/

#include "svtSensor.h"

extern Serial pc;				///< \brief external accessor to print messages to the pc terminal
extern sensorManager sensorMan; ///< \brief external accessor to the system sensorManager

timeout_state timeoutState;		///< \brief structure dictating the state of the current sensor's sample read
Timeout sensorReadTimout;		///< \brief MBED safe timeout structure that provides timeout functionality in the event of a sensor connection loss

/*!
	\brief timeout callback function that is called when the timeout period has ended. Normally invoked if the sensor is disconnected while trying to
	read from the sensor. Allows the specific sensor sample function to exit prematurely.
 */
void read_timeout_handler(){
	if(timeoutState == started)
		timeoutState = stalled;
}
/*! \brief Constructor
	\param sensorID <int32_t> sensor Identifier for access use in the sensorManager
	\param periodMS <int> desired ms between sensor samples. i.e. 1/periodMs = data sample rate x 10^3 
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

/*! 
	\brief initialize sensor if necessary
 */
void svtSensor::init(){   
}

/*! 
	\brief generic function that samples the sensor without timeout functionality
 */
void svtSensor::readSensor(char * pData){
}
/*! 
	\brief generic function that samples the sensor with timerout functionality
 */
void svtSensor::readSensor(timeout_state* pToState, char* pData){
}

/*!
	\brief generic function that returns the size of the data that the sensor sample stores
 */
int svtSensor::getSizeOfData(){
	return 0;
}

/*!
	\brief wrapper function that updates the sensor's timestamp and samples the sensor
 */
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

/*!
	\brief wrapper function that checks the system time for whether it's time to sample the sensor
 */
bool svtSensor::timeToSample(){
	//.printf("lt:%d, s:%d, time:%d\r\n", lastSampleTimestamp, samplePeriodMs, sensorMan.getTimeMs());
	if( (lastSampleTimestamp + samplePeriodMs) < sensorMan.getTimeMs() ){
		return true;
	}else
	{
		return false;
	}
}