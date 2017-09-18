/************************************
  -
  - file: svtGPSsensor.cpp
  -
  - purpose: instance of svtSensor for K65F onboard accelerometer.
  -
  - author: J. C. Wiley, Feb. 2015
  -     ADAFruit GPS Ultra sensor
  -
*************************************/
#include "svtGPSsensor.h"

/** svtGPSsensor - Constructor
*/
svtGPSsensor::svtGPSsensor(PinName tx, PinName rx, int addr): sq(tx, rx){
    // activate the peripheral
    uTX = tx;
    uRX = rx;
    cmdmax = CMDMAXLEN;
}

/** svtGPSsensor - Destructor
*/
svtGPSsensor::~svtGPSsensor(){

};

/** init - initializes after construction (overrides)
*/
int svtGPSsensor::init(){ 
    status = 0;
    if(sq.init() != 0){
      status =  -1;
    }
    return status;
}

/** readSensor - overrides virtual base class function
 *  data for three CAN msgs stored in gc, and should be current
 *  put three messages on CAN output queue.
*/
int svtGPSsensor::readSensor(){
     CANMessage msg;
     msg.id     = CANbase + 1;
     msg.len    = 8;
     msg.type   = CANData;
     msg.format = CANStandard;
     msg.data[0] =  gc.tt & 0x000000FF;
     msg.data[1] =  (gc.tt & 0x0000FF00) >>8;
     msg.data[2] =  (gc.tt & 0x00FF0000) >>16;
     msg.data[3] =  (gc.tt & 0xFF000000) >>24; 
     msg.data[4] =  gc.lat_deg;
     msg.data[5] =  gc.lat_min;
     msg.data[6] =  gc.statqual;;
     msg.data[7] =  gc.seq;
     if(sendOutMsg(msg) != 0){
	 udebug.printf(" mem fail in sendOutMsg 1 \r\n"); 
     }

     msg.id     = CANbase + 2;
     msg.len    = 8;
     msg.type   = CANData;
     msg.format = CANStandard;
     msg.data[0] =  gc.long_deg;
     msg.data[1] =  gc.long_min;
     msg.data[2] =  gc.altitude & 0x00FF;
     msg.data[3] = (gc.altitude & 0xFF00) >> 8;
     msg.data[4] =  gc.speed & 0x00FF;
     msg.data[5] = (gc.speed & 0xFF00) >> 8;
     msg.data[6] =  (gc.sats & 0x00FF);
     msg.data[7] =  gc.seq;
     if(sendOutMsg(msg) != 0){
	 udebug.printf(" mem fail in sendOutMsg 2 \r\n"); 
     }

     msg.id     = CANbase + 3;
     msg.len    = 8;
     msg.type   = CANData;
     msg.format = CANStandard;
     msg.data[0] =  gc.lat_104_min & 0x00FF;
     msg.data[1] = (gc.lat_104_min & 0xFF00) >>8;
     msg.data[2] =  gc.long_104_min & 0x00FF;
     msg.data[3] = (gc.long_104_min & 0xFF00) >>8; 
     msg.data[4] = (gc.hog & 0x00FF);
     msg.data[5] = (gc.hog & 0xFF00) >> 8;
     msg.data[6] =  gc.hdop;
     msg.data[7] =  gc.seq;
     if(sendOutMsg(msg) != 0){
	 udebug.printf(" mem fail in sendOutMsg 3 \r\n"); 
     }
     return 0;
}

/** command - receive command via CAN message
 *     a command may tell sensor to change internal settings
 *     or initiate an activity
 */
int svtGPSsensor::command(CANMessage& m){

    return 0;
}

/** update - update is called at the requested interval
 */
void svtGPSsensor::update(){
    int n;

    if((n = sq.scanQ(&cmd[0], cmdmax)) > 0){
	// valid message?
	//udebug.printf(" gps %d %s \r\n", n, cmd);
	if( cmd[0] == '$' && cmd[n-4] == '*'){
	    //GPGGA
	    if( cmd[4] == 'G'){
		parseGGA(cmd, n);
                seq1 = gc.seq;
	    }
	    // GPRMC
	    else if( cmd[4] == 'M'){
		parseRMC(cmd, n);
                seq2 = gc.seq;
	    }
	}            
    }
    // both sentences at same second and not sent before.
    
    if( (seq1 == seq2) && (seq1 != lastseq)){
	readSensor(); // extracts data from structure and sends CAN messages.
	//udebug.printf("gps seq %d \r\n", gc.seq);
      lastseq = seq1;
    }
}

/** parseRMC - extracts data from GPRMC NMEA sentence
 *  @param pointer to sentence
 *  @param length of sentence
 */
void svtGPSsensor::parseRMC(char* s, int n){
    int ctab[11];
    int i=0;
    double x;
    struct tm tinfo;

    // delimit fields
    for(int k=0; k<11; k++){
	while(s[i] != ',' && i < n)i++;
        ctab[k] = i++;
    }
    // check status
    if( s[ctab[2]-1] == 'A'){  // only changes on active (valid) fix.
        gc.statqual = 0x10 |(gc.statqual & 0x0F) ;
	// extract time/date of fix
	s[ctab[1]-4] = 0;
	tinfo.tm_sec = atoi( &s[ctab[1]-6]); // 0-60
	s[ctab[1]-6] = 0;  
	tinfo.tm_min = atoi(&s[ctab[1]-8]); // 0-59
	s[ctab[1]-8] = 0;
	tinfo.tm_hour = atoi(&s[ctab[0]]+1);  // 0-23 
	s[ctab[9]]=0;
	tinfo.tm_year = atoi(&s[ctab[9]-2]) +100;  // 1900+
	s[ctab[9]-2] = 0;
	tinfo.tm_mon = atoi(&s[ctab[9]-4]) -1 ; // 0-11
	s[ctab[9]-4] = 0;
	tinfo.tm_mday = atoi(&s[ctab[9]-6]);   // 1-31
        gc.seq = tinfo.tm_sec;
	// unix time stamp GMT
	gc.tt = mktime(&tinfo);
	// latitude
	s[ctab[3]] = 0;
	gc.lat_104_min = atoi(&s[ctab[3]-4]);
	s[ctab[3]-5] = 0;
	gc.lat_min = atoi(&s[ctab[3]-7]);
	s[ctab[3]-7] = 0;
	gc.lat_deg = atoi(&s[ctab[2]+1]);
	if( s[ctab[4]-1] == 'S') gc.lat_deg = -gc.lat_deg; //hemisphere S is neg
        // longitude
	s[ctab[5]] = 0;
	gc.long_104_min = atoi(&s[ctab[5]-4]);
	s[ctab[5]-5] = 0;
	gc.long_min = atoi(&s[ctab[5]-7]);
	s[ctab[5]-7] = 0;
	gc.long_deg = atoi(&s[ctab[4]+1]);
	if(s[ctab[6]-1] == 'W') gc.long_min = -gc.long_min;
	// speed  - value in sentence is float with two decimal place in knots
	// convert to float, float to kph, multiply by 2 to give units of 0.5kph and
	// convert to unsigned byte, which is sent. Bounded by commas index 6 and 7
	s[ctab[7]] = 0; // truncate string
	x = atof(&s[ctab[6]+1]); // knots
        x = 2*1.85200*x;  // convert to units of 1/2 km per hr.
	gc.speed = uint16_t(x);
    }
}

/** parseGGA - extracts data from GPGGA NMEA sentence
 *  @param pointer to sentence
 *  @param length of sentence
 */
void svtGPSsensor::parseGGA(char* s, int n){
    int ctab[14];
    int i = 0;

    // delimit fields
    for(int k=0; k<14; k++){
	while(s[i] != ',' && i < n)i++;
        ctab[k] = i++;
    }
    // check status
    if( s[ctab[5]+1] != '0'){  // only changes if not invalid fix.
        // fix second
        s[ctab[1]-4] = 0;
        gc.seq = atoi(&s[ctab[1]-6]);
	// extract quality
	s[ctab[5]+2] = 0;
	gc.statqual =   (gc.statqual & 0xF0) | atoi( &s[ctab[5]+1]); // 0-8
        // number of satellites
	s[ctab[7]] = 0;  
	gc.sats = atoi(&s[ctab[7]-2]); // 0-99
        // altitude
        s[ctab[9]-2] = s[ctab[9]-1];
 	s[ctab[9]-1] = 0;
	gc.altitude = atoi(&s[ctab[8]+1]);  // 0-65000
        // height over geiod
        s[ctab[11]-2] = s[ctab[11]-1];
 	s[ctab[11]-1] = 0;
	gc.hog = atoi(&s[ctab[10]+1]) ;  
        // horizontal dilution of position
 	s[ctab[8]] = 0;
	double x = atof(&s[ctab[7]+1]);
        gc.hdop = int(x*10) ; 
    }
}
