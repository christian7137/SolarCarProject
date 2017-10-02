#include "mbed.h"
#include "canPayloadCreator.hh"
#include "sharedWithAcquisitionSW.hh"
 
#define THREAD_TIME_INTERVAL_SEC    2
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led1(LED1);

Ticker dataTimer;
bool bTimerFlag = false;

void timerCallback(void) {
    bTimerFlag = true;
}

void addDataToBuffer(unsigned int sensorIdx, int * pData){
    led1 = !led1;    

	if(CPC != NULL)
	{
		// TODO ADD MUTEX TO CPC AND CHECK IF IT'S HERE IF AVAILABLE
		CPC->updateSensorData( sensorIdx, pData );	
	}    
}

/* main thread*/
#define SENSOR_1_ID 1
int main() {
	/*INIT storage classes needed for all of the threads*/
    CPC = new canPayloadCreator();
    
    // initialize sensors and add a storage element to the data buffer
    //test, creating one sensor
    CPC->addSensor(SENSOR_1_ID, sizeof(int));   
    
    CPC->Init();
	
    // start CAN message threads
    thread_CANager.start(thread_canPayloadCreator_main);

	// start payload creator thread
	thread_CPC.start(thread_canPayloadCreator_main);
	
	//  TODO: start data acquisition loops
	// SAMPLE INTERNAL DATA LOOP
    dataTimer.attach(&timerCallback, SENSOR_ACQ_SECS);
    while(1)
    {
        if(bTimerFlag == true){
            static int count = 1;
            while(lock != AVAIL){}
            lock = LOCKED;
            addDataToBuffer( SENSOR_1_ID , &count );
            lock = AVAIL;
            count++;
            bTimerFlag = false;    
        }
        sleep();
    }
}