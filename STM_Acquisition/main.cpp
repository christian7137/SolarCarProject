/*!
 * \file 		main.cpp
 * \author 		Beau Roland
 *
 * \brief 		The main software superloop that controls the data acquisition system.
 *
 * \details 	This program initializes a list of sensors, a system timer, and period timer, and a CAN fifo.
 * It continues by looping through the sensors one by one, comparing the system timer against
 * the last time the sensor was sampled + the sensor's sample period. It samples the sensor
 * if it's time. Regardless, it moves on to the next sensor. After looking at all of the sensors,
 * we look to see the period CAN create message timer flag has been raised ( is it time to create CAN messages
 * that dictate the state of all of the sensors). If so, we create CAN messages for all of the data and push
 * them onto the CAN fifo. Finally, at the end of the super loop, we send the next CAN message if it's not empty.
 * <br>
 * <br>
 * A final note: config.h defines different configurations that dictate which sensors are compiled.
 * Thus, by defining in a configuration, the software automatically compiles the sensor objects and supporting
 * hardware.
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

extern sensorManager sensorMan;
extern canQueue CQ;

Serial pc(USBTX, USBRX); 	///< \brief object used to print messages to the pc serial console. Configured for a faster baud rate, 921600. 
DigitalOut led1(LED1);		///< \brief on board led used to dictate can message creation activity 
DigitalOut led3(D5);		///< \brief on hat led used to dictate... not used

int CANaddress = 1337;
CAN MainCANbus(HAT_STM_CANRX, HAT_STM_CANTX);	///< \brief CAN bus connection between the nucleo and the RPi collection hub

bool bTimerFlag = false;	///< \brief global flag that dictates whether we need to create CAN messages
							///< \details to keep the interrupt routine short, we allow the main foreground to handle the interrupt.
							///< Therefore, the timer interrupt sets this flag. The main super loop acknowledges this flag by
							///< clearing it and creates the can messages.
Ticker canCreateTimer;		///< \brief timer that controls when we should create can messages

/*!
 * \brief Main Timer Callback Function that set's a flag indicating that it is time create can messages
 */
void timerCallback(void) {
    bTimerFlag = true;
}

/*!
 * \brief Function that makes use of the sensorManager to update our sample sensor data.
 * \details In this system, we populate some shared memory int he sensorManager for each sensor.
 * Therefore, everytime we want to update a sensors data, we need to go through this object,
 * find the location of memory reserved for the sensor (given by it's sensorId), and update the data.
 *
 * \param sensorIdx < unsigned int > sensor ID that was used in the initialization phase. can be accessed from the sensor object as well.
 * \param pData < char * > pointer to the data that we wish to push into storage
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

/*!
 * \brief Function that creates can messages and adds them to the tx CAN fifo
 */
void createSensorCanMessages(){
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

/*!
 * \brief Initialization function that sets up the tx CAN bus.
 */
#define freq_1MHZ   100000
void setup_can_bus()
{
    MainCANbus.frequency(freq_1MHZ);
    MainCANbus.reset();
}

/*!
 * \brief helper function that determines whether a CAN message is empty.
 * \details The function was introduced because at the start of running this program,
 * the system will determine that it is immediately time to create a CAN message. In
 * doing so, all of the data is zeroed out. This causes a problem on the visualization
 * side. (aka, you probably are not at a GPS location of 0 degrees N, 0 degrees W. and
 * experience 0 g's in all x,y,and z axis). To resolve this problem, for certain sensor
 * ids, we check to see if the CAN payload portion of the can message is filled with 0's.
 *
 * \param msg <CAN_MSG> can message of 8 bytes that will be examined to see if it's
 * payload is zeroed out.
 * \return bool boolean that is true if it's an empty CAN message
 */
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
 * \details Initializes a list of sensors, adds them to a sensorManager, sets up a CAN creation timer,
 * and begins the super loop. In the super loop, we sample the sensors on a timely basis (based on the
 * sensor's sample period), create can messages, and send can messages.
 */
int main() {
    pc.baud(921600);
    
    //----------- Create Sensors -----------------
#ifdef CONFIG_1
    Adafruit_BNO055 s1(SENSOR_ID_ACC, (HAT_BNO_ADDR<<1), 1000, HAT_BNO_SDA, HAT_BNO_SCL);	///< \brief 9 DOF sensor
    TSL2561 s2(SENSOR_ID_LIGHT, HAT_TSL_ADDR , 1000, HAT_TSL_SDA, HAT_TSL_SCL);				///< \brief Light Flux sensor
#endif
#ifdef CONFIG_2
    simGpsBps s3(SENSOR_ID_GPS, 1000, HAT_UART_RX, HAT_UART_INT);							///< \brief Simulated GPS data via a serial rx and gpio request interface
																							///< \details Data is requested by lowering the request signal, then the external
																							///< board sends a serial message that is '\n' terminated. I.e. ####.#####,####.####\n
																							///< (Latitude,Longitude Format)
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
    sensorMan.startTimer();
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
        if(bTimerFlag){
            led1 = !led1; 
            bTimerFlag = false;
            createSensorCanMessages();
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