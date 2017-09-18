/**

   File: svtDASHsensor.cpp

   Purpose - FMF communication with Dashboard (PIC) via 422 serial
   and outputs CAN message with state of switches
 
   Author - J. C. Wiley,  March. 2015
   All dashboard information from Benton Greens SV_Dashboard code for Beaglebone
and dash routines from SV_Dashboard.cpp

   Dashboard description:
   The dashboard has:
   8 push buttons:
   Cruise  0x01
   Home    0x02
   Back    0x04
   Select  0x08
   Right   0x10
   Left    0x20
   Down    0x40
   Up      0x80
   each state change coded into one bit of the button byte.

   7 LEDs plus break lights
   LV          0x01  low voltage - on with power
   BPS-green   0x02
   BPS-red     0x04
   PreCharg    0x08
   Motor-red   0x10
   Motor-green 0x20
   Reverse     0x40
   Break       0x80 break light not on dash.

   switches:
   four position ignition switch: 
   0x00, state 1
   0x01, state 2
   0x02, state 3
   0x03  state 4
   forward-reverse switch          0x04 - reverse 0n
   break switch on pedal           0x08 - break on
   left-right blinker switch
   switch positions coded into control byte

  controls message codes from dash to Bb lowest two bits are switch state
   upper three bits not used
<1:0>
  0b00,  0 - boot position
  0b01   1 - lv  low voltage (24V) position
  0b10   2 - BPS position
  0b11   3 - Motor position
<2>
    direction: 1 - reverse, 0 - forward
<3>
    brakes:    1 - pressed, 0 - not pressed
<4>
    blinker:
<5-7>
      - not used
*/
#include "svtDASHsensor.h"
#include "SV_FMF.h"

// FMF communication message tokens
#define cSTART  0x01
#define cEND    0xFE

// Button debounce
#define DEBOUNCE_MASK 0b00000111 

// cruise control
#define ENGINEON     0b00000001
#define BREAKON      0b00000010
#define DIRECTION    0b00000100
#define CCRESUM      0b00001000
#define CRUISEON     0b00010000
#define CCCHGSPEED   0b00100000
#define CCINCREASE   0b01000000
#define BBTHREN      0b10000000 

// display
#define HOMESCREEN 0
#define CRUISESCREEN 4
#define GAPSCREEN 1
#define MAXSCREENS 6

extern RawSerial udebug;
extern DigitalOut _ledB;

/**
   Constructor -- in header
*/

/**
   destructor
*/
svtDASHsensor::~svtDASHsensor(){ 

};

/** init - sets up serial queue.
 *   -
*/
int svtDASHsensor::init(){    
// initialize input queue
    _q.setTerminalQ(cEND);
    _q.setStartchar(cSTART);
    _q.baud(DASHBAUD);
// initialize debounce arrays
    for (unsigned ii = 0; ii < 8; ii++) {
	button_history[ii] = 0;
	cntrls_history[ii] = 0;
    }
    _ledB.write(1);
    displaycount = 0;
// initial state vector values that we control.   
    _ss->DashGapReq = 50;
    _ss->gappos =  _ss->DashGapReq; // we don't control pos, start with req until updated
    _ss->DashScreenOne = HOMESCREEN;
    _ss->DashScreenTwo = (HOMESCREEN)%MAXSCREENS;
    _ss->DashIgnBPS   = false;
    _ss->DashIgnMotor = false;
    _ss->DashForward = false;
    _ss->DashCCrsm   = false;
    _ss->DashCCsign  = false;
    _ss->DashCCincr  = false;
    _ss->DashCCon    = false;
    _ss->CC_state = 0;
    return 0;
}

/**  update -- process 
 *  Send dashboard state only when state changes or every 255 serial readings.
 */
void svtDASHsensor::update(){ 
    uint8_t cmd[CMDMAX];  // valid commands are cmdmax characters or less

    displaycount++;
    // receiving section
    _q.scanQ();  // conditions queue - looks for valid messages in queue
 
     // when have a valid message in serial queue, act on it
    if(_q.hasMsg()){          
       // Only if the messages have changed or after 255 messages ~3.4 sec. does it send message
	int length = _q.extractMsg(cmd, CMDMAX); // Extracts the valid message found
	if(length == 8 && cmd[3] == 1){  // is this the expected message
            // debounce input 
	    button_msg = cmd[4];
            debounce(button_msg, button_history, button_state);
	    cntrls_msg = cmd[5];
            debounce(cntrls_msg, cntrls_history, cntrls_state);
	    if( (button_msg != button_state) || (cntrls_msg != cntrls_state ) || (msg_cnt == 0) || firsttime){ // send msg
                parse(); // deal with changes
		button_state = button_msg;
		cntrls_state = cntrls_msg;
		readSensor(); 
		msg_cnt = 1;
		firsttime = true;
#ifdef DASHDEBUG              
                if(msg_cnt != 1 )udebug.printf("DASH: msg button %x %x\r\n", button_msg, cntrls_msg); 
#endif
	    }
	    else{
		msg_cnt++;  // keeps track of unchanging input             
	    }
	}                   
    }
    if(displayDirty){
	display_cmd();
    }
    else if(displaycount > 200){
	display_cmd();
	displaycount = 0;
    }
    
}

/** readSensor - overrides virtual base class function
 *  puts current dashboard state in CAN output queue.
 */
int svtDASHsensor::readSensor(){
    CAN_Message msg;
    uint8_t swstate;
    // pack CC_state
    _ss->CC_state = 0;
    if(_ss->DashCCrsm)   _ss->CC_state |=  CCRESUM;
    if(_ss->DashCCon)    _ss->CC_state |=  CRUISEON;
    if(_ss->DashCCsign)  _ss->CC_state |=  CCINCREASE;
    if(_ss->DashCCincr)  _ss->CC_state |=  CCCHGSPEED;
    swstate = 0;
    if(_ss->ArrayA)     swstate |= 0b00000001;
    if(_ss->ArrayB)     swstate |= 0b00000010;
    if(_ss->ArrayC)     swstate |= 0b00000100;
    if(_ss->MotCont)    swstate |= 0b00001000;
    if(_ss->ArrCont)    swstate |= 0b00010000;
    if(_ss->BSChKill)   swstate |= 0b00100000;
    if(_ss->Estop)      swstate |= 0b01000000;
    if(_ss->DashScreenTwo == GAPSCREEN) swstate |= 0b10000000; // rule for gap motor to be live.
    
    //udebug.printf("sw %x, Es %x\r\n", swstate, _ss->Estop);
  
    // build CAN message
    msg.id     = CANbase + 1; 
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = SVTCANFORMAT;
    msg.data[0] = _ss->CC_state;
    msg.data[1] = cntrls_state;
    msg.data[2] = _ss->DashGapReq;  // gap request
    msg.data[3] = _ss->last_c_state; // last c_state action
    msg.data[4] = swstate;
    msg.data[5] = _ss->DashScreenTwo;
    msg.data[6] = (_ss->volts24 & 0x00FF); // should be less than 255
    msg.data[7] = (_ss->volts12 & 0x00FF);
    if(sendOutMsg(msg) != 0){
#ifdef DASHDEBUG 
	udebug.printf("svtDASHsensor: mem fail in sendOutMsg 1 \r\n"); 
#endif
    }
#ifdef DASHDEBUG 
    //udebug.printf("DASH:sent %x cnt %d \r\n", msg.id, msgcounter); 
#endif
    return 0;
}

/** command - responds to CAN bus messages.
 * @param m - CAN message from another device on bus.
 */
int svtDASHsensor::command(CAN_Message& m){
    switch(m.id & 0x7FF){
    case 0x411: // Gap message - 3ed byte is position of gapmotor
	_ss->gappos = m.data[2];
	displayDirty = true;
#ifdef DASHDEBUG 
	//udebug.printf("DASH: %x gap pos %d cnt %d\r\n", m.id, _ss->gappos, m.data[3]); 
#endif
	break;
    case 1:  

	break;
    case 2:  

	break;
    case 3:  

	break;
    case 4:
	break;
    default:

	break;
    }
    return 0;
}

/*************************************************************************
-
- debounce -  debounce switch input - modified version of Benton's routine
-
*************************************************************************/
void svtDASHsensor::debounce(uint8_t& msg, uint8_t* history, uint8_t& state) {
    uint8_t bit;
    for (unsigned ii = 0; ii < 8; ii++) {
        // shift the bits in each debounce counter over by 1 and append the
        // current msg bit
        bit = 1 << ii;
	history[ii] = (history[ii] << 1) | ((msg & bit) >> ii);
		
        // if the debounce counter is filled completely with ones (minus the
        // part we don't care about - determined by mask), then change the bit in msg to one, if
        // counter is all zero change msg bit to zero, otherwise keep old value. 
	if( (history[ii] & DEBOUNCE_MASK) == DEBOUNCE_MASK) {   
	    msg |= bit;
	} 
        else if((history[ii] & DEBOUNCE_MASK) == 0){
	    msg &= ~bit;
	}
        else{
            msg = (msg & (~bit)) | (state & bit);  // keep current value until sure of change
	}  
    }
}

/*************************************************************************
-
- led_cmd - compose LED update message for Dashboard from state vector
-
*************************************************************************/
int svtDASHsensor::led_cmd(){
    uint8_t ledbit = DB_LED_LV;
	
    if ((_ss->BPSon || _ss->DashIgnBPS) && !_ss->BPSTrip) ledbit |= DB_LED_BPSG;
    if (_ss->BPSTrip) {
	ledbit |= DB_LED_BPSR;
	ledbit &= ~DB_LED_BPSG;
    }
    if (_ss->IgnOn || _ss->DashIgnMotor) ledbit |= DB_LED_MOTORG;
    if (_ss->ReginOn) ledbit |= DB_LED_BRAKE;
    if (!_ss->DashForward) ledbit |= DB_LED_REVERSE;
	
    ledmsg.resize(1);
    ledmsg[0] = ledbit;
    _q.putFMF(FMFCMD_BB_DB_LEDS, FMFID_DASHBOARD, ledmsg);
    ledmsg.clear();
#ifdef DASHDEBUG              
     udebug.printf("DASH: led %x \r\n", ledbit); 
#endif
    return 0;
}

/**************************************************
-
- display_cmd -compose FMF message to display unit
-
*************************************************/
int svtDASHsensor::display_cmd(){
    uint16_t IA;
    if(displaymsg.size() !=16 ) displaymsg.resize(16);
    displaymsg[0] = _ss->BbPIC24BMsg;
    // set flags
    if(_ss->DashIgnMotor)   displaymsg[1] = displaymsg[1] | ENGINEON;
    if(_ss->DashCCbrk)      displaymsg[1] = displaymsg[1] | BREAKON;
    if(_ss->DashForward)    displaymsg[1] = displaymsg[1] | DIRECTION;
    if(_ss->DashCCrsm)      displaymsg[1] = displaymsg[1] | CCRESUM;
    if(_ss->DashCCon)       displaymsg[1] = displaymsg[1] | CRUISEON;
    if(_ss->DashCCsign)     displaymsg[1] = displaymsg[1] | CCINCREASE;
    if(_ss->DashCCincr)     displaymsg[1] = displaymsg[1] | CCCHGSPEED;
    if(_ss->Thren)          displaymsg[1] = displaymsg[1] | BBTHREN;
    displaymsg[2] = _ss->DashScreenOne; // screen one index
    displaymsg[3] = _ss->DashScreenTwo; // screen two index
    displaymsg[4] = _ss->BPSsoc; // SOC
    // display expects unsigned int, does not do negative numbers
    // so convert cA to A and use absolute value
    if(_ss->BPScurrent >= 0){
      IA = _ss->BPScurrent/100; // convert current in cA to A
    }
    else{
      IA = (-_ss->BPScurrent)/100; 
    }
    displaymsg[5] =  IA     & 0x00FF; // current LB   (A)
    displaymsg[6] = (IA>>8) & 0x00FF; // current HB  (A)
    displaymsg[7] = _ss->LFPress; // LF tire pressure
    displaymsg[8] = _ss->RFPress; // RF tire pressure
    displaymsg[9] = _ss->RTPress; // R tire pressure
    displaymsg[10] = _ss->RFTemp; // RF tire temperature index
    displaymsg[11] = _ss->LFTemp; // LF
    displaymsg[12] = _ss->RTTemp; // R
    displaymsg[13] = _ss->DashGapReq; // gap motor request
    displaymsg[14] = _ss->gappos;     // gap motor pos
    displaymsg[15] = _ss->DashCCon;   // cruise control on/off
		
    if(_q.putFMF(FMFMSG_BB_DISPLAY, FMFID_DISPLAY, displaymsg) ==0){
	displayDirty = false; // clear flag if sent
    }
    return 0;
}

/*************************************************************************
-
- beep_cmd - compose beep command message given a requested frequency (in 
-            Hz) and duration (in milliseconds) of the desired beep
-
*************************************************************************/
int svtDASHsensor::beep_cmd(int frequency, int duration){
    uint16_t half_cycle_time, num_cycles;
	
    half_cycle_time = int(500000.0/float(frequency));
    num_cycles = frequency * duration / 1000;
	
    beepmsg.push_back((half_cycle_time >> 8) & 0xff);
    beepmsg.push_back(half_cycle_time & 0xff);
    beepmsg.push_back((num_cycles >> 8) & 0xff);
    beepmsg.push_back(num_cycles & 0xff);
	
    return 0;
}

/*************************************************************************
-
- parse - move data from message to state vector, depending on command
-
*************************************************************************/
int svtDASHsensor::parse(){
    uint8_t button_changes, cntrls_changes;
		
    button_changes = button_state ^ button_msg;
    cntrls_changes = cntrls_state ^ cntrls_msg;
		
    if (button_changes) {
#ifdef DASHDEBUG 
        udebug.printf("button presses\r\n");
#endif
	if (button_changes & DB_CRUISE_BUTTON) {
	    if (button_msg & DB_CRUISE_BUTTON) {
		ui_interact(DB_CRUISE_BUTTON);
		if (beepmsg.size() == 0) {
		    beep_cmd(3951, 300);
		    sendinterval_beep = 200;
		}
		// cout << "Cruise switch pressed at " << _ss->utime/1000 << " s" << endl;
	    }
	}
	if (button_changes & DB_L_BUTTON) {
	    if (button_msg & DB_L_BUTTON) {
		ui_interact(DB_L_BUTTON);
		if (beepmsg.size() == 0) {
		    //beep_cmd(2093, 300);
		    sendinterval_beep = 200;
		}
		// cout << "Left switch pressed at " << _ss->utime/1000 << " s" << endl;
	    }
	}
	if (button_changes & DB_R_BUTTON) {
	    if (button_msg & DB_R_BUTTON) {
	ui_interact(DB_R_BUTTON); 
		if (beepmsg.size() == 0) {
		    //beep_cmd(2349, 300);
		    sendinterval_beep = 200;
		}
		// cout << "Right switch pressed at " << _ss->utime/1000 << " s" << endl;
	    }
	}
	if (button_changes & DB_U_BUTTON) {
	    if (button_msg & DB_U_BUTTON) {
		ui_interact(DB_U_BUTTON);
		if (beepmsg.size() == 0) {
		    beep_cmd(2637, 300);
		    sendinterval_beep = 200;
		}
		// cout << "Up switch pressed at " << _ss->utime/1000 << " s" << endl;
	    }
	}
	if (button_changes & DB_D_BUTTON) {
	    if (button_msg & DB_D_BUTTON) {
		ui_interact(DB_D_BUTTON);
		if (beepmsg.size() == 0) {
		    beep_cmd(2793, 300);
		    sendinterval_beep = 200;
		}
		// cout << "Down switch pressed at " << _ss->utime/1000 << " s" << endl;
	    }
	}
	if (button_changes & DB_BACK_BUTTON) {
	    if (button_msg & DB_BACK_BUTTON) {
		ui_interact(DB_BACK_BUTTON);
		if (beepmsg.size() == 0) {
		    beep_cmd(3135, 300);
		    sendinterval_beep = 200;
		}
		// cout << "Back switch pressed at " << _ss->utime/1000 << " s" << endl;
	    }
	}
	if (button_changes & DB_HOME_BUTTON) {
	    if (button_msg & DB_HOME_BUTTON) {
		ui_interact(DB_HOME_BUTTON);
		if (beepmsg.size() == 0) {
		    beep_cmd(3520, 300);
		    sendinterval_beep = 200;
		}
		// cout << "Back switch pressed at " << _ss->utime/1000 << " s" << endl;
	    }
	}
	if (button_changes & DB_SELECT_BUTTON) {
	    if (button_msg & DB_SELECT_BUTTON) {
		ui_interact(DB_SELECT_BUTTON);
		if (beepmsg.size() == 0) {
		    beep_cmd(4186, 300);
		    sendinterval_beep = 200;
		}
		// cout << "Select switch pressed at " << _ss->utime/1000 << " s" << endl;
	    }
	}
    }
	
    // If some of the control bits changed since the last received packet...
    if (cntrls_changes) {
	if (cntrls_changes & SV_BLINKER) {
	    if (cntrls_msg & SV_BLINKER) {
		_ss->DashBlink = true;
		//Not quite sure what to do with this info
		// cout << "Blinkers turned on at " << _ss->utime/1000 << " s" << endl;
	    } else {
		_ss->DashBlink = false;
		//Again, what needs to know about the blinkers?
		// cout << "Blinkers turned off at " << _ss->utime/1000 << " s" << endl;
	    }
	}
	// If the brake light changes from off to on, turn off
	// the cruise control
	if (cntrls_changes & SV_BRAKE) {
	    if (cntrls_msg & SV_BRAKE) {
		_ss->DashCCon = false;
	    }
	}
	//If the reverse switch changes state, update the state vector accordingly
	if (cntrls_changes & SV_REVERSE) {
	    if (cntrls_msg & SV_REVERSE) {
		_ss->DashForward = false;
	    } else {
		_ss->DashForward = true;
	    }
	}
	if ((cntrls_changes & DB_IGN_BIT_1) || (cntrls_changes & DB_IGN_BIT_2)) {
	    uint8_t ign_state = (cntrls_msg & DB_IGN_BIT_1) | (cntrls_msg & DB_IGN_BIT_2);
			
	    //if the ignition state changes from some other value to the 
	    //shutdown state, then set the shutdown flag

	    if (ign_state == DB_IGN_STATE_1) {
		_ss->DashShutdown = true;
	    }
	    else if (ign_state == DB_IGN_STATE_2) {
		_ss->DashIgnBPS = false;
		_ss->DashIgnMotor = false;
	    }
	    else if (ign_state == DB_IGN_STATE_3) {
		_ss->DashIgnBPS = true;
		_ss->DashIgnMotor = false;
	    }
	    else if (ign_state == DB_IGN_STATE_4) {
		_ss->DashIgnBPS = true;
		_ss->DashIgnMotor = true;
	    }
#ifdef DASHDEBUG              
            udebug.printf("DASH: ign changed %x %x\r\n", button_msg, cntrls_msg); 
#endif
	}
        led_cmd(); // 
    }
    return 0;
}

/************************************************
 *
 *  ui_interact - changes state variables based on button presses
 *      responsible for:
 *               - display - which screen is shown (screen number)  (L/R)
 *               - gap - what is current gap request (increase/decrease) Gap screen + (Up/Down)
 *               - cruise control - on/off/resume increase/decrease  Cruise screen +  (Up/Down)
 *
 ************************************************/
int svtDASHsensor::ui_interact(uint8_t event) {
    vector<unsigned char> cmd_str;

    switch(event){
    case DB_CRUISE_BUTTON:
	_ss->DashCCon = !_ss->DashCCon;
	break;
    case DB_L_BUTTON:
        if(active_screen > 0) active_screen--;
        displayDirty = true;
	break;
    case DB_R_BUTTON:
	active_screen = (active_screen + 1)%MAXSCREENS;
        displayDirty = true;
	//cout << "event right button " << int(event) << " " << active_screen << endl;
	break;
    case DB_U_BUTTON:
        if(active_screen == GAPSCREEN){
	    _ss->DashGapReq = (_ss->DashGapReq + 1)%95; 
	    displayDirty = true;          
        }
	break;
    case DB_D_BUTTON:
        if(active_screen == GAPSCREEN && (_ss->DashGapReq > 5) ){
	    _ss->DashGapReq -= 1;  
	    displayDirty = true;         
        }
	break;
    case DB_BACK_BUTTON:
	break;
    case DB_HOME_BUTTON:
	prev_screen = active_screen;
	active_screen = HOMESCREEN;
        displayDirty = true;
	break;
    case DB_SELECT_BUTTON:
	break;
    default:
#ifdef DASHDEBUG 
	udebug.printf("UserInterface::interact: event not found \r\n");
#endif
	break;
    }
    if(_ss->DashScreenTwo != active_screen){
        _ss->DashScreenTwo = active_screen;
        _ss->DashScreenOne = HOMESCREEN;
        displayDirty = true;
    }
    return 1;
}


