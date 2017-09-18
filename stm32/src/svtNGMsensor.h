/*
 - File: svtNGMsensor.h

 - Purpose: Interface to NGM motor controller

 - Author: J. C. Wiley, March 2014
 derived from PIC24 code

 *     Notes June 2014
 *
 * Notes on setting torque control p. 49 of NGM manual
 * disable the controller                             00B!     // disable controller
 * enable writes to the configuration page (page 3)   008.3=1  // allow pg 3 to be written
 * set BIT_defaultspdctrl (VC_discrete.13)            302.D=0  // clear for torque control
 * save the configuration page                        0F4!     // store in nonvolital memory
 * disable writes to all pages                        008=0    // protect all pages
 * immediately switch to torque control               00C!     // set torque control
 * enable the controller                              00A!     // enable controller

 *   "00B!","008.3=1","302.D=0","0F4!","008=0","00C!","00A!"  // torque seq
 *   "00B!","008.3=1","302.D=1","0F4!","008=0","00D!","00A!"  // speed seq

 */
#ifndef SVTNGMSENSOR_H 
#define SVTNGMSENSOR_H

#include "svtSensor.h"
#include "BufferedNGM.h"
#include "SV_FMF.h"
#include <stdint.h>
#include <string>

extern RawSerial udebug;

#define NGMBAUD 19200
#define NGMINQUEUE 80
#define NGMOUTQUEUE 80

typedef struct PAGE0 {
	int16_t SI_desiredphaseI;
	int16_t SI_desiredspd;
	int16_t SI_phIramp;
	int16_t SI_spdramp;
	int16_t SI_thrphaseIlimit;
	int16_t SI_rgnphaseIlimit;
	int16_t SI_dischargeIlimit;
	int16_t SI_chargeIlimit;
	int16_t SI_writeenable;
} page0_t;

// Page 1 - Instrument registers (19) 16 bits. signed
typedef struct PAGE1 {
	int16_t AM_velocity;
	int16_t AM_supplyV;
	int16_t AM_supplyI;
	int16_t AM_baseplateT;
	int16_t AM_ambientT;
	int16_t AM_motorT;
	int16_t AM_SOC;
	int16_t AM_thr;
	int16_t AM_rgn;
	int16_t SV_desiredphaseI;
	int16_t SV_desiredspd;
	int16_t SV_targetphaseI;
	int16_t SV_drivestate;
	int16_t SV_fault1latch;
	int16_t SV_fault1;
	int16_t SV_fault2;
	int16_t SV_fault3;
	int16_t SV_thrlimit;
	int16_t SV_rgnlimit;
} page1_t;


// Page 3 - Vehicle Configuration registers (64) 16 bits. various formats
typedef struct PAGE3 {
    int16_t VC_SCsupplyI;
    int16_t VC_OFsupplyI;
    int16_t VC_discrete;
    int16_t VC_invert;
} page3_t;


enum eNGMSTATE{START, WAITECHO, SENDQUERY, WAITRESPONSE, REPLY, SENDMESG,
               SENDERROR, SENDDUMP, SENDOVERFLOW};

typedef enum eNGMSTATE ngmstate_t;

/** NGMsensor class.
 * Uses BufferedSerial interface 
 */
class svtNGMsensor: public svtSensor {
public:
	svtNGMsensor(PinName tx, PinName rx, int addr, PinName ThInPin,
		     PinName RgInPin, PinName ThrEnPin, PinName FwdRevPin) :
			_q(tx, rx, NGMINQUEUE, NGMOUTQUEUE), ThrIn(ThInPin), RgnIn(RgInPin), ThrEnIn(
			    ThrEnPin), FwdRevOut(FwdRevPin) {
		// query sequence
		query.push_back("00A!");  // keep alive
		query.push_back("100?");
		query.push_back("102?");
		query.push_back("0**?");
		query.push_back("00A!"); // keep alive
		query.push_back("100?");
		query.push_back("102?");
		query.push_back("1**?");
		query.push_back("00A!"); // keep alive
		query.push_back("100?");
		query.push_back("102?");
		query.push_back("C**?");
		query.push_back("00A!"); // keep alive
		query.push_back("00B!"); // set torque control - only send once
		query.push_back("008.3=1");
		query.push_back("302.D=0");
		query.push_back("0F4!");
		query.push_back("008=0");
		query.push_back("00C!");
		query.push_back("302?");   // dump page 3 at end and only dump once.
		query.push_back("00A!");
		querylength = query.size();
		for (int i = 0; i < querylength; i++){
			querydelta.push_back(150); // time to wait for response
	    }
	};

	~svtNGMsensor();

	int init();
	virtual int readSensor();
	virtual int command(CAN_Message& m);
	virtual void update();

private:
    BufferedNGM _q;
    bool query000B;
    bool query000C;
    bool haveResponse;
    uint8_t page;
    uint16_t nexttime;
    uint16_t querywatch;
    uint16_t lastclock;
    unsigned int nmsg;
    ngmstate_t nstate;
    unsigned  int q;
    unsigned  int count;
    void NGM_translate(int n, const char* d, int16_t *f);
    void parse(string& query, string& s);
    vector<string> query;
    vector<uint8_t> querydelta;
    int querylength; // number of queries to cycle through
    int dutycycle; // reduce number of CAN messages sent

    // motor variables
    page0_t page0;
    page1_t page1;
    page3_t page3;

    // car variables
    uint16_t Thr;
    uint16_t Rgn;
    uint16_t ThrLast;
    uint16_t RgnLast;
    bool ThrEnLast;
    bool ThrEn;
    bool msg1_dirty;
    bool msg2_dirty;
    bool msg3_dirty;
    bool msg4_dirty;
    string rawmsg;
    uint8_t error_code;
    AnalogIn ThrIn;    // J4-4 - P5-8
    AnalogIn RgnIn;    // J4-2 - P5-10
    DigitalIn ThrEnIn;  //J1-10 - P5-6
    DigitalOut FwdRevOut; // J1-12 -CAN
    void Smooth(uint16_t& v, uint16_t x, int n);
    // parse array
    int16_t f[19];
};
#endif

