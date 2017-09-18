/***********************************************************************************
 *
 *  File - svtContactor.cpp
 *
 *  Purpose - Sequences turn on /turn off of precharge and contactors
 *
 *  Author: J. C. Wiley March 2015, 
 *             from Solvehicle SV_Contactor class - May 2013
 *
 ************************************************************************************/
#include "svtContactor.h"

extern RawSerial udebug;

/**************************************************
-
- Destructor
-
*************************************************/
svtContactor::~svtContactor(){

}

/**************************************************
-
- init
-
*************************************************/
void svtContactor::init(int at1, int at2){
    delta1 = at1;
    delta2 = at2;
}

/**************************************************
-
- update
-
*************************************************/
int svtContactor::update(int at){
    t = at;

    switch(state){
    case stateOff:  // off
	break;
    case statePre:  // precharge
	if(t >= t0 + delta1 - delta2){
#ifdef CONTACTORTRACE
	    udebug.printf("svtContactor::update turn on contactor.\r\n");
#endif
	    cont.write(1); // turn on contactor
	    state = stateOver;
	}
	break;
    case stateOver: // precharge & contactor

	if(t >= t0 + delta1){
#ifdef CONTACTORTRACE
	    udebug.printf("svtContactor::update turn off precharge.\r\n");
#endif
	    pre.write(0); // turn off precharge
	    state = stateOn;
	}
	break;
    case stateOn: // contactor

	break;
    default:
	break;
    }
    return 0;
}

/**************************************************
-
- turnOn
-
*************************************************/
void svtContactor::turnOn(){
    if( state == stateOff){
#ifdef CONTACTORTRACE
	udebug.printf("svtContactor::turnOn turn on precharge.\r\n");
#endif
	t0 = t;
	pre.write(1);
	state = statePre;
    }
}

/**************************************************
-
- turnOff
-
*************************************************/
void svtContactor::turnOff(){
#ifdef CONTACTORTRACE
    udebug.printf("svtContactor::turnoff contactor and precharge.\r\n");
#endif
    cont.write(0);
    pre.write(0);
    state = stateOff;
}
