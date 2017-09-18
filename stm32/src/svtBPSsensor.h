/**

   File: svtBPSsensor.h


   Purpose - FMF communication with Dashboard (PIC) via 422 serial
             and outputs CAN message with state of switches
 
   Author - J. C. Wiley,  March. 2015

*/
#ifndef SVTBPSSENSOR_H
#define SVTBPSSENSOR_H

#include "svtSensor.h"
#include "BufferedFMF.h"
#include "SV_FMF.h"
#include <stdint.h>

#define BPSBAUD 9600
#define BPSINQUEUE 250
#define BPSOUTQUEUE 20

#undef BPSDEBUG
//#define BPSDEBUG

/** BPSsensor class.
 * Uses BufferedSerial interface  
 */
class svtBPSsensor: public svtSensor {
public:
svtBPSsensor(PinName tx, PinName rx,  int addr): _q(tx, rx, BPSINQUEUE, BPSOUTQUEUE){

	msg_cnt = 0;
	idxTV = 0;
	blkcnt = 0;
    };
    ~svtBPSsensor();

    int init();
    virtual int readSensor();
    virtual int command(CAN_Message& m);
    virtual void update(); 

private:
    uint8_t msg_cnt; // number of messages with no change
    BufferedFMF _q;
    vector<unsigned char> bpsmsg;
    int parse(uint8_t len, uint8_t cmd, uint8_t* d);
    int start_cmd();
    int stop_cmd();
    int syson_cmd();
    int sysoff_cmd();
    int reset_cmd();
    int tobedone_cmd();
    bool snapShot(uint8_t*);
    int lastsent_ms;
    int senddelta_ms;
    uint8_t idxTV;  // index for TV snapshot messages 0-31
    uint8_t blkcnt; // count blocks with overflow
    uint8_t snapTime; // last 8 bits of local snap time
    uint8_t bootstrapCount; // counter for bootstrap data CAN message output
    vector<uint16_t> Tsnap;   // cell temperatures (10ths of a degree Kelvin)
    vector<uint16_t> Vsnap;
};
#endif

