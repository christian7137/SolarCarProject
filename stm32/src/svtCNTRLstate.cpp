/**

   File: svtCNTRLstate.cpp

   Purpose - stores control variables for Texsun svtCNTRLsensor.
 
   Author - J. C. Wiley,  April 2015

*/

#include "svtCNTRLstate.h"

/**
  - Constructor
*/

svtCNTRLstate::svtCNTRLstate(){

}
    
/**
  - Destructor
*/
svtCNTRLstate::~svtCNTRLstate(){

}

/**
  - copy
*/
void svtCNTRLstate::copy(const svtCNTRLstate& s){
   DashIgnBPS = s.DashIgnBPS;	  
   DashIgnMotor = s.DashIgnMotor;
   DashShutdown = s.DashShutdown;
   BPSon = s.BPSon;   
   BPSmeasuring = s.BPSmeasuring;
   BPSbalancing = s.BPSbalancing; 
   BPSTrip = s.BPSTrip;
   Estop = s.Estop; 
   ArrayContactorOn = s.ArrayContactorOn; 
   MotorContactorOn = s.MotorContactorOn; 
   BSChKill = s.BSChKill;   
   ArrayAon = s.ArrayAon; 
   ArrayBon = s.ArrayBon; 
   ArrayCon = s.ArrayCon; 
   BPSMaxV = s.BPSMaxV;  
   BPSstatus = s.BPSstatus; 
   BPS_bs_V_t=s.BPS_bs_V_t;
}
