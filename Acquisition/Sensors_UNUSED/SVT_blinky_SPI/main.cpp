/** \file main.cpp
 * Note that each mbed processor has a unique id, which is determined differently
 * for each type of processor e.g. K64F or Nucleo-F429ZI. The unique id is used
 * to generate a local to the car environment, a unique processor number. The
 * processor number is used to generate the CAN bus address which depends on not
 * only the processor number but the sensors attached to the board. If the processor
 * is not recognized it prints message local serial number so it can be entered into
 * a table assigning local processor numbers to specific boards. There maybe more
 * than one processor number assigned to a given CAN address, in the case of a spare
 * processor, but at one time there should only be one board with a given processor
 * number in the system.
 */

#include "mbed.h"
#include "rtos.h" 
#include "RawSerial.h"
#include "mbed_events.h"

#undef k65f
#define stm32

#ifdef stm32
#define LED_GREEN LED1
#define LED_BLUE  LED2
#define LED_RED   LED3
#include "STM32ID.h"
#endif

#include "svtSensorMgr.h"
#include "svtSTM32CAN.h"
#include "GreenLEDsensor.h"
#include "SPIsensor.h"
// #include "svtGPSsensor.h"
// #include "svtShutdownSensor.h"
// #include "svtTPMSsensor.h"
// #include "svtMPPTsensor.h"
// #include "svtDASHsensor.h"
// #include "svtBPSsensor.h"
// #include "svtIMUsensor.h"
// #include "svtCNTRLsensor.h"

// -----  select active sensors -----
// essential:
//#define senNGM
//#define senDASH
//#define senBPS
//#define senCNTRL
//#define senSHUTDOWN

// informational
//#define senGAP
//#define senGPS
#define senBLINKYGREEN
#define senSPI
//#define senTPMS
//#define senMPPT 
//#define senIMU

//---define active boards CAN addresses (processor part only)
//#define STM32ONECANADDR     0x0100    // Nucleo prototype card with GPS
// #define TEXSUNONECANADDR 0x0300
// #define MOTORCANADDR     0x0400
#define SENSORONECANADDR 0x0100

// // Pin Usage: TexsunOne shield
// #define EstopPin           PTB10  // J4-6  PTB10
// #define BootKillPin        PTB11  // J4-8  PTB11
// #define MotorPrechargePin  PTC2   // J1-14 PTC2
// #define MotorContactorPin  PTA2   // J1-12 PTA2
// #define ArrayPrechargePin  PTB23  // J1-10 PTB23
// #define ArrayContactorPin  PTA1   // J1-8  PTA1
// #define ArrayAPin          PTD1   // J2-12 PTD1
// #define ArrayBPin          PTE25  // J2-18 PTE25
// #define ArrayCPin          PTE24  // J2-20 PTE24
// #define V12sensePin        PTB2   // J4-2  PTB2
// #define V24sensePin        PTB3   // J4-4  PTB3
// #define DashRxPin          PTC16  // J1-2  U3-RX
// #define DashTxPin          PTC17  // J1-4  U3-TX
// #define BPStxPin           PTC4   // J2-4  U1-TX
// #define BPSrxPin           PTC3   // J1-16 U1-RX

// // Pin Usage: MotorControl shield
// #define GapSwPin           PTC4   // J2-4
// #define GapDirPin          PTE24  // J2-20
// #define GapPosPin          PTB10  // J4-6
// #define NGMRxPin           PTC16  // J1-2 U3rx
// #define NGMTxPin           PTC17  // J1-4 U3tx
// #define ThrInPin           PTB3   // J4-4
// #define RgInPin            PTB2   // J4-2
// #define ThrEnInPin         PTB23  // J1-10
// #define FwdRevPin          PTA2   // J1-12

// // Pin Usage: SensorOne shield
// #define GPStxPin           PG_14    // STM32  --- K64F    PTC4   // J2-4  U1-TX
// #define GPSrxPin           PG_9     // STM32  --- K64F   PTC3   // J1-16 U1-RX
// #define SPIncsPin          PTD0   // J2-6  SPI-ncs    MCP2515 - SEEEDCAN - MPPT
// #define SPImosiPin         PTD2   // J2-8  SPI-mosi   MCP2515 - SEEEDCAN - MPPT
// #define SPImisoPin         PTD3   // J2-10 SPI-miso   MCP2515 - SEEEDCAN - MPPT
// #define SPIsckPin          PTD1   // J2-12 SPI-clk    MCP2515 - SEEEDCAN - MPPT
// #define SPIintPin          PTB9   // J1-6  interrupt  MCP2515 - SEEEDCAN - MPPT

// ----- select CAN address type ----
#define SVTCANFORMAT CANStandard
//#define SVTCANFORMAT CANSExtended

#define CANSPEED 100000 
#define SPISPEED 500000

// globals
RawSerial udebug(USBTX, USBRX);          // USB virtual serial port
//DigitalOut _ledG(LED_GREEN);
DigitalOut _ledB(LED_BLUE);
DigitalOut ledR(LED_RED);

int CANaddr;  ///< processor based can address

/** @name	msClockStepper 
  *	@brief 	A millisecond clock stepper function
  * @param *tick an EventQueue tick
  * @return void - no output
  * 
  * This method does something documented here.
  * And returns exceptionally void results.
  */
void msClockStepper(void const *tick) {
    uint32_t *t;
    t  = (uint32_t *)tick;
    (*t)++;
}

/**
  * @name	whoami
  * @brief	determines which board of multi processor system
  * @return PROCID	an int representing the local processor number
  *
  * side effect - set global CANaddr for this processor.
**/
int whoami(){
#ifdef stm32
    STM32ID catalog;
    int PROCID = catalog.getID();  ///< Processor ID - derived from serial number
    if(PROCID == -1){
	udebug.printf("new serial: 0x%X  not in database\r\n", 
		      DBGMCU->IDCODE);
    }

    CANaddr = (catalog.getCANaddress(PROCID) & 0x0007) << 8; //last 3 bits of processor local id
    return PROCID;
#endif
    //return 0;
}

/** main - main is first thread
 */ 
int main(){

    //DigitalOut* tempGapSW;

    uint32_t msclock = 0;
    RtosTimer MsClock(msClockStepper, osTimerPeriodic, (void *)&msclock);
    MsClock.start(1); ///<period in ms

    EventQueue clockQ;
    clockQ.tick();
    
    ledR = 1;
    _ledB = 1;

    // Find board serial number and assign local board ID, and CAN address
    //show build date and time
    udebug.printf("  start - build: ");
    udebug.printf(__DATE__);
    udebug.printf(" ");
    udebug.printf(__TIME__);
    udebug.printf("\r\n");	

    // // if motor controller shield, turn off Gap motor as soon as possible
    // if(CANaddr == MOTORCANADDR){	
    // 	tempGapSW = new DigitalOut(GapSwPin, GAPMOTOROFF);
    // }

    wait(0.25); /// wait briefly for everything to settle
    udebug.printf("\r\n CAN processor addr %x, local ID \r\n", CANaddr);
  
    svtSysState SS;
    svtSensorMgr smanager;
    svtCAN*  canbus; 
    int32_t err;

    // setup can bus
#ifdef stm32
    svtSTM32CAN can1;
#endif

    canbus = &can1;  ///< select active CAN bus

    if(canbus == 0 ) udebug.printf("CAN bus is zero \r\n");
    
// initialize  CAN
    if(canbus != 0){
	err = canbus->init(CANSPEED, SVTCANFORMAT, CANaddr, &smanager);
	if(err != 0){
	    udebug.printf("CAN init failed\r\n");
	}
    }


#ifdef senBLINKYGREEN
	// create blinking green led 
	svtSensor* gblinking = new GreenLEDsensor();
//	gblinking->setCANBASE(CANaddr + (GreenLedSensorID << 4));
//	gblinking->attachCAN(canbus);   // attach sensor to CAN bus
	gblinking->setPeriod(300);       // set update period (ms)
        gblinking->attachClock(&msclock);

	// initialize sensor and add to sensor Manager
	if(!(err = gblinking->init())){
	    smanager.addSensor(gblinking);
	}
	else{  // process initialization error - report failure
	    udebug.printf("Gblink failed to initialize.\r\n");
	}
#endif

#ifdef senSPI
	// create blinking green led 
	svtSensor* spitest = new SPIsensor();
//	gblinking->setCANBASE(CANaddr + (GreenLedSensorID << 4));
//	gblinking->attachCAN(canbus);   // attach sensor to CAN bus
	spitest->setPeriod(800);       // set update period (ms)
        spitest->attachClock(&msclock);

	// initialize sensor and add to sensor Manager
	if(!(err = spitest->init())){
	    smanager.addSensor(spitest);
	}
	else{  // process initialization error - report failure
	    udebug.printf("SPI failed to initialize.\r\n");
	}
#endif

    // sensor configuration complete
    smanager.listSensors();

// run - main thread not doing any work,
    while(true){
        int in1, out1;
	Thread::wait(400);
        in1 = canbus->getCANin();
        out1 = canbus->getCANout();
	//Thread::wait(600);
        _ledB = !_ledB;

	//udebug.printf("\r\n %d out %d  in %d \r\n", msclock, out1, in1);
        //udebug.printf("%d Txerr %x Rxerr %x \r\n", msclock, canbus->TxErrors(), canbus->RxErrors());
    } 
}
 
 

