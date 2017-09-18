/**

   File: svtBPSsensor.cpp

   Purpose - FMF communication with BPS (PIC) via 422 serial
   and outputs CAN message with BPS state
 
   Author - J. C. Wiley,  March. 2015
   BPS info from Beaglebone code, and Fred's notes

*/
#include "svtBPSsensor.h"
#include "SV_FMF.h"

// Fred's communication message tokens
#define cSTART  0x01
#define cEND    0xFE
// circular queue parameters for dashboard
#define CMDMAX  255

extern RawSerial udebug;

/**
   Constructor -- in header
*/

/**
   destructor
*/
svtBPSsensor::~svtBPSsensor(){ 

};

/** init - sets up serial queue.
 *   
*/
int svtBPSsensor::init(){    
    // initialize input queue
    _q.setTerminalQ(cEND);
    _q.setStartchar(cSTART);
    _q.baud(BPSBAUD);
    senddelta_ms = 250;  // Freq. of standard CAN messages
    lastsent_ms = 0;
    Tsnap.resize(MAXBATTERYCELLS, 0);
    Vsnap.resize(MAXBATTERYCELLS, 0);
    bootstrapCount = 0;
    return 0;
}

/**  update -- process 
 *    Monitors input switches, EStop, 12Vsense, 24Vsense,
 *    Controls bps and motor contactors
 *    Controls arrays
 */
void svtBPSsensor::update(){  
    uint8_t d[CMDMAX];  // FMF raw message  
    int n;

    // check for commands from controller (svtCNTRLsensor)
    if(_ss->BPScommand != FMFCMD_BB_BPS_NONE){
#ifdef BPSDEBUG
	udebug.printf("BPS: command (sv): %d\r\n", _ss->BPScommand);
#endif
	switch(_ss->BPScommand){
	case FMFCMD_BB_BPS_START: // Start measuring
	    start_cmd();
	    break;
	case FMFCMD_BB_BPS_STOP:  // Stop measuring (Contactor must be off)
	    stop_cmd();
	    break;
	case FMFCMD_BB_BPS_SYSON:  // Turn on system (Contactor)
	    syson_cmd();
	    break;
	case FMFCMD_BB_BPS_SYSOFF: // Turn off system (Contactor)
	    sysoff_cmd();
	    break;
	case FMFCMD_BB_BPS_RESET:  // Reset trip
	    reset_cmd();
	    break;
	    // commands not implemented yet
	case FMFCMD_BB_BPS_GETREG:  // Read register
	case FMFCMD_BB_BPS_SETREG:  // Set register
	case FMFCMD_BB_BPS_GETFRZF:  // Read FreezeFrame
	case FMFCMD_BB_BPS_GETHEALTH:  // Read Health
	case FMFCMD_BB_BPS_IDENT:  // Ask what system version
	case FMFCMD_BB_BPS_PASS:  // Passthrough Talk to slaves direct
	case FMFCMD_BB_BPS_GETRTC:  // Read RTC Read the time/date
	case FMFCMD_BB_BPS_PUTRTC:  // Write RTC Set the clock struct
	    tobedone_cmd();
	    break;
	default:
	    break;
	}
    }
  
    // look for messages on serial line from BPS
    _q.scanQ();  // conditions queue - looks for valid messages in queue
 
    // if there is a valid message in serial, queue act on it.
    if(_q.hasMsg()){          
	n = _q.extractMsg(d, CMDMAX); // Extracts raw FMF mesg including start and stop bytes
#ifdef BPSDEBUG
	udebug.printf("BPS: fmf in %x %d \r\n", d[3], _ss->BPSon); 
#endif        
        if(n >= 6 ){
	    parse(d[1], d[3], &d[4]);  
	}               
    }
    
    // Time to send CAN  messages?
    uint32_t now_ms = *msclockpt;
    if( now_ms > lastsent_ms + senddelta_ms){
	lastsent_ms = now_ms;
        readSensor();
    }
}

/** readSensor - overrides virtual base class function
 *   writes changed BPS CAN messages to System CAN bus. 
 */
int svtBPSsensor::readSensor(){
    CAN_Message msg;
    msg.id     = CANbase + 1;
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = CANStandard;
    msg.data[0] =  _ss->BPSsoc;
    msg.data[1] =  _ss->BPSstatus;
    msg.data[2] =  (_ss->BPSMaxV & 0x00FF);
    msg.data[3] =  (_ss->BPSMaxV & 0xFF00) >>8;
    msg.data[4] =   _ss->BPSMaxVindex;
    msg.data[5] =  (_ss->BPSMinV & 0x00FF);
    msg.data[6] =  (_ss->BPSMinV & 0xFF00) >>8;
    msg.data[7] =   _ss->BPSMinVindex;
    if(sendOutMsg(msg) != 0){
	 udebug.printf(" mem fail in sendOutMsg 1 \r\n");
    }

    msg.id     = CANbase + 2;
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = CANStandard;
    msg.data[0] = (_ss->BPScurrent & 0x00FF);
    msg.data[1] = (_ss->BPScurrent & 0xFF00) >>8;
    msg.data[2] = (_ss->BPSMaxT & 0x00FF);
    msg.data[3] = (_ss->BPSMaxT & 0xFF00) >>8;
    msg.data[4] =  _ss->BPSMaxTindex;
    msg.data[5] = (_ss->BPSMinT & 0x00FF);
    msg.data[6] = (_ss->BPSMinT & 0xFF00) >>8;
    msg.data[7] =  _ss->BPSMinTindex;
    if(sendOutMsg(msg) != 0){
	 udebug.printf(" mem fail in sendOutMsg 2 \r\n");
    }
    // udebug.printf("bps I %d, %x, %x, %x \r\n",_ss->BPScurrent ,_ss->BPScurrent, msg.data[0], msg.data[1]);

    // function snapShot generates CAN data
    if(snapShot(&msg.data[0])){
	msg.id     = CANbase + 3;
	msg.len    = 8;
	msg.type   = CANData;
	msg.format = CANStandard;
	if(sendOutMsg(msg) != 0){
	    udebug.printf(" mem fail in sendOutMsg 3 \r\n");
	}
    }

    if(bootstrapCount == 0){
    // bootstrap pack top data
    msg.id     = CANbase + 4;
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = CANStandard;
    msg.data[0] = (_ss->BPS_bs_T_t & 0x00FF);
    msg.data[1] = (_ss->BPS_bs_T_t & 0xFF00) >>8;
    msg.data[2] = (_ss->BPS_bs_I_t & 0x00FF);
    msg.data[3] = (_ss->BPS_bs_I_t & 0xFF00) >>8;
    msg.data[4] = (_ss->BPS_bs_V_t & 0x00FF);
    msg.data[5] = (_ss->BPS_bs_V_t & 0xFF00) >>8;
    msg.data[6] = (_ss->BPS_bs_Y_t & 0x00FF);
    msg.data[7] = (_ss->BPS_bs_Y_t & 0xFF00) >>8;
    if(sendOutMsg(msg) != 0){
	 udebug.printf(" mem fail in sendOutMsg 2 \r\n");
    }
    }

    if(bootstrapCount == 2){

    // bootstrap pack bottom data
    msg.id     = CANbase + 5;
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = CANStandard;
    msg.data[0] = (_ss->BPS_bs_T_b & 0x00FF);
    msg.data[1] = (_ss->BPS_bs_T_b & 0xFF00) >>8;
    msg.data[2] = (_ss->BPS_bs_I_b & 0x00FF);
    msg.data[3] = (_ss->BPS_bs_I_b & 0xFF00) >>8;
    msg.data[4] = (_ss->BPS_bs_V_b & 0x00FF);
    msg.data[5] = (_ss->BPS_bs_V_b & 0xFF00) >>8;
    msg.data[6] = (_ss->BPS_bs_Y_b & 0x00FF);
    msg.data[7] = (_ss->BPS_bs_Y_b & 0xFF00) >>8;
    if(sendOutMsg(msg) != 0){
	 udebug.printf(" mem fail in sendOutMsg 2 \r\n");
    }
    }

    if(bootstrapCount == 4){
    // bootstrap pack status data
    msg.id     = CANbase + 6;
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = CANStandard;
    msg.data[0] = _ss->BPS_bs_status_t;
    msg.data[1] = _ss->BPS_bs_status_b;
    msg.data[2] = 0;
    msg.data[3] = 0;
    msg.data[4] = 0;
    msg.data[5] = 0;
    msg.data[6] = 0;
    msg.data[7] = 0;
    if(sendOutMsg(msg) != 0){
	 udebug.printf(" mem fail in sendOutMsg 2 \r\n");
    }
    }
    bootstrapCount++;
    if(bootstrapCount == 20) bootstrapCount = 0; // reduced duty cycle for bootstrap

    return 0;
}

/** snapShot -- manages snapshot of T and V vectors, send small blocks in rotation
 *  @param - pointer to CAN message data, 8 bytes.
 */
bool svtBPSsensor::snapShot(uint8_t* d){
    bool rtn;
    if(idxTV == 0){  // start new snap shot and increment block count.
	blkcnt++;
	// store current values
	for(int i=0; i< MAXBATTERYCELLS; i++){
	    Tsnap[i] = _ss->BPStemp[i];
	    Vsnap[i] = _ss->BPSvolt[i];
	}
	// grab time 
	uint32_t now_ms = *msclockpt; 
	snapTime = now_ms & 0x00FF;
	//udebug.printf("snap %x %d %d \r\n", idxTV, Tsnap[6], _ss->BPStemp[6]);
    }
    int offset = 3*(idxTV & 0x0F); // grap last nibble, mult by 3
    // if offset in range in number of modules 
    // construct CAN data
    rtn = false;
    if(offset  < MAXBATTERYCELLS){  // last one or two values may be junk
	d[0] = ((blkcnt << 5) + idxTV) & 0x00FF;
	d[1] = snapTime;
	if( (idxTV & 0x10) == 0){  // low bit - volts
	    d[2] = Vsnap[offset]     & 0x00FF;
	    d[3] =(Vsnap[offset]     & 0xFF00) >>8;
	    d[4] = Vsnap[offset + 1] & 0x00FF;
	    d[5] =(Vsnap[offset + 1] & 0xFF00) >>8;
	    d[6] = Vsnap[offset + 2] & 0x00FF;
	    d[7] =(Vsnap[offset + 2] & 0xFF00) >>8;	
	}
	else{   // high flag - temp
	    d[2] = Tsnap[offset]     & 0x00FF;
	    d[3] =(Tsnap[offset]     & 0xFF00) >>8;
	    d[4] = Tsnap[offset + 1] & 0x00FF;
	    d[5] =(Tsnap[offset + 1] & 0xFF00) >>8;
	    d[6] = Tsnap[offset + 2] & 0x00FF;
	    d[7] =(Tsnap[offset + 2] & 0xFF00) >>8;		
	}
	rtn = true;
    }
    idxTV += 1;
    if(idxTV > 31) {
	idxTV = 0;
    }
    return rtn;
}
   
/** command - responds to messages on CAN bus.
 * @param m - CAN message. First byte is command.
 */
int svtBPSsensor::command(CAN_Message& m){
    switch(m.id){
    case 0: 

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
- parse - move data from FMF message to state vector, depending on command
-
*************************************************************************/
int svtBPSsensor::parse(uint8_t len, uint8_t cmd, uint8_t* d){
    uint16_t tp;
#ifdef BPSDEBUG
	udebug.printf("parse %d \r\n", d[3], cmd); 
#endif
    switch(cmd){
    case FMFCMD_BPS_BB_STATUS:  // should be 3 bytes in message
    {
	_ss->BPSstatus = (d[0] & BPSstatusmask);
	// current signed integer, read in 100th’s of an amp, rescale 
	_ss->BPScurrent = int16_t((d[1] << 8) + d[2]); // signed centiamps cA
	_ss->BPSbalancing = (d[0] & BPSbalancingmask);
	_ss->BPSTrip      = (_ss->BPSstatus == 3);
	_ss->BPSon        = (_ss->BPSstatus == 2);
    }
    break;
    case FMFCMD_BPS_BB_VOLTS: // cell voltages (ccV) msb, lsb, msb, lsb,...  (ccV ==10^-4 V)
    {
	_ss->BPSMaxV = 0;  // contrafactuals
	_ss->BPSMinV = 40000;
	for(int i = 0; i < min(len/2, MAXBATTERYCELLS); i++){
	    tp = (d[2*i] << 8) + d[2*i+1];
	    _ss->BPSvolt[i] = tp;
	    if(_ss->BPSMaxV < tp){
		_ss->BPSMaxV = tp; // keep track of maximum voltage
		_ss->BPSMaxVindex = i;
	    }
	    if(_ss->BPSMinV > tp){
		_ss->BPSMinV = tp;  // minimum voltage
		_ss->BPSMinVindex = i;
	    }
	}
	//udebug.printf("V6 %d\r\n", _ss->BPSvolt[6]);
    }
    break;
    case FMFCMD_BPS_BB_TEMPS:  // cell temperatures (10ths of a degree Kelvin, cK)
    {
	_ss->BPSMaxT = 0;  // contrafactuals
	_ss->BPSMinT = 40000;
	for(int i = 0; i < min(len/2, MAXBATTERYCELLS ); i++){
	    tp = (d[2*i] << 8) + d[2*i+1];
	    _ss->BPStemp[i] = tp;
	    if(_ss->BPSMaxT < tp){
		_ss->BPSMaxT = tp;
		_ss->BPSMaxTindex = i;
	    }
	    if(_ss->BPSMinT > tp){
		_ss->BPSMinT = tp;
		_ss->BPSMinTindex = i;
	    }
	}
    }
    break;
    case FMFCMD_BPS_BB_BALANCING: // Balancing
    {
	// Document missing?
    }
    break;
    case FMFCMD_BPS_BB_BOOTSTRAP: // Bootstrap
    {
	_ss->BPS_bs_T_t = (d[4] << 8) + d[5];   // bootstrap top temp (100th kelvin) unsigned
	_ss->BPS_bs_I_t = (d[2] << 8) + d[3];   // bootstrap top I    (100th A)      signed
	_ss->BPS_bs_V_t = (d[0] << 8) + d[1];   // bootstrap top V         (mV)      unsigned
	_ss->BPS_bs_Y_t = (d[6] << 8) + d[7];   // bootstrap top V_supply  (mV)      unsigned
	_ss->BPS_bs_status_t = d[8];            // bootstrap top status -- byte code of leds

	_ss->BPS_bs_T_b = (d[13] << 8) + d[14];  // bootstrap bottom temp (100th kelvin) unsigned
	_ss->BPS_bs_I_b = (d[11] << 8) + d[12];  // bootstrap bottom I    (100th A)      signed
	_ss->BPS_bs_V_b = (d[9]  << 8) + d[10];  // bootstrap bottom V        (mV)       unsigned
	_ss->BPS_bs_Y_b = (d[15] << 8) + d[16];  // bootstrap bottom V_supply (mV)       unsigned
	_ss->BPS_bs_status_b = d[17];            // bootstrap bottom status -- byte code of leds
    }
    break;
    case FMFCMD_BPS_BB_SOC:  // state of charge
    {
	if(len == 1){
	    // error checking?
	    int soc = d[0];
	    if(soc >=0 && soc <= 100){ // valid range
		_ss->BPSsoc = soc;
	    }
	    else{  // 255 indicates not ready, other values should not be seen.
		_ss->BPSsoc = 255;
	    }
	}
    }
    break;
    case FMFCMD_BPS_BB_REGVAL:	  	// Register Value
    {

    }
    break;
    case FMFCMD_BPS_BB_SYSHEALTH:
    {	// System Health

    }
    break;
    case FMFCMD_BPS_BB_FFSTAT: // FreezeFrame Status
    {
	// Document missing?
    }
    break;
    case FMFCMD_BPS_BB_FFVOLT:	// FreezeFrame Voltages
    {
	// Document missing?
    }
    break;
    case FMFCMD_BPS_BB_FFTEMP: // Freeze Frame Temperatures
    {
	// Document missing?
    }
    break;
    case FMFCMD_BPS_BB_ID: // System ID
    {
	// Document missing?
    }
    break;
    case FMFCMD_BPS_BB_Pass:  // Pass through Response
    {

    }
    break;
    case FMFCMD_BPS_BB_RTC:	// Clock Time  6 bytes
    {
    }
    break;
//		 case ???:   FMFCMD_BPS_BB_SOC
//				 I have three main variables: a main
//				 accumulator (32 bit unsigned int. units are amp-milliseconds capacity
//				 in the battery), a percentage (a byte from 0 to 100) (derived from the
//				 accumulator), and an initialization status (i.e., was the system
//				 successful is restoring the previous value of the accumulator from the
//				 battery backed SRAM). I'd assume I'd just have it periodically send an
//				 FMF with the percent SoC in it.
//			break;
    default:
	return 1;
    } // switch
    _ss->LastBPSMeasurement = *msclockpt; // received message from BPS (ms), -> must be running
    return 0;
}

/*************************************************************************
-
- start_cmd - compose BPS start command
-
*************************************************************************/
int svtBPSsensor::start_cmd(){
    bpsmsg.clear();
    if(_q.putFMF(FMFCMD_BB_BPS_START, FMFID_BPS, bpsmsg) == 0){
	_ss->BPScommand = FMFCMD_BB_BPS_NONE;
    }
#ifdef BPSDEBUG
    udebug.printf("BPS start\r\n");
#endif
    return 0;
}
/*************************************************************************
-
- stop_cmd - compose BPS stop command
-
*************************************************************************/
int svtBPSsensor::stop_cmd(){
    bpsmsg.clear();
    if(_q.putFMF(FMFCMD_BB_BPS_STOP, FMFID_BPS, bpsmsg) == 0){
	_ss->BPScommand = FMFCMD_BB_BPS_NONE;
    }
#ifdef BPSDEBUG
    udebug.printf("BPS: stop\r\n");
#endif
    return 0;
}

/*************************************************************************
-
-  syson_cmd() - compose BPS syson command
-
*************************************************************************/
int svtBPSsensor::syson_cmd(){
    bpsmsg.clear();
    if(_q.putFMF(FMFCMD_BB_BPS_SYSON, FMFID_BPS, bpsmsg) == 0){
	_ss->BPScommand = FMFCMD_BB_BPS_NONE;
    }
#ifdef BPSDEBUG
    udebug.printf("BPS: syson to bps\r\n");
#endif
    return 0;
}

/*************************************************************************
-
- sysoff_cmd - compose BPS sysoff command
-
*************************************************************************/
int svtBPSsensor::sysoff_cmd(){
    bpsmsg.clear();
    if(_q.putFMF(FMFCMD_BB_BPS_SYSOFF, FMFID_BPS, bpsmsg) == 0){
	_ss->BPScommand = FMFCMD_BB_BPS_NONE;
    }
#ifdef BPSDEBUG
    udebug.printf("BPS: sysoff\r\n");
#endif
    return 0;
}

/*************************************************************************
-
- reset_cmd - compose BPS reset command
-
*************************************************************************/
int svtBPSsensor::reset_cmd(){
    bpsmsg.clear();
    if(_q.putFMF(FMFCMD_BB_BPS_RESET, FMFID_BPS, bpsmsg) == 0){
	_ss->BPScommand = FMFCMD_BB_BPS_NONE;
    }
#ifdef BPSDEBUG
    udebug.printf("BPS: reset\r\n");
#endif
    return 0;
}

/*************************************************************************
-
- tobedone_cmd - commands not yet implimented
-
*************************************************************************/
int svtBPSsensor::tobedone_cmd(){
    _ss->BPScommand = FMFCMD_BB_BPS_NONE;
#ifdef BPSDEBUG
    udebug.printf("BPS: unimplemented command \r\n");
#endif
    return 0;
}
