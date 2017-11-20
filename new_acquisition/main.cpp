/*!
 * \file thread_main.cpp
 * \author Beau Roland
 * \brief Thread software that begins the program.
 * \details This thread initializes the sensors, the threads, and any timers
 * needed to run the software.
 *
 */
 
#include "mbed.h"
#include "svtSensor.h"
#include "CAN.h"
#include "canQueue.h"
#include "sensorManager.h"
#include "nucleo_hat_pin_names.h"
#include "config.h"
 
#ifdef CONFIG_1
#include "BNO055.h"
#include "TSL2561.h"
#endif
#ifdef CONFIG_2
#include "simGpsBps.h"
#endif

#define THREAD_TIME_INTERVAL_SEC    2

extern Mutex mutex_sensor_states;
extern sensorManager sensorMan;
extern canQueue CQ;
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led1(LED1);
DigitalOut led3(D5);
int CANaddress = 1337;
CAN MainCANbus(HAT_STM_CANRX, HAT_STM_CANTX);

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
       
#ifdef EN_PRINTF
    #ifdef CONFIG_1
        if(sensorIdx == SENSOR_ID_ACC){
            BNO055_VECTOR_DATA * pVals;
            pVals = (BNO055_VECTOR_DATA *) pData;
            pc.printf("x:%d ", pVals->fields.x);
            pc.printf("y:%d ", pVals->fields.y);
            pc.printf("z:%d\r\n", pVals->fields.z);    
        }
        
        if(sensorIdx == SENSOR_ID_LIGHT){
            TSL2561_DATA * pVals;
            pVals = (TSL2561_DATA *) pData;
            pc.printf("lumens: %d\r\n", pVals->fields.lumens);    
        }
    #endif
    #ifdef CONFIG_2
        if(sensorIdx == SENSOR_ID_GPS){
            SIM_GPS_BPS_DATA * pVals;
            pVals = (SIM_GPS_BPS_DATA *) pData;
            //pc.printf("lat:%f\r\n", pVals->fields.lat);
            //pc.printf("lon:%f\r\n", pVals->fields.lon);   
            pc.printf("%f,%f\r\n", pVals->fields.lat, pVals->fields.lon);
        }
    #endif
#endif
    sensorMan.updateSensorData( sensorIdx, pData);  
}

void createCanMessagesAndSend(){
    std::list<CAN_MSG> canMsgs;
    
    bool bLock;
    do{
        bLock = mutex_sensor_states.trylock();
    }while( bLock == false);
            
    canMsgs = sensorMan.createCanMessages();
    mutex_sensor_states.unlock();
    
    std::list<CAN_MSG>::iterator it;
    it = canMsgs.begin();
    
    //----------------SEND CAN MSG------------------------
#ifdef CONFIG_1
    CANMessage canMessage(CANaddress, it->raw_msg, 8);
    CAN_MSG * msg1;
    msg1 = (CAN_MSG * ) it->raw_msg;
    CQ.push(*msg1);
    it++;
    CANMessage canMessage2(CANaddress, it->raw_msg, 8);
    CQ.push(* (CAN_MSG * ) it->raw_msg);
    #ifdef CONFIG_2
        it++;
    #endif
#endif
#ifdef CONFIG_2
    CANMessage canMessage3(CANaddress, it->raw_msg, 8);
    CQ.push(* (CAN_MSG * ) it->raw_msg);
    it++;
    CANMessage canMessage4(CANaddress, it->raw_msg, 8);
    CQ.push(* (CAN_MSG * ) it->raw_msg);
#endif

}

#define freq_1MHZ   100000
void setup_can_bus()
{
    MainCANbus.frequency(freq_1MHZ);
    MainCANbus.reset();
}

bool emptyPacket(CAN_MSG msg){
    bool bEmpty = true;
    for(int a=1; a<8; a++){
        if(msg.raw_msg[a] != 0)
            bEmpty = false;  
    }
    
    return bEmpty;
}
/* main thread*/

/*!
 * \brief Main function of Data Aquisition Firmware
 * \details This function adds sensors to the list of sensors to create CAN Messages<br>
 * In addition, it starts the different threads.<br>
 * Finally, this main thread on a timely basis, increments a counter, updates our sample sensor data to be this counter, and sleeps till the next timer interrupt.
 */
int main() {
    pc.baud(921600);
    
    //----------- Create Sensors -----------------
#ifdef CONFIG_1
    Adafruit_BNO055 s1(SENSOR_ID_ACC, (HAT_BNO_ADDR<<1), 1000, HAT_BNO_SDA, HAT_BNO_SCL);
    TSL2561 s2(SENSOR_ID_LIGHT, HAT_TSL_ADDR , 1000, HAT_TSL_SDA, HAT_TSL_SCL);
#endif
#ifdef CONFIG_2
    simGpsBps s3(SENSOR_ID_GPS, 1000, HAT_UART_RX, HAT_UART_INT);
#endif
    //------------------- Init sensors -------------------------
#ifdef CONFIG_1
    s1.init();
    s2.init();
#endif
#ifdef CONFIG_2
    s3.init();
#endif

    //------------------ add sensors to sensor manager --------------------------
#ifdef CONFIG_1
    sensorMan.addSensor(s1.getId(), s1.getSizeOfData());  
    sensorMan.addSensor(s2.getId(), s2.getSizeOfData());
#endif
#ifdef CONFIG_2
    sensorMan.addSensor(s3.getId(), s3.getSizeOfData()); 
#endif
    
    setup_can_bus();
    
    mutex_sensor_states.unlock();
    sensorMan.startTimer();
    Ticker canCreateTimer;
    canCreateTimer.attach(&timerCallback, 1);
    pc.printf("begin acq loop\r\n");

    wait(1);
    //------------------- main processing loop -------------------------
    while(1)
    {    
        //-------------- sample sensor if it's time --------------------
#ifdef CONFIG_1
        if(s1.timeToSample()){
            BNO055_VECTOR_DATA data;
            s1.sampleSensor(data.data);
            addDataToBuffer( s1.getId() , data.data );
        }
        
        if(s2.timeToSample()){

            TSL2561_DATA data2;
            s2.sampleSensor(data2.data);
            addDataToBuffer( s2.getId() , data2.data );
        }
#endif
#ifdef CONFIG_2
        if(s3.timeToSample()){
            SIM_GPS_BPS_DATA data;
            s3.sampleSensor(data.data);
            addDataToBuffer( s3.getId() , data.data );
        }
#endif

        //--------------------- create CAN messages and add to can fifo -----------------------
        //TODO: change function name to correlate what it's actually doing
        if(bTimerFlag){
            led1 = !led1; 
            bTimerFlag = false;
            createCanMessagesAndSend();
        }
        
        //---------------------- send next CAN message ----------------------
        if(CQ.queueEmpty() == false){
            CAN_MSG nextMsg = CQ.getNextCanMsg();
    
            if(emptyPacket(nextMsg) == false){
                /* print the can message for debugging purposes */
#ifdef EN_PRINTF
                pc.printf("Out: 0x");
                for(int a=0; a<8; a++){
                    pc.printf("%2.2x", nextMsg.raw_msg[a]);
                }
                pc.printf("\r\n");
#endif
                //CANMessage canMessage(CANaddress, nextMsg.raw_msg, sizeof(nextMsg.raw_msg));
                CANMessage canMessage(CANaddress, nextMsg.raw_msg, 8);
                MainCANbus.write(canMessage);
                
                //wait some time to ensure that the message is sent
                //can controller can throw away data if we send to quickly
                wait(.01);
            }
        }
    }
}
