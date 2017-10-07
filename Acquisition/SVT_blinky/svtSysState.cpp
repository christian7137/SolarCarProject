/**

   File: svtSysState.cpp

   Purpose - Stores essential state of car shared between sensors.
             Does not store complete state as does Beaglebone including diagnostics, but
             subset which controls operations and must be shared between sensors. Primarly
             dashboard, BPS, and NGM.
 
   Author - J. C. Wiley,  March. 2015

*/

#include "svtSysState.h"

/** Constructor
 */
svtSysState::svtSysState(){
    // an assumed initial state.

    IgnOn = false;
    ReginOn = false;
    BPSon = false;
    BPSTrip = false;
    BPScommand = FMFCMD_BB_BPS_NONE;
    BPSmeasuring = true;
    BPSbalancing = false; 
    BPSMaxV = 30000;
    BPStemp.resize(MAXBATTERYCELLS, 0);
    BPSvolt.resize(MAXBATTERYCELLS, 0);

    // BPS_bs_T_t = 0;
    // BPS_bs_I_t = 0;
    // BPS_bs_V_t = 0;
    // BPS_bs_Y_t = 0;
    // BPS_bs_status_t = 0;

    // BPS_bs_T_b = 0;
    // BPS_bs_I_b = 0;
    // BPS_bs_V_b = 0;
    // BPS_bs_Y_b = 0;
    // BPS_bs_status_b = 0;
}

/** Destructor
 */
svtSysState::~svtSysState(){

}
