/*
  - File: svtNGMsensor.cpp

  - Purpose: Interface to NGM motor controller

  - Author: J. C. Wiley, March 2014
  derived from PIC24 code

*/
#include "svtNGMsensor.h"
#include <stdlib.h>
#include <cerrno>

#define NGMDEBUG
#undef  NGMDEBUG

// circular queue parameters for NGM
#define CMDMAX  80

#define HTAB 0x09
#define CR 0x0D
#define NL 0x0A
#define Mask 0xFD00
extern RawSerial udebug;

/**
   Constructor -- in header
*/

/**
   destructor
*/
svtNGMsensor::~svtNGMsensor(){ 

};

/** init - sets up serial queue.
 *   -
 */
int svtNGMsensor::init(){    
// initialize input queue
    _q.setTerminalQ(CR);
    _q.baud(NGMBAUD);
    dutycycle = 10;
    q = 0;
    nstate = START;
    count = 0;
    nexttime  = querydelta[0];
    querywatch = nexttime + (querydelta[0] + (querydelta[0]>>1)); // watch dog timeout interval
    query000C = false; // not seen yet
    query000B = false; // not seen yet
    msg1_dirty = true;
    msg2_dirty = true;
    msg3_dirty = true;
    msg4_dirty = true;
    haveResponse = false;
    return 0;
}

/** Smooths values, shift is power of 2 to divide change
 *  @param v - current value
 *  @param r - reading
 *  @param n - divide change by 2^n. (2, 3, 4);
 */
void svtNGMsensor::Smooth(uint16_t& v, uint16_t x, int n){
    uint16_t del;   
    if(x > v){
        del = x - v;
	v += (del >> n);
    }
    else{
        del = v - x;
	v -= (del >> n); 
    }
}

/**  update -- process 
 *        Queries NGM controller and checks pedal values.
 */
void svtNGMsensor::update(){  
    uint16_t msloop;
    uint8_t cmd[CMDMAX];  // valid commands are cmdmax characters or less 

    // check pedal values
    Smooth(Thr,  ThrIn.read_u16(), 4);
    Smooth(Rgn,  RgnIn.read_u16(), 4);
    ThrEn = ThrEnIn.read();
    if( ((Thr & Mask) != (ThrLast & Mask)) || ( (Rgn & Mask) != (RgnLast & Mask)) || (ThrEn != ThrEnLast)){
	msg3_dirty = true;
	ThrLast = Thr;
	RgnLast = Rgn;
	ThrEnLast = ThrEn;
	// udebug.printf("thr %d rgn %d En %d\r\n", Thr, Rgn, ThrEn);
    }

    // check serial queue for new message (string ending in CR/LF)
    _q.scanQ();  // conditions queue 
    if(_q.hasMsg() && (rawmsg.length() == 0) ){   // extract next message from queue       
	if( (nmsg = _q.extractMsg(&cmd[0], CMDMAX)) > 0){
	    if(nmsg > 0){                 
		for(unsigned int i=0; i<nmsg; i++){
		    rawmsg.push_back(cmd[i]);
		}
		haveResponse = true;
#ifdef NGMDEBUG 
		udebug.printf("rawmsg: %s\r\n", cmd);
#endif
	    }
	}
    } 

// FSM -- interacts with motor controller
    msloop = *msclockpt;
#ifdef NGMDEBUG
    udebug.printf("state %d %d\r\n", nstate, msloop);
#endif
    msloop = *msclockpt;
    switch(nstate){
    case START:  //clear buffer and reset timers
#ifdef NGMDEBUG
	// udebug.printf("start %d\r\n", msloop);
#endif
	if( (msloop > nexttime) || (count > querywatch)){
	    count = 0;  // timeout count
	    lastclock = msloop;
	    if((q == 0) && query000B && query000C) querylength = 12; // truncate after torque set once.
	    nexttime = lastclock + querydelta[q];
	    if(nexttime < msloop) nexttime = querydelta[q];
	    querywatch = querydelta[q] + (querydelta[q]>>2); // timeout watch
            _q.clearQ();
            haveResponse = false;
            rawmsg.clear();  
	    nstate = SENDQUERY; // ready for next query
	}
	break;
    case SENDQUERY:
#ifdef NGMDEBUG
	//   udebug.printf("send query %s\r\n", query[q].c_str());
#endif
	if( _q.putNGM(query[q]) == 0){ // if zero return, sent.
	    nstate = WAITECHO;
	}
	break;
    case WAITECHO: // wait for command echo and response
#ifdef NGMDEBUG
	//   udebug.printf("wait echo %s\r\n", rawmsg.c_str());
#endif
	if(haveResponse){            
	    if( query[q].compare(0, 4, rawmsg) == 0){ // echo correct
		nstate = WAITRESPONSE;
                rawmsg.clear();  
		if(q == 13){
		    query000B = true; // see B first
		}
		if(query000B && (q == 18)){  // see C second
		    query000C = true;
		}
	    }
	    else{    // error
		nstate = SENDERROR;
	    }
	    if( rawmsg.length() == CMDMAX ){
		nstate = SENDOVERFLOW;
	    }
	}
	break;
    case SENDOVERFLOW:
	error_code = 1;
	msg3_dirty = true;
	nstate = SENDMESG;
	break;
    case SENDERROR:
	error_code = 2;
	msg3_dirty = true;
	nstate = SENDMESG;
	break;
    case WAITRESPONSE: // have valid echo, wait for next sentence
        if(haveResponse){
	    if(rawmsg.length() < CMDMAX){
		nstate = SENDMESG; // will send CAN mesg(s) if any dirty
#ifdef NGMDEBUG
                udebug.printf("wait response %s\r\n", rawmsg.c_str());
#endif
		parse(query[q], rawmsg); // query and response
	    }
	    else{
		nstate = SENDOVERFLOW;
	    }
	}
	break;
    case SENDMESG:
#ifdef NGMDEBUG
        udebug.printf("send CAN\r\n");
#endif
        q = (q + 1) % querylength; // go to next query
	nstate = START;
	break;
    case SENDDUMP: // send rawmsg to debug port
	udebug.printf("dump: %s\r\n", rawmsg.c_str());
	nstate = START; 
	break;
    default:
	break;
    } // end FSM
// watchdog timer, if trapped too long, start over by resetting state
    if(msloop != lastclock){ // increment only if 1 ms passed
	count +=  msloop - lastclock;
	lastclock = msloop;
    }
    if(count > querywatch){  // time out limit, count in ms.
#ifdef NGMDEBUG
        udebug.printf("time out %d %d \r\n", count, querywatch);
#endif
	nstate = START;
    }
    // if any CAN msgs ready, send.
    readSensor();
}

/** readSensor - overrides virtual base class function
 *  data for CAN msgs in just arrived in message
 *  puts current dashboard state in CAN output queue.
 */
int svtNGMsensor::readSensor(){
    CAN_Message msg;

    if(dutycycle > 0){
	dutycycle--;
    }
    else{  // actually send data
	dutycycle = 10; // reload for next case
	if(msg1_dirty){   
	    msg.id     = CANbase + 1;
	    msg.len    = 8;
	    msg.type   = CANData;
	    msg.format  = SVTCANFORMAT;
	    msg.data[0] =   page1.AM_supplyV       & 0x00FF;
	    msg.data[1] =  (page1.AM_supplyV >> 8) & 0x00FF;
	    msg.data[2] =   page1.AM_supplyI       & 0x00FF;
	    msg.data[3] =  (page1.AM_supplyI >> 8) & 0x00FF; 
	    msg.data[4] =   page1.AM_thr           & 0x00FF;
	    msg.data[5] =  (page1.AM_thr >> 8)     & 0x00FF;
	    msg.data[6] =   page1.AM_velocity           & 0x00FF;
	    msg.data[7] =  (page1.AM_velocity >> 8)     & 0x00FF;
	    if(sendOutMsg(msg) != 0){
		udebug.printf("svtNGMsensor: mem fail in sendOutMsg 1 \r\n"); 
	    }
	    msg1_dirty = false;
	}

	if(msg2_dirty){
	    msg.id     = CANbase + 2;
	    msg.len    = 8;
	    msg.type   = CANData;
	    msg.format = SVTCANFORMAT;
	    msg.data[0] =   page1.AM_baseplateT       & 0x00FF;
	    msg.data[1] =  (page1.AM_baseplateT >> 8) & 0x00FF;
	    msg.data[2] =   page1.AM_ambientT         & 0x00FF;
	    msg.data[3] =  (page1.AM_ambientT >> 8)   & 0x00FF; 
	    msg.data[4] =   page1.AM_motorT           & 0x00FF;
	    msg.data[5] =  (page1.AM_motorT >> 8)     & 0x00FF;
	    msg.data[6] =  page1.SV_fault1 & 0x00FF;  // needs compression
	    msg.data[7] =  page1.SV_fault2 & 0x00FF;  // needs compression
	
	    if(sendOutMsg(msg) != 0){
		udebug.printf("svtNGMsensor: mem fail in sendOutMsg 1 \r\n"); 
	    }
	    msg2_dirty = false;
	}

	if(msg3_dirty){      
	    msg.id     = CANbase + 3;
	    msg.len    = 8;
	    msg.type   = CANData;
	    msg.format = SVTCANFORMAT;
	    msg.data[0] =  error_code;
	    msg.data[1] =   ThrEn     & 0x00FF;
	    msg.data[2] =   Thr       & 0x00FF;
	    msg.data[3] =  (Thr >> 8) & 0x00FF;
	    msg.data[4] =   Rgn       & 0x00FF;
	    msg.data[5] =  (Rgn >> 8) & 0x00FF; 
	    msg.data[6] =   page1.AM_rgn              & 0x00FF;
	    msg.data[7] =  (page1.AM_rgn   >>8)       & 0x00FF;
	    if(sendOutMsg(msg) != 0){
		udebug.printf("svtNGMsensor: mem fail in sendOutMsg 1 \r\n"); 
	    }
	    msg3_dirty = false;
	}  

	if(msg4_dirty){
	    msg.id     = CANbase + 4;
	    msg.len    = 8;
	    msg.type   = CANData;
	    msg.format = SVTCANFORMAT;
	    msg.data[0] =  page1.SV_drivestate      & 0x00FF;
	    msg.data[1] = (page1.SV_drivestate >> 8)& 0x00FF;
	    msg.data[2] =  page1.SV_desiredphaseI          & 0x00FF;
	    msg.data[3] = (page1.SV_desiredphaseI >> 8)    & 0x00FF; 
	    msg.data[4] =  page1.SV_desiredspd        & 0x00FF;
	    msg.data[5] = (page1.SV_desiredspd >> 8)  & 0x00FF;
	    msg.data[6] =  page1.SV_targetphaseI       & 0x00FF;
	    msg.data[7] = (page1.SV_targetphaseI >> 8) & 0x00FF;
	    if(sendOutMsg(msg) != 0){
		udebug.printf("svtNGMsensor: mem fail in sendOutMsg 1 \r\n"); 
	    }
	    msg4_dirty = false;
	}
    }
    return 0;
}

/** command - responds to Beaglebone commands.
 * @param m - CAN message from Beaglebone. First byte is command.
 */
int svtNGMsensor::command(CAN_Message& m){
    switch(m.id & 0x7FF){
    case 0x351:  // dashboard switch settings and gap request 
        _ss->DashForward = m.data[1] & 0x04; // select forward/rev bit
	FwdRevOut.write(_ss->DashForward);
	break;
    default:
	break;
    }
    return 0;
}

/***************************************************************
  -
  - NGM_translate - convert ascii string tab delimited to ints
  -
  - d - character string with element separated by tabs
  -     each element should be represented by signed 16 bit integer
  -  6*19+19 =  114+19 = 135 max length of d
**************************************************************/
void svtNGMsensor::NGM_translate(int n, const char* d, int16_t* f){
    char *endptr;
    int front = 0;
    int k;
    for(k=0; k<n; k++){
        int i = front;
        long x;
        while( (d[i] == HTAB) && (i-front < 8)) i++;
        errno = 0;
        x = strtol(&d[front], &endptr, 10);
        if(errno == 0){
#ifdef NGMDEBUG
	    udebug.printf("x %s\r\n", d);
#endif
	    f[k] = x;
        }
        front = i+1;
    }
}

/***************************************************************
  -
  - parse  - converts NGM page data to local values and marks
  -     messages to be sent.
  -   q is query
  -   s is NGM response string.
  -
**************************************************************/
void svtNGMsensor::parse(string& q, string& s){

    if(q.compare(0, 4, "00A!") ==0 ){ // keep alive - no state change
	return;	    
    }
    else if (q.compare(0, 4,"100?") == 0){ // AM_velocity
	NGM_translate(1, s.c_str(), f);

	if( f[0] != page1.AM_velocity){
	    page1.AM_velocity    = f[0];
	    msg1_dirty = true;
	}
	return;
    }
    else if(q.compare(0, 4,"102?") == 0){ // AM_supplyI
	NGM_translate(1, s.c_str(), f);
	if( f[0] != page1.AM_supplyI){
	    page1.AM_supplyI     = f[0];
	    msg1_dirty = true;
	}
	return;
    }
    else if(q.compare(0, 4,"302?") == 0){ // VC_discrete
	NGM_translate(1, s.c_str(), f);
	if( f[0] != page3.VC_discrete){
	    page3.VC_discrete     = f[0];
	}
	return;
    }
    else if(q.compare(0, 4,"1**?") == 0){
	NGM_translate(19, s.c_str(), f);
	// Test for changes and update local state as necessary.
	// Local state needs to follow NGM controller for cruise control, however
	// data logging is slower - so only send CAN message at proscribed rate.
    
// msg 1
	if( f[0] != page1.AM_velocity){
	    page1.AM_velocity    = f[0];
	    msg1_dirty = true;
	}
	if( f[1] != page1.AM_supplyV){
	    page1.AM_supplyV     = f[1];
	    msg1_dirty = true;
	}
	if( f[2] != page1.AM_supplyI){
	    page1.AM_supplyI     = f[2];
	    msg1_dirty = true;
	}
	if( f[7] != page1.AM_thr){
	    page1.AM_thr         = f[7];
	    msg1_dirty = true;
	}

// msg 2
	if( f[3] != page1.AM_baseplateT){
	    page1.AM_baseplateT  = f[3];
	    msg2_dirty = true;
	}    
	if( f[4] != page1.AM_ambientT){
	    page1.AM_ambientT    = f[4];
	    msg2_dirty = true;
	}
	if( f[5] != page1.AM_motorT){
	    page1.AM_motorT      = f[5];
	    msg2_dirty = true;
	}
	if( f[14] != page1.SV_fault1){
	    page1.SV_fault1      = f[14];
	    msg2_dirty = true;
	}
	if( f[15] != page1.SV_fault2){
	    page1.SV_fault2      = f[15];
	    msg2_dirty = true;
	}

// msg 3	
	if( f[8] != page1.AM_rgn){
	    page1.AM_rgn         = f[8];
	    msg3_dirty = true;
	}

// msg 4
	if( f[9] != page1.SV_desiredphaseI){
	    page1.SV_desiredphaseI  = f[9];
	    msg4_dirty = true;
	}
	if( f[10] != page1.SV_desiredspd){
	    page1.SV_desiredspd     = f[10];
	    msg4_dirty = true;
	}
	if( f[11] != page1.SV_targetphaseI){
	    page1.SV_targetphaseI   = f[11];
	    msg4_dirty = true;
	}
	if( f[12] != page1.SV_drivestate){
	    page1.SV_drivestate     = f[12];
	    msg4_dirty = true;
	}


	return;
    }
    
    // convert ASCII message from NGM into 16 bit integers

// not used
/*    if( f[6] != page1.AM_SOC){
      page1.AM_SOC         = f[6];
      msg1_dirty = true;
      }

      if( f[13] != page1.AM_fault1latch){
      page1.SV_fault1latch = f[13];
      msg1_dirty = true;
      }
      if( f[16] != page1.AM_fault3){
      page1.SV_fault3     = f[16] ;
      msg1_dirty = true;
      }
      if( f[17] != page1.AM_thrlimit){
      page1.SV_thrlimit   = f[17];
      msg1_dirty = true;
      }
      if( f[18] != page1.AM_rgnlimit){
      page1.SV_rgnlimit   = f[18];
      msg1_dirty = true;
      }
*/

}


