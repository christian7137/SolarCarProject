/**

   File: svtCNTRLsensor.h

   Purpose - Performs control functions for Contactors, Arrays, Dash, BPS
 
   Author - J. C. Wiley,  March. 2015, from SV_Controller in Solvehicle

*/
#ifndef SVTCNTRLSENSOR_H
#define SVTCNTRLSENSOR_H

#include "svtSensor.h"
#include "svtContactor.h"
#include <string>
#include "SV_FMF.h"
#include "svtCNTRLstate.h"
#include "svtSwitch.h"
#include "svtAnalogInFilter.h"

#define CNTRLDEBUG
#undef  CNTRLDEBUG


enum eCSTATE{ESTOP=1, SHUTDOWN=2, BPSTURNON=3, BPSTURNOFF=4, MOTORTURNON=5,
		 MOTORTURNOFF=6, BPSON=7, ARRAYON=8, BPSTRIP=9, BOOTSTURNON=10,
		 BOOTSTURNOFF=11,  NONE=12, ARRAYAON=13, ARRAYBON=14, ARRAYCON=15,
		 ARRAYAOFF=16, ARRAYBOFF=17, ARRAYCOFF=18, ARRAYHIVOFF=19};
typedef enum eCSTATE CSTATE_t;

/** svtCNTRLsensor class.
 *  
 */
class svtCNTRLsensor: public svtSensor {
public:
svtCNTRLsensor(PinName EstopPin, PinName BootKillPin, PinName MotorPrechargePin,
		   PinName MotorContactorPin, PinName ArrayPrechargePin, 
		   PinName ArrayContactorPin, PinName ArrayAPin,
		   PinName ArrayBPin, PinName ArrayCPin, PinName V12sensePin,
	           PinName V24sensePin,
                   PinName ShutdownPin):
        Estop(EstopPin),
        BootKill(BootKillPin),
        motorcontactor(MotorPrechargePin, MotorContactorPin), 
        arraycontactor(ArrayPrechargePin, ArrayContactorPin), 
        ArrayA(ArrayAPin), ArrayB(ArrayBPin), ArrayC(ArrayCPin),
	V12sense(V12sensePin), V24sense(V24sensePin),
	UserButton(ShutdownPin){
        // can be modified over CAN bus in commands, if routine added.
	MAXCELLVOLTSMAX   = 35500;  // Max cell voltage (mV) at which to shutdown all 3 power trackers
	MAXCELLVOLTSMID   = 35250; // Max cell voltage (mV) at which to shutdown 2 power trackers
	MAXCELLVOLTSMIN   = 35000;  // Max cell voltage (mV) at which to shutdown 1 power tracker
	BPS_bs_LOW_THRESHOLD = 13500; // in mV  nominal voltage is 14 V or 14000 mV.
	BPS_bs_HI_THRESHOLD  = 14000; // bootstrap battery high threshold
        BPSTIMEOUT = 2000;   // timeout for no messages from BPS (ms)	
			 };
~svtCNTRLsensor();

int init();
virtual int readSensor();
virtual int command(CAN_Message& m);
virtual void update();

private:
int msclock; // time in ms at start of the update
svtCNTRLstate curstate, prestate;
svtSwitch  Estop;

DigitalOut BootKill;
svtContactor motorcontactor;
svtContactor arraycontactor;
DigitalOut ArrayA;
DigitalOut ArrayB;
DigitalOut ArrayC;
svtAnalogInFilter V12sense;
svtAnalogInFilter V24sense;
float Vscale24;
float Vscale12;
int ARRAYSWON;
int ARRAYSWOFF;

// voltage limit settings
uint16_t MAXCELLVOLTSMAX;
uint16_t MAXCELLVOLTSMID;
uint16_t MAXCELLVOLTSMIN;
uint16_t BPSMAXVLOWER;
uint16_t BPSMAXVLIMIT;
uint16_t BPSTIMEOUT;
uint16_t BPS_bs_LOW_THRESHOLD;
uint16_t BPS_bs_HI_THRESHOLD;

// labels for debug messages
vector<string> cstatelab;

// FSM state variable
CSTATE_t c_state;

};
#endif
