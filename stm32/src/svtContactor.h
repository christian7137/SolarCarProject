/***********************************************************************************
 *
 *  File - svtContactor.h
 *
 *  Purpose - Sequences turn on /turn off of precharge and contactors
 *
 *  Author: J. C. Wiley March 2015, 
 *             from Solvehicle SV_Contactor class - May 2013
 *
************************************************************************************/
#ifndef svtCONTACTOR_H_
#define svtCONTACTOR_H_
#include "mbed.h"

#undef  CONTACTORTRACE
#define CONTACTORTRACE

enum eSTATE{stateOn, stateOff, statePre, stateOver};
typedef enum eSTATE state_t;

class svtContactor {
public:
    svtContactor(PinName apre, PinName acont):pre(apre), cont(acont){
	state = stateOff;
    }
    ~svtContactor();
    void init(int t1, int t2);
    int update(int t);
    void turnOn();
    void turnOff();
    enum eSTATE{stateOn, stateOff, statePre, stateOver};
    typedef enum eSTATE state_t;
    bool IsOn(){ return state == stateOn;}
private:
    state_t state; // {0-A, 1-B, 2-C, 3-D}
    int delta1; // time interval in utime units for precharge to be on
    int delta2; // time interval in utime units before end of precharge to turn on contactor
    int t0; // turn on time (utime)
    int t;  // current time (utime)
    DigitalOut pre;
    DigitalOut cont;
};
#endif
