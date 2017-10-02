#include "mbed.h"
#include "canPayloadCreator.hh"
 
#define THREAD_TIME_INTERVAL_SEC    2
extern bool bLock;
extern canPayloadCreator * CPC;
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led1(LED1);

Ticker dataTimer;
bool bTimerFlag = false;

extern void thread_canPayloadCreator_main(void);

void timerCallback(void) {
    bTimerFlag = true;
}

void addDataToBuffer(unsigned int sensorIdx, int * pData){
    led1 = !led1;    

    if(CPC != NULL)
    {
        // TODO ADD MUTEX TO CPC AND CHECK IF IT'S HERE IF AVAILABLE
        if(CPC != NULL){
            pc.printf("%d\r\n", *pData);
            CPC->updateSensorData( sensorIdx, pData);  
        }
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
    
    // start CAN message threads
    //thread_CANager.start(thread_canPayloadCreator_main);

    // start payload creator thread
    Thread thread_CPC;
    thread_CPC.start(thread_canPayloadCreator_main);
    
    //  TODO: start data acquisition loops
    // SAMPLE INTERNAL DATA LOOP
    dataTimer.attach(&timerCallback, 0.3);
    while(1)
    {
        if(bTimerFlag == true){
            static int count = 1;
            while(bLock != false){}
            bLock = true;
            addDataToBuffer( SENSOR_1_ID , &count );
            bLock = false;
            count++;
            bTimerFlag = false;    
        }
        sleep();
    }
}