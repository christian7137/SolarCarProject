/*!
 * \file thread_main.cpp
 * \author Beau Roland
 * \brief Thread software that begins the program.
 * \details This thread initializes the sensors, the threads, and any timers
 * needed to run the software.
 *
 */
 
#include "mbed.h"
#include "BNO055.h"
#include "svtSensor.h"
#include "sensorManager.h"
 
#define THREAD_TIME_INTERVAL_SEC    2
#define SENSOR_1_ID 1

extern Mutex mutex_sensor_states;
extern sensorManager sensorMan;
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led1(LED1);

Ticker dataTimer;
bool bTimerFlag = false;

extern void thread_sensorManager_main(void);
extern void thread_canager_main(void);

/*!
 * \brief Main Thread's Timer Callback Function that set's a flag indicating that it is time to update our sample sensor data (a counter)
 */
void timerCallback(void) {
    bTimerFlag = true;
}
/*!
 * \brief Thread function that makes use of the CPC (sensorManager class) to update our sample sensor data
 */
void addDataToBuffer(unsigned int sensorIdx, char * pData){
    led1 = !led1;    

    if(sensorIdx == SENSOR_1_ID){
        BNO055_VECTOR_DATA * pVals;
        pVals = (BNO055_VECTOR_DATA *) pData;
        pc.printf("x:%d ", pVals->fields.x);
        pc.printf("y:%d ", pVals->fields.y);
        pc.printf("z:%d\r\n", pVals->fields.z);    
    }
    sensorMan.updateSensorData( sensorIdx, pData);  
}

/* main thread*/

/*!
 * \brief Main function of Data Aquisition Firmware
 * \details This function adds sensors to the list of sensors to create CAN Messages<br>
 * In addition, it starts the different threads.<br>
 * Finally, this main thread on a timely basis, increments a counter, updates our sample sensor data to be this counter, and sleeps till the next timer interrupt.
 */
int main() {

    Adafruit_BNO055 s1(SENSOR_1_ID, (0x28<<1), 1500, PB_9, PB_8);
    
    s1.init();
    
    sensorMan.addSensor(s1.getId(), s1.getSizeOfData());   
    
    mutex_sensor_states.unlock();
    
    // start CAN message threads
    Thread thread_CANager;
    thread_CANager.start(thread_canager_main);

    // start payload creator thread
    Thread thread_CPC;
    thread_CPC.start(thread_sensorManager_main);
    
    pc.printf("begin acq loop\r\n");
    while(1)
    {
        if(s1.timeToSample()){
            pc.printf("sample\t");
            
            bool bLock;
            do{
                bLock = mutex_sensor_states.trylock();
            }while( bLock == false);
            
            BNO055_VECTOR_DATA data;
            s1.sampleSensor(data.data);
            addDataToBuffer( s1.getId() , data.data );
            
           mutex_sensor_states.unlock();
        }
    }
}