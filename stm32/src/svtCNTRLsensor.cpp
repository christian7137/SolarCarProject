/**

l   File: svtCNTRLsensor.cpp

   Purpose - Performs control functions for Contactors, Arrays, Dash, BPS
 
   Author - J. C. Wiley,  March. 2015, from SV_Controller in Solvehicle
*/

#include "svtCNTRLsensor.h"

/** init
 */
int svtCNTRLsensor::init(){

    ARRAYSWON = 0;  // set parity of array switches
    ARRAYSWOFF = 1; // when ArrayOn is 1, relay closes and turns off MPPT
    
    Vscale12 = 181; // vScale12 = (65535*2K)/(3.3*22k*10) = 180.5 to scale volts12 to 0.1 volts,
    Vscale24 = 111; // Vscale24 = (65535*2.2863/19.47*3.3)/10 to scale volts24 to 0.1 volts,
    
    // setup contactor times - intervals in ms
    arraycontactor.init(2500, 500);
    motorcontactor.init(2500, 500);
    // arraycontactor.turnOff();
    // motorcontactor.turnOff();

    if(Estop.Up())   _ss->Estop = true;
    if(Estop.Down()) _ss->Estop = false;
    
    curstate.DashIgnBPS = _ss->DashIgnBPS;	  
    curstate.DashIgnMotor = _ss->DashIgnMotor;
    curstate.BPSon = _ss->BPSon;   
    curstate.BPSmeasuring = _ss->BPSmeasuring;
    curstate.BPSbalancing = _ss->BPSbalancing; 
    curstate.BPSTrip = _ss->BPSTrip; 
    curstate.BPSMaxV = _ss->BPSMaxV;  
    curstate.BPSstatus = _ss->BPSstatus;
    curstate.Estop = _ss->Estop;    
    curstate.ArrayContactorOn = arraycontactor.IsOn(); 
    curstate.MotorContactorOn = motorcontactor.IsOn(); 
    curstate.BSChKill = BootKill.read();   
    curstate.ArrayAon = ArrayA.read(); 
    curstate.ArrayBon = ArrayB.read(); 
    curstate.ArrayCon = ArrayC.read();
    prestate.copy(curstate);

    for(int i=0; i<21;i++){
	cstatelab.push_back("");
    }
    (cstatelab[0]).append("null");
    (cstatelab[ESTOP]).append("Estop");
    (cstatelab[SHUTDOWN]).append("shutdown");
    (cstatelab[BPSTURNON]).append("bps turn on");
    (cstatelab[BPSTURNOFF]).append("bps turn off");
    (cstatelab[MOTORTURNON]).append("motor turn on");
    (cstatelab[MOTORTURNOFF]).append("motor turn off");
    (cstatelab[BPSON]).append("BPS on");
    (cstatelab[ARRAYON]).append("arrays on");
    (cstatelab[BPSTRIP]).append("BPS trip on");
    (cstatelab[BOOTSTURNON]).append("bootstrap charge on");
    (cstatelab[BOOTSTURNOFF]).append("bootstrap charge off");
    (cstatelab[ARRAYAON]).append("Array A on");
    (cstatelab[ARRAYBON]).append("Array B on");
    (cstatelab[ARRAYCON]).append("Array C on");
    (cstatelab[ARRAYAOFF]).append("Array A off");
    (cstatelab[ARRAYBOFF]).append("Array B off");
    (cstatelab[ARRAYCOFF]).append("Array C off");
    (cstatelab[ARRAYHIVOFF]).append("max V too high array off");
    (cstatelab[NONE]).append("none");

    BPSTIMEOUT = 2000;   // timeout for no messages from BPS ms
    BPSMAXVLIMIT = 35500;
    BPSMAXVLOWER = 34000;

    ArrayA.write(ARRAYSWOFF);      //   turn off array A
    ArrayB.write(ARRAYSWOFF);      //   turn off array B
    ArrayC.write(ARRAYSWOFF);      //   turn off array C

    c_state = NONE;
    return 0;
}

/** readSensor -- dummy, sends no CAN messages
 *     CAN message added 4/2017 to pass shutdown button press to RPi
 */
int svtCNTRLsensor::readSensor(){
     CANMessage msg;
     msg.id     = CANbase + 1;
     msg.len    = 8;
     msg.type   = CANData;
     msg.format = CANStandard;
     msg.data[0] =  1;
     msg.data[1] =  2;
     msg.data[2] =  3;
     msg.data[3] =  4; 
     msg.data[4] =  5;
     msg.data[5] =  6;
     msg.data[6] =  7;
     msg.data[7] =  8;
     // if(sendOutMsg(msg) != 0){
      	 udebug.printf(" mem fail in sendOutMsg 1 \r\n"); 
     }
    return 0;
}

/** command -- dummy, receives no CAN messages
 */
int svtCNTRLsensor::command(CAN_Message& m){
    return 0;
}

/** update
 */
void svtCNTRLsensor::update(){
    msclock = *msclockpt;
    
    // READ PHYSICAL PINS:
    // read digital inputs - with debounce?

   if(Estop.Up())   _ss->Estop = true;
   if(Estop.Down()) _ss->Estop = false;

    // digital output pin states
    _ss->ArrayA  = ArrayA.read();
    _ss->ArrayB  = ArrayB.read();
    _ss->ArrayC  = ArrayC.read();
    _ss->BSChKill= BootKill.read();
	
    // contactors
    _ss->MotCont = motorcontactor.IsOn();
    _ss->ArrCont = arraycontactor.IsOn();
	
    // read analog inputs  
    _ss->volts12 = V12sense.readu16Smooth()/Vscale12;    
    _ss->volts24 = V24sense.readu16Smooth()/Vscale24; 

    // build local control state from state vector
    // from dashboard
    curstate.DashIgnBPS    = _ss->DashIgnBPS;	  
    curstate.DashIgnMotor  = _ss->DashIgnMotor;
    // from BPS
    curstate.BPSon         = _ss->BPSon;   
    curstate.BPSmeasuring  = _ss->BPSmeasuring;
    curstate.BPSbalancing  = _ss->BPSbalancing; 
    curstate.BPSTrip       = _ss->BPSTrip; 
    curstate.BPSMaxV       = _ss->BPSMaxV;  
    curstate.BPSstatus     = _ss->BPSstatus;
    // local pins
    curstate.ArrayContactorOn = _ss->ArrCont; 
    curstate.MotorContactorOn = _ss->MotCont; 
    curstate.Estop    = _ss->Estop;    
    curstate.BSChKill = _ss->BSChKill;   
    curstate.ArrayAon = _ss->ArrayA; 
    curstate.ArrayBon = _ss->ArrayB; 
    curstate.ArrayCon = _ss->ArrayC;

/*--------------action triggers -----------------

  1. Estop         on       -- Emergency shutdown of systems

  2. DashShutdown  on       -- shutdown computer and subsystems

  3. DashIgnBPS    off -> on -- send BPS start command, start BPS FSM

  4. DashIgnBPS    on -> off -- send BPS stop command,
  turn off array contactor
  turn on bootstrap charger kill ( turns off charger)

  5. BPSon         off -> on -- begin array contactor sequence
  -- turn on arrays

  6. DashIgnMotor  off -> on -- begin motor contactor sequence

  7. DashIgnMotor  on -> off -- turn off motor contactor

  8. Faults: BPStrip off -> on -- turn off motor and array contactors

  9.       BSCHkill on  and BPS_bs_V_t < BPS_bs_LOW_THRESHOLD -> BSCHkill off (turns on charger)
  BSCHkill off and BPS_bs_V_t > BPS_bs_HI_THRESHOLD  -> BSCHkill on (turns off charger)

  10. Max Cell Voltage thresholds:
  Max(V_cell) > MAXCELLVOLTSMIN  -- turn off array 1
  array 1 off Max(V_cell) < (MAXCELLVOLTSMIN - MaxCellHystorisus)

  10. Warnings:

  --------------------------------------------------------*/
// next state 
    int voltagedelta = 0.3*(MAXCELLVOLTSMAX - MAXCELLVOLTSMAX);

    if(curstate.Estop != prestate.Estop){
	c_state = ESTOP;
    }
    else if(curstate.DashShutdown != prestate.DashShutdown){
	if(curstate.DashShutdown) c_state = SHUTDOWN;
    }
    else if(curstate.BPSTrip != prestate.BPSTrip){
	if(curstate.BPSTrip) c_state = BPSTRIP;
    }
    else if(curstate.DashIgnBPS != prestate.DashIgnBPS){
	if(curstate.DashIgnBPS ) c_state = BPSTURNON;
	else                     c_state = BPSTURNOFF;
    }
    else if( curstate.DashIgnMotor != prestate.DashIgnMotor){
	if(curstate.DashIgnMotor) c_state = MOTORTURNON;
	else                      c_state = MOTORTURNOFF;
    }
    else if(curstate.ArrayContactorOn && (curstate.BPSMaxV < BPSMAXVLOWER) && (!curstate.ArrayAon)){
	c_state = ARRAYON;
    }
    else if(curstate.BPSon != prestate.BPSon){
	if(curstate.BPSon) c_state = BPSON; // turn on array contactor
    }
    else if(curstate.ArrayContactorOn && !prestate.ArrayContactorOn){
	c_state = ARRAYON;            // turn on array after contactor is on
    }
    // don't overcharge bootstrap battery pack
    else if((curstate.BSChKill != prestate.BSChKill) && curstate.BSChKill && (curstate.BPS_bs_V_t < BPS_bs_LOW_THRESHOLD)){ // voltage low, turn on
	c_state = BOOTSTURNON;
    }
    else if((curstate.BSChKill != prestate.BSChKill) && !curstate.BSChKill && (curstate.BPS_bs_V_t > BPS_bs_HI_THRESHOLD)){ // voltage hi, turn off
	c_state = BOOTSTURNOFF;
    }
    // test for over max voltage main battery - turn all off
    else if((curstate.BPSMaxV > BPSMAXVLIMIT) && (curstate.ArrayAon ||curstate.ArrayBon || curstate.ArrayCon)){
	c_state = ARRAYHIVOFF;
    }
    else if((curstate.BPSMaxV > MAXCELLVOLTSMIN) && (curstate.ArrayAon)){  // too high
	c_state = ARRAYAOFF;
    }
    else if( (curstate.BPSMaxV < (MAXCELLVOLTSMIN - voltagedelta)) // can charge again
	     && !curstate.ArrayAon
	     && curstate.ArrayContactorOn){
	c_state = ARRAYAON;
    }
    else if((curstate.BPSMaxV > MAXCELLVOLTSMID) && (curstate.ArrayAon)){  // too high
	c_state = ARRAYBOFF;
    }
    else if( (curstate.BPSMaxV < (MAXCELLVOLTSMID - voltagedelta)) // can charge again
	     && !curstate.ArrayBon
	     && curstate.ArrayContactorOn){
	c_state = ARRAYBON;
    }
    else if((curstate.BPSMaxV > MAXCELLVOLTSMAX) && (curstate.ArrayCon)){ // too high
	c_state = ARRAYCOFF;
    }
    else if( (curstate.BPSMaxV < (MAXCELLVOLTSMAX - voltagedelta)) // can charge again
	     && !curstate.ArrayCon
	     && curstate.ArrayContactorOn){
	c_state = ARRAYCON;
    }
    // current state saved as prestate - for next  loop
    prestate.copy(curstate);

    // Actions
    if(c_state != NONE){
	_ss->last_c_state = c_state;
#ifdef CNTRLDEBUG
	udebug.printf("CNTRL: %d action c_state: %s, Estop %d  (0-normal)\r\n", msclock, cstatelab[c_state].c_str(), curstate.Estop);
#endif
    }
    switch(c_state){
    case ESTOP:
	motorcontactor.turnOff();
	arraycontactor.turnOff();
	ArrayA.write(ARRAYSWOFF);      // turn off array A
	ArrayB.write(ARRAYSWOFF);      // turn off array B
	ArrayC.write(ARRAYSWOFF);      // turn off array C
	c_state = NONE;
	break;
    case BPSTURNON:
	if(_ss->BPScommand == FMFCMD_BB_BPS_NONE){ // keep trying to send
#ifdef CNTRLDEBUG
	    udebug.printf("CNTRL: %d send BPS SYSON command to bps subsystem.\r\n", msclock); 
#endif
	    _ss->BPScommand = FMFCMD_BB_BPS_SYSON; // send BPS turn on BPS contactor command
	    c_state = NONE;
	}
	break;
    case BPSON:                              // BPS is on
#ifdef CNTRLDEBUG
	udebug.printf("CNTRL: %d ready to turn on array contactor. \r\n", msclock); 
#endif
	arraycontactor.turnOn();
	BootKill.write(0);
	c_state = NONE;
	break;
    case BPSTURNOFF:
	if(_ss->BPScommand == FMFCMD_BB_BPS_NONE){ // keep trying to send
	    ArrayA.write(ARRAYSWOFF);      // turn off array A
	    ArrayB.write(ARRAYSWOFF);      // turn off array B
	    ArrayC.write(ARRAYSWOFF);      // turn off array C
	    arraycontactor.turnOff();
#ifdef CNTRLDEBUG
	    udebug.printf("CNTRL: %d Arrays off \r\n", msclock); 
#endif
	    BootKill.write(1);
	    _ss->BPScommand = FMFCMD_BB_BPS_SYSOFF; // send BPS turn off command
#ifdef CNTRLDEBUG
	    udebug.printf("CNTRL: %d send BPS SYSOFF command to bps subsystem\r\n", msclock);
#endif
	    c_state = NONE;
	}
	break;
    case MOTORTURNON:
	motorcontactor.turnOn();
	c_state = NONE;
	break;
    case MOTORTURNOFF:
	motorcontactor.turnOff();
	c_state = NONE;
	break;
    case ARRAYON:
	ArrayA.write(ARRAYSWON);      // turn on array A
	ArrayB.write(ARRAYSWON);      // turn on array B
	ArrayC.write(ARRAYSWON);      // turn on array C
	c_state = NONE;
	break;
    case ARRAYAON:
	ArrayA.write(ARRAYSWON); // turn on array A
	c_state = NONE;
	break;
    case ARRAYBON:
	ArrayB.write(ARRAYSWON); // turn on array B
	c_state = NONE;
	break;
    case ARRAYCON:
	ArrayC.write(ARRAYSWON); // turn on array C
	c_state = NONE;
	break;
    case ARRAYAOFF:
	ArrayA.write(ARRAYSWOFF); // turn off array A
	c_state = NONE;
	break;
    case ARRAYBOFF:
	ArrayB.write(ARRAYSWOFF); // turn off array B
	c_state = NONE;
	break;
    case ARRAYCOFF:
	ArrayC.write(ARRAYSWOFF); // turn off array C
	c_state = NONE;
	break;
    case ARRAYHIVOFF:
	ArrayA.write(ARRAYSWOFF);      // turn off array A
	ArrayB.write(ARRAYSWOFF);      // turn off array B
	ArrayC.write(ARRAYSWOFF);      // turn off array C
	c_state = NONE;
	break;		    
    case BPSTRIP:
	motorcontactor.turnOff();     // turn off motor contactor
	arraycontactor.turnOff();     // turn off array contactor
	c_state = NONE;
	break;
    case BOOTSTURNON:
	BootKill.write(0);
	c_state = NONE;
	break;
    case BOOTSTURNOFF:
	BootKill.write(1);
	c_state = NONE;
	break;
    case SHUTDOWN:
	motorcontactor.turnOff();     //  turn off motor contactor
	arraycontactor.turnOff();     //  turn off array contactor
	ArrayA.write(ARRAYSWOFF);      //   turn off array A
	ArrayB.write(ARRAYSWOFF);      //   turn off array B
	ArrayC.write(ARRAYSWOFF);      //   turn off array C
	BootKill.write(0);    //   turn off bootstrap charger
	c_state = NONE;
#ifdef CNTRLDEBUG
	udebug.printf(" %d SV_Controller: shutdown \r\n", msclock); 
#endif
	break;
    case NONE:
	break;
    default:
	break;
    }
    // update members
    motorcontactor.update(msclock);
    arraycontactor.update(msclock);
}
