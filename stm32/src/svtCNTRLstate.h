/**

   File: svtCNTRLstate.h

   Purpose - stores control variables for Texsun svtCNTRLsensor.
 
   Author - J. C. Wiley,  April 2015

*/
#ifndef SVTCNTRLSTATE_H
#define SVTCNTRLSTATE_H
#include "mbed.h"
#include <vector>

class svtCNTRLstate{
public:
    svtCNTRLstate();
    ~svtCNTRLstate();
// Dashboard-ignition switch
    bool DashIgnBPS;	// set by svtDASHsensor BPS is enabled   
    bool DashIgnMotor;	// set by svtDASHsensor Motor is enabled
    bool DashShutdown;  // shutdown triggered 
// BPS
    bool BPSon;        // set by svtBPSsensor - status
    bool BPSmeasuring; // set by svtBPSsensor - status
    bool BPSbalancing; // set by svtBPSsensor - status
    bool BPSTrip;      // set by svtBPSsensor - status
    bool Estop;        // set by sctCNTRLsensor 
    bool ArrayContactorOn; // set by sctCNTRLsensor
    bool MotorContactorOn; // set by sctCNTRLsensor
    bool BSChKill;   // set by sctCNTRLsensor
    bool ArrayAon; // set by sctCNTRLsensor
    bool ArrayBon; // set by sctCNTRLsensor
    bool ArrayCon; // set by sctCNTRLsensor
    int16_t BPSMaxV;  //  set by svtBPSsensor 
    uint8_t BPSstatus; // [0 - 4]
    uint16_t BPS_bs_V_t; // bootstrap battery voltage
    void copy(const svtCNTRLstate& s);
};
#endif
