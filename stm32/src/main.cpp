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
#include "svtGPSsensor.h"
#include "svtShutdownSensor.h"
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
#define senSHUTDOWN

// informational
//#define senGAP
#define senGPS
//#define senBLINKYGREEN
//#define senTPMS
//#define senMPPT 
//#define senIMU

//---define active boards CAN addresses (processor part only)
#define STM32ONECANADDR     0x0100    // Nucleo prototype card with GPS
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
#define GPStxPin           PG_14    // STM32  --- K64F    PTC4   // J2-4  U1-TX
#define GPSrxPin           PG_9     // STM32  --- K64F   PTC3   // J1-16 U1-RX
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
DigitalOut _ledB(LED_BLUE);
DigitalOut ledR(LED_RED);

int CANaddr;  // processor based can address

void msClockStepper(void const *tick) {
    uint32_t *t;
    t  = (uint32_t *)tick;
    (*t)++;
}

/*
  whoami - determines which board of multi processor system
  @return - int, local processor number
  side effect - set global CANaddr for this processor.
**/
int whoami(){
#ifdef k64f
    K64FID catalog;
    int PROCID = catalog.getID();  // Processor ID - derived from serial number
    CANaddr = (catalog.getCANaddress(PROCID) & 0x0007) << 8; //last 3 bits of processor local id
    return PROCID;
#endif
#ifdef stm32
    STM32ID catalog;
    int PROCID = catalog.getID();  // Processor ID - derived from serial number
    if(PROCID == -1){
	udebug.printf("new serial: 0x%X  not in database\r\n", 
		      DBGMCU->IDCODE);
    }

    CANaddr = (catalog.getCANaddress(PROCID) & 0x0007) << 8; //last 3 bits of processor local id
    return PROCID;
#endif
    return 0;
}

/** main - main is first thread
 */ 
int main(){

    //DigitalOut* tempGapSW;
    int PROCID;

    uint32_t msclock = 0;
    RtosTimer MsClock(msClockStepper, osTimerPeriodic, (void *)&msclock);
    MsClock.start(1); //period in ms

    EventQueue clockQ;
    clockQ.tick();
    
    ledR = 1;
    _ledB = 1;

    // Find board serial number and assign local board ID, and CAN address
    //show build date and time
    PROCID = whoami();
    udebug.printf("  start - build: ");
    udebug.printf(__DATE__);
    udebug.printf(" ");
    udebug.printf(__TIME__);
    udebug.printf("\r\n");	

    // // if motor controller shield, turn off Gap motor as soon as possible
    // if(CANaddr == MOTORCANADDR){	
    // 	tempGapSW = new DigitalOut(GapSwPin, GAPMOTOROFF);
    // }

    wait(0.25); // wait briefly for everything to settle
    udebug.printf("\r\n CAN processor addr %x, local ID %d\r\n", CANaddr, PROCID);
  
    svtSysState SS;
    svtSensorMgr smanager;
    svtCAN*  canbus; 
    int32_t err;

    // setup can bus
#ifdef k64f
    svtK64FCAN  can1;
#endif
#ifdef stm32
    svtSTM32CAN can1;
#endif

    canbus = &can1;  // select active CAN bus

    if(canbus == 0 ) udebug.printf("CAN bus is zero \r\n");
    
// initialize  CAN
    if(canbus != 0){
	err = canbus->init(CANSPEED, SVTCANFORMAT, CANaddr, &smanager);
	if(err != 0){
	    udebug.printf("CAN init failed\r\n");
	}
    }

#define NOFILTERING
//#undef NOFILTERING  // filtering appears to work
#ifndef NOFILTERING
// set up to six filter ids. Selects which CAN messages this processor
// sees. Excludes own messages. Filters only the processor number from
// the CAN address.
    int n = 6; // max allowed by mbed.
    int32_t filters[6];
    filters[3] = 0x0100;  // SensorOne - IMU, MPPT, GPS, TPMS
    filters[2] = 0x0200;
    filters[1] = 0x0300;  // TexsunOne - DASH, CNTRL, BPS
    filters[0] = 0x0400;  // Motor - NGM, GAP
    filters[4] = 0x0100;
    filters[5] = 0x0100;
    for(int i=0; i<n; i++){
	if(filters[i] == CANaddr){  // excludes self
	    filters[i] = 0x0000;
	}
    }
 
    // pick out processor part of id( top 3 bits) - only tests bits with 1 in mask.
    if(canbus->Filter(PROCESSORMASK, filters, n)){
    	udebug.printf("set can filter ids failed \r\n");
    }
#endif

#ifdef senFXOS8700CQ
    if(CANaddr == TEXSUNONECANADDR){
// create Accelerometer/magnetometer sensor
	svtSensor* acc_mag = new FXOS8700CQ(PTE25, PTE24, FXOS8700CQ_SLAVE_ADDR1);
	acc_mag->setCANBASE( CANaddr + (FXOS8700CQID << 4));
	acc_mag->attachCAN(canbus);   // attach sensor to CAN bus
	acc_mag->setPeriod(250);       // set update period (ms)

// initialize sensor and add to sensor Manager
	if(!(err = acc_mag->init())){
	    smanager.addSensor(acc_mag);
	}
	else{  // process initialization error - report failure

	}
    }
#endif 

#ifdef senGAP
    if(CANaddr == MOTORCANADDR){
// create GAP sensor
	delete tempGapSW;
	svtSensor* gap = new svtGAPsensor(GapSwPin, GapDirPin, GapPosPin);
	gap->setCANBASE( CANaddr + (svtGAPsensorID << 4));
	gap->attachCAN(canbus);   // attach sensor to CAN bus
	gap->setPeriod(100);      // set update period (ms)
	gap->attachSysState(&SS);
	gap->attachClock(&msclock);

// initialize sensor and add to sensor Manager
	if(!(err = gap->init())){
	    smanager.addSensor(gap);
	    udebug.printf("GAP: initialize \r\n");
	}
	else{  // process initialization error - report failure
            udebug.printf("GAP: not initialize \r\n");
	}
    }
#endif

#ifdef senNGM
    if(CANaddr == MOTORCANADDR){
// create NGM sensor
	svtSensor* ngm = new svtNGMsensor(NGMTxPin, NGMRxPin, 0, ThrInPin, RgInPin, ThrEnInPin, FwdRevPin);
	ngm->setCANBASE( CANaddr + (svtNGMsensorID << 4));
	ngm->attachCAN(canbus);   // attach sensor to CAN bus
	ngm->setPeriod(35);      // set update period (ms) NGM need keep alive message
	ngm->attachSysState(&SS);
	ngm->attachClock(&msclock);

// initialize sensor and add to sensor Manager
	if(!(err = ngm->init())){
	    smanager.addSensor(ngm);
	    udebug.printf("NGM: initialize \r\n");
	}
	else{  // process initialization error - report failure
	    udebug.printf("NGM: not initialize \r\n");
	}
    }
#endif

#ifdef senDASH
    if(CANaddr == TEXSUNONECANADDR){
// create DASH sensor
	svtSensor* dash = new svtDASHsensor(DashTxPin, DashRxPin, 0); // U3 pin J1 4 & 2
	dash->setCANBASE( CANaddr + (svtDASHsensorID << 4));
	dash->attachCAN(canbus);   // attach sensor to CAN bus
	dash->setPeriod(15);       // set update period (ms)
	dash->attachSysState(&SS);  
        dash->attachClock(&msclock);

// initialize sensor and add to sensor Manager
	if(!(err = dash->init())){
	    smanager.addSensor(dash);
            udebug.printf("dash initialized \r\n");
	}
	else{  // process initialization error - report failure
	    udebug.printf("dash did not initialize \r\n");
	}
    }
#endif
    

#ifdef senCNTRL
    if(CANaddr == TEXSUNONECANADDR){
// create CNTRL sensor
	svtSensor* cntrl = new svtCNTRLsensor(EstopPin, BootKillPin, 
					      MotorPrechargePin, MotorContactorPin, 
					      ArrayPrechargePin, ArrayContactorPin,
					      ArrayAPin, ArrayBPin, ArrayCPin, 
					      V12sensePin, V24sensePin, USER_BUTTON); 
	if(cntrl != 0){
 	    cntrl->setCANBASE( CANaddr + (svtCNTRLsensorID << 4));
 	    cntrl->attachCAN(canbus);   // attach sensor to CAN bus
 	    cntrl->setPeriod(15);       // set update period (ms)
 	    cntrl->attachSysState(&SS);
            cntrl->attachClock(&msclock);

	    // initialize sensor and add to sensor Manager
	    if(!(err = cntrl->init())){
		smanager.addSensor(cntrl);
		udebug.printf("cntrls initialized \r\n");
	    }
	    else{  // process initialization error - report failure
		udebug.printf("cntrls did not initialize \r\n");
	    }
 	}
    }
#endif

#ifdef senBPS
    if(CANaddr == TEXSUNONECANADDR){
// create BPS sensor
	svtSensor* bps = new svtBPSsensor(BPStxPin, BPSrxPin, 0); 
	bps->setCANBASE( CANaddr + (svtBPSsensorID << 4));
	bps->attachCAN(canbus);   // attach sensor to CAN bus
	bps->setPeriod(15);       // set update period (ms)
	bps->attachSysState(&SS);
	bps->attachClock(&msclock);

// initialize sensor and add to sensor Manager
	if(!(err = bps->init())){
	    smanager.addSensor(bps);
	    udebug.printf("bps initialized \r\n");
	}
	else{  // process initialization error - report failure
	    udebug.printf("bps did not initialize \r\n");
	}
    }
#endif

#ifdef senTPMS
    if(CANaddr == SENSORONECANADDR){
// create TPMS sensor
	svtSensor* tpms = new svtTPMSsensor(PTC11, PTC10, TPMSWRITE);
	tpms->setCANBASE( CANaddr + (svtTPMSsensorID << 4));
	tpms->attachCAN(canbus);   // attach sensor to CAN bus
	tpms->setPeriod(250);       // set update period (ms)
        tpms->attachClock(&msclock);

// initialize sensor and add to sensor Manager
	if(!(err = tpms->init())){
	    smanager.addSensor(tpms);
	    udebug.printf("tpms initialized \r\n");
	}
	else{  // process initialization error - report failure
	    udebug.printf("tpms did not initialize \r\n");
	}
    }
#endif 

#ifdef senIMU
    if(CANaddr == SENSORONECANADDR){
// create IMU sensor
	svtSensor* imu = new svtIMUsensor(PTB3, PTB2, 0);
	imu->setCANBASE( CANaddr + (svtIMUsensorID << 4));
	imu->attachCAN(canbus);   // attach sensor to CAN bus
	imu->setPeriod(250);       // set update period (ms)
        imu->attachClock(&msclock);

// initialize sensor and add to sensor Manager
	if(!(err = imu->init())){
	    smanager.addSensor(imu);
	}
	else{  // process initialization error - report failure
	    udebug.printf("imu did not initialize \r\n");
	}
    }
#endif   

#ifdef senMPPT
    if(CANaddr == SENSORONECANADDR){
	// create MPPT sensor
	svtSensor* mppt = new svtMPPTsensor(SPIncsPin, SPIintPin, SPImosiPin, 
                                            SPImisoPin, SPIsckPin, SPISPEED);
	mppt->setCANBASE( CANaddr + (svtMPPTsensorID << 4));
	mppt->attachCAN(canbus);   // attach sensor to system CAN bus
	mppt->setPeriod(250);      // set update period (ms)
        mppt->attachClock(&msclock);

	// initialize sensor and add to sensor Manager
	if(!(err = mppt->init())){
	    smanager.addSensor(mppt);
	}
	else{  // process initialization error - report failure
	    udebug.printf("mppt did not initialize \r\n");
	}
    }
#endif 

#ifdef senBLINKYGREEN
    if(CANaddr == STM32ONECANADDR ){ 
	// create blinking green led 
	svtSensor* gblinking = new GreenLEDsensor();
	gblinking->setCANBASE(CANaddr + (GreenLedSensorID << 4));
	gblinking->attachCAN(canbus);   // attach sensor to CAN bus
	gblinking->setPeriod(300);       // set update period (ms)
        gblinking->attachClock(&msclock);

	// initialize sensor and add to sensor Manager
	if(!(err = gblinking->init())){
	    smanager.addSensor(gblinking);
	}
	else{  // process initialization error - report failure
	    udebug.printf("Gblink failed to initialize.\r\n");
	}
    }
#endif

#ifdef senSHUTDOWN
    if(CANaddr == SENSORONECANADDR){
	// create GPS  
	svtSensor* shutd = new svtShutdownSensor(USER_BUTTON,PullDown);
	/* note that the Nucleo User Button when configured as PullDown,
            reports 0 (false) when not pressed and 1 (true) when pressed.
	**/

	shutd->setCANBASE(CANaddr + (svtGPSsensorID << 4));
	shutd->attachCAN(canbus);   // attach sensor to CAN bus
	shutd->setPeriod(200);      // set update period (ms)
        shutd->attachClock(&msclock);

	// initialize sensor and add to sensor Manager
	if(!(err = shutd->init())){
	    smanager.addSensor(shutd);
	}
	else{  // process initialization error - report failure
	    udebug.printf("Shutdown failed to initialize.\r\n");
	}
    }
#endif

#ifdef senGPS
    if(CANaddr == SENSORONECANADDR){
	// create GPS  
	svtSensor* gps = new svtGPSsensor(GPStxPin, GPSrxPin, 0);

	gps->setCANBASE(CANaddr + (svtGPSsensorID << 4));
	gps->attachCAN(canbus);   // attach sensor to CAN bus
	gps->setPeriod(400);      // set update period (ms)
        gps->attachClock(&msclock);

	// initialize sensor and add to sensor Manager
	if(!(err = gps->init())){
	    smanager.addSensor(gps);
	}
	else{  // process initialization error - report failure
	    udebug.printf("GPS failed to initialize.\r\n");
	}
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
 
 

