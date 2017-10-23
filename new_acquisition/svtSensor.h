/*!*************************************************************
   \file  svtSensor.h

   \brief Base class for sensors
   \author 	J. C. Wiley, Sept. 2014
			Beau Roland, Oct. 2017
***************************************************************/
#ifndef _SVTSENSOR_H_
#define _SVTSENSOR_H_

#include "mbed.h"
#include "sensorManager.hh"

class svtSensor{
public:
    /** svtSensor - Constructor
     */
    svtSensor(int32_t sensorID, int periodMs);

    /** svtSensor - Destructor
     */
    ~svtSensor();

    /** init - \brief initialize sensor if necessary
     */
    virtual void init();
    

    /*! readSensor \brief read this sensor's data
     *  @return error code
     */
    virtual void readSensor(char* pData);
    
    virtual int getSizeOfData();
    
    /*! \brief intermediate universal step to update the sample time step before proceeding to the specific read sensor function
    */
    void sampleSensor(char * pData);

	/*! timeToSample \brief compares the current time with the last time we sampled, if greater than threshold, return true 
	*/
	bool timeToSample();
	
    /*! getPeriod - 
	@return samplePeriodMs -  in ms
    */
    int getPeriod(){return samplePeriodMs;}
   
   	int32_t getId(){ return _sensorID; }
private:
	int32_t _sensorID;
    int samplePeriodMs; 	// ms - update sensor at this rate, may be same as report rate or sample rate.
	float lastSampleTimestamp;
};   

#endif

