/**

   File: svtSysState.h

   Purpose - Stores essential state of car shared between sensors.
             Does not store complete state as does Beaglebone including diagnostics, but
             subset which controls operations and must be shared between sensors. Primarly
             dashboard, BPS, and NGM data.
 
   Author - J. C. Wiley,  March. 2015

*/
#ifndef SVTSYSSTATE_H
#define SVTSYSSTATE_H
#include "mbed.h"
#include <vector>
#include "SV_FMF.h"

#define MAXBATTERYCELLS 37 // maximum cells - this should be determined dynamically (BPS/WWW) - later.

static const uint8_t BPSbalancingmask = 0b10000000;
static const uint8_t BPSstatusmask    = 0b00000111;

class svtSysState{
public:
    svtSysState();
    ~svtSysState();
// Dashboard
    uint8_t CC_state;   // byte for cruise control
    bool DashCCon;	    //cruise on
    bool DashCCsign;	//sign of increment
    bool DashCCincr;	//increment if true
    bool DashCCrsm;	    //resume set speed
    bool DashCCbrk;	    //Brake pedal pressed
    bool DashIgnBPS;	// BPS is enabled
    bool DashIgnMotor;	// Motor is enabled
    bool DashShutdown;	// System needs to gracefully shut down
    bool DashForward;	// forward (true)/reverse (false)
    bool DashBlink;     // are blinkers on
    unsigned int  DashGapReq; // requested gap in percent [0-99]
    uint8_t DashScreenOne;    // current screen displayed, controlled by dashboard -- display uses this value
    uint8_t DashScreenTwo;    // current screen displayed, controlled by dashboard -- display uses this value

    int  DashScrnMax;   // maximum number of screens (for roll over), set by display

// PIC24B
    uint8_t BbPIC24BMsg; // command to motor controller

//BMS
    uint8_t BPScommand;  // controller msg to BPS
    int16_t BPScurrent;  // 100ths of Amp, signed.   (cA)
    uint8_t BPSsoc;	     // soc = State Of Charge [0 -100] unsigned
    uint8_t BPSstatus; // [0 - 4]
    bool BPSon;        // contactors on?
    bool BPSmeasuring; // measuring?
    bool BPSbalancing; // balancing?
    bool BPSTrip;      // tripped?

    uint8_t BPStime;   // last measurement time (watch dog)
    vector<uint16_t> BPStemp;   // cell temperatures (10ths of a degree Kelvin)
    vector<uint16_t> BPSvolt;   // cell voltages (10ths of mV)
    uint16_t BPSMaxV;           // maximum cell voltage seen.
    uint16_t BPSMinV;           // minimum cell voltage
    uint16_t BPSMaxT;           // maximum cell temperature
    uint16_t BPSMinT;           // minimum cell temperature
    uint8_t BPSMaxVindex;           // maximum cell voltage seen.
    uint8_t BPSMinVindex;           // minimum cell voltage
    uint8_t BPSMaxTindex;           // maximum cell temperature
    uint8_t BPSMinTindex;           // minimum cell temperature

// BPS bootstrap system
    uint16_t BPS_bs_T_t;      // bootstrap top temp (100th kelvin) unsigned
    int16_t  BPS_bs_I_t;      // bootstrap top I    (100th A)      signed
    uint16_t BPS_bs_V_t;      // bootstrap top V         (mV)      unsigned
    uint16_t BPS_bs_Y_t;      // bootstrap top V_supply  (mV)      unsigned
    uint8_t  BPS_bs_status_t; // bootstrap top status -- byte code of leds

    uint16_t BPS_bs_T_b;      // bootstrap bottom temp (100th kelvin) unsigned
    int16_t  BPS_bs_I_b;      // bootstrap bottom I    (100th A)      signed
    uint16_t BPS_bs_V_b;      // bootstrap bottom V        (mV)       unsigned
    uint16_t BPS_bs_Y_b;      // bootstrap bottom V_supply (mV)       unsigned
    uint8_t  BPS_bs_status_b; // bootstrap bottom status -- byte code of leds
// bps deadman time
    uint32_t LastBPSMeasurement;
// motor control messages
    bool IgnOn;
    bool BrkOn;
    bool ReginOn;
// TPMS
	uint8_t RFPress;  // right front  psi
	uint8_t RFTemp;   // temperature index
	uint8_t LFPress;  // left front
	uint8_t LFTemp;   // temperature index
	uint8_t RTPress;   // rear
	uint8_t RTTemp;   // temperature index
// MotorControllerBoard
        uint16_t gappos;   // motor gap position 
// BbController (now motor controller board);
  	bool Thren;     // 8_45
// Controller signals:
//   analog values
       uint16_t volts24;  // J4-4, voltage of the 24 V power bus (mV)
       uint16_t volts12;  // J4-2, voltage of the 12 V power bus (mV)
//  digital values
  	bool MotCont;   // J1-12;
  	bool MotPreC;   // J1-14;
  	bool ArrCont;   // J1-8;
  	bool ArrPreC;   // J1-10;
  	bool ArrayA;    // J2-12
  	bool ArrayB;    // J2-18
  	bool ArrayC;    // J2-20
  	bool Estop;     // J4-6
	bool BSChKill;  // J4-8
// track last (not NONE) CNTRL c_state action state
	uint8_t last_c_state;
};

#endif
