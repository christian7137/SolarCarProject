/**

   File: svtDASHsensor.h


   Purpose - FMF communication with Dashboard (PIC) via 422 serial
             and outputs CAN message with state of switches
 
   Author - J. C. Wiley,  March. 2015

*/
#ifndef SVTDASHSENSOR_H
#define SVTDASHSENSOR_H

#include "svtSensor.h"
#include "BufferedFMF.h"
#include "SV_dashboard_msg.h"
#include "SV_FMF.h"
#include <stdint.h>

#undef DASHDEBUG
#define DASHDEBUG 

#define DASHBAUD 115200

// circular queue parameters for dashboard
#define DASHINQUEUE  80
#define DASHOUTQUEUE 80
#define CMDMAX       80

/** DASHsensor class.
 * Uses BufferedSerial interface  
 */
class svtDASHsensor: public svtSensor {
public:
svtDASHsensor(PinName tx, PinName rx,  int addr): _q(tx, rx, DASHINQUEUE, DASHOUTQUEUE){
	button_state = 0;
	cntrls_state = 0;
	msg_cnt = 0;
        displayDirty = true;
	msgcounter = 0; // counts sent CAN messages
	firsttime = true;
    };
    ~svtDASHsensor();

    int init();
    virtual int readSensor();
    virtual int command(CAN_Message& m);
    virtual void update();

private:
    uint8_t button_state;
    uint8_t button_msg;
    uint8_t button_history[8];
    uint8_t cntrls_state;
    uint8_t cntrls_msg;
    uint8_t cntrls_history[8];
    uint8_t msg_cnt; // number of messages with no change
    uint8_t active_screen, prev_screen;
    bool displayDirty;  // set if something changes display data
    BufferedFMF _q;
    void debounce(uint8_t& msg, uint8_t* history, uint8_t& state); 
    int led_cmd();
    int display_cmd();
    int start_cmd();
    int beep_cmd(int frequency, int duration);
    int ui_interact(uint8_t event); 
    uint32_t sendinterval_beep;	// clicks
    int parse();
    vector<unsigned char> ledmsg;
    vector<unsigned char> beepmsg;
    vector<unsigned char> displaymsg;
    int msgcounter;
    bool firsttime;
    uint8_t displaycount;
};
#endif

