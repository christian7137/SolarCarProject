/**

   File: svtTPMSsensor.cpp

   Purpose - Uses I2C interface to manage tire pressure sensor.
 
   Author - J. C. Wiley,  Feb. 2015

*/
#include "svtTPMSsensor.h"

extern RawSerial udebug;

/**
   Constructor
*/
svtTPMSsensor::svtTPMSsensor(PinName sda, PinName scl, int addr) {
    // activate the peripheral
    m_i2c = new I2C(sda, scl);
    maddr = addr;
    m_i2c->frequency(200000);
    state = starting;
    t.start(); // start timer
}

/**
   destructor
*/
svtTPMSsensor::~svtTPMSsensor(){ 
};

/** readRegs - I2C read 
 */
void svtTPMSsensor::readRegs(int addr, uint8_t * data, int len) {
    char t[1] = {addr};
    m_i2c->write(maddr, t, 1, true);
    m_i2c->read(maddr, (char *)data, len);
}

/** writeRegs - I2C write 
 */
void svtTPMSsensor::writeRegs(uint8_t * data, int len) {
    m_i2c->write(maddr, (char *)data, len);
}

/** FSM - runs local finite state machine
 */
void svtTPMSsensor::FSM(){
    switch(state){
    case starting:
        readSN();
        if(t.read()>3){  // wait 3 seconds
	    state = clearing;
	}
	break;
    case clearing:
        deleteTires(0x00);  // <-- put tire position here
        state = normal;
        break;
    case normal: 
        if(t.read() > 10){   // after 10 seconds, accept new air
            Sbuf[0] = CurrentTire;
            Sbuf[1] = 0x8;       // tire position
            writeRegs(Sbuf, 2);
            Sbuf[0] = AddTire;
            writeRegs(Sbuf, 1);
	    state = listning;
	}
	break;
    case listning:   // waiting for new sensor - stop waiting after 50 seconds.
        //dump();
        if(t.read() > 50){
            Sbuf[0] = AddTireCan;
            writeRegs(Sbuf, 1);
	    state = running;
	}
	break; 
    case running:
        Current();
        Pressure();
        Temperature();
        Alarm12();
        Alarm25();
        Seen();
        Missing();
        //dump();
        if( ((t.read_ms()>>12) & 0x7) == 0){  // about 32 sec.
            readSensor();
        }
	break;
    default:
	break;
    }
}

/** init
    -
*/
int svtTPMSsensor::init(){
    return 0;
}

/** reset - pull reset line low for 5 ms. ( not sure length)?
 */
void svtTPMSsensor::reset(){
    // hardware line not implimented yet.
}

/**
   -  status - TPMS status command, query slave address only
*/
void svtTPMSsensor::Status(){
    m_i2c->read(maddr, (char *)Sbuf, 1);
    status = Sbuf[0];
}

/** deleteTires - deletes tires flagged in ct argument. 
    @param ct - 8 bit array (1 byte)  for tires to be deleted
    - Sets current tire [0] to ct and then deletes tires.
*/
void svtTPMSsensor::deleteTires(uint8_t ct){
    // setup command and current tire bits.
    Sbuf[0] = CurrentTire;
    Sbuf[1] = ct;
    Sbuf[2] = 0;
    Sbuf[3] = 0;
    Sbuf[4] = 0;
    Sbuf[5] = 0;
    writeRegs(Sbuf, 6);
    // send delete tire command
    Sbuf[0] = DelTire;
    writeRegs(Sbuf, 1);
}

/** Alarm12 - reads 12% low alarm byte, updates state vector
 *
 */
void svtTPMSsensor::Alarm12(){
    readRegs(Warning12, Sbuf, 5);
    sv.alarm12 = Sbuf[0];
}

/** Alarm25 - reads 25% low alarm byte, updates state vector
 *
 */
void svtTPMSsensor::Alarm25(){
    readRegs(Warning25, Sbuf, 5);
    sv.alarm25 = Sbuf[0];
}

/** Seen - reads tires seen in last 15 minutes
 *
 */
void svtTPMSsensor::Seen(){
    readRegs(SensorSeen, Sbuf, 5);
    sv.seen = Sbuf[0];
}

/** Missing - reads tires assumed to be missing
 *
 */
void svtTPMSsensor::Missing(){
    readRegs(MSWarning, Sbuf, 5);
    sv.missing = Sbuf[0];
}

/** EEactive- eeprom active tires
 *
 */
void svtTPMSsensor::EEactive(){
    readRegs(EE_Tires, Sbuf, 5);
    eeactive = Sbuf[0];
}

/** Pressure - reads tire pressure
 *
 */
void svtTPMSsensor::Pressure(){
    readRegs(SensorValue0, Sbuf, 5);
    sv.P1 = Sbuf[0];
    sv.P2 = Sbuf[1];
    sv.P3 = Sbuf[2];
    sv.P4 = Sbuf[3];
}

/** Missing - reads tires assumed to be missing
 *
 */
void svtTPMSsensor::Temperature(){
    readRegs(SensorTemp0, Sbuf, 5);
    sv.T1 = Sbuf[0] & 0x07;
    sv.T2 = Sbuf[1] & 0x07;
    sv.T3 = Sbuf[2] & 0x07;
    sv.T4 = Sbuf[3] & 0x07;
}

/** Current - current tire register
 *
 */
void svtTPMSsensor::Current(){
    readRegs(CurrentTire, Sbuf, 5);
    current = Sbuf[0];
}

/** readSN - reads 4 serial numbers
   -
*/
void svtTPMSsensor::readSN(){
    sv.SN1 = 0;
    sv.SN2 = 0;
    sv.SN3 = 0;
    sv.SN4 = 0;

    readRegs(EE_SNmbrs0, Sbuf, 5);
    sv.SN1 =  (Sbuf[0] << 16) | (Sbuf[1] << 8) | Sbuf[2];
    sv.SN2 =  (Sbuf[3] << 16) | (Sbuf[4] << 8);    

    readRegs(EE_SNmbrs1, Sbuf, 5);
    sv.SN2= sv.SN2 | Sbuf[0];
    sv.SN3 = (Sbuf[1] << 16) | (Sbuf[2] << 8) | Sbuf[3];
    sv.SN4 = (Sbuf[4] << 16); 

    readRegs(EE_SNmbrs2, Sbuf, 5);
    sv.SN4 =  sv.SN4 | (Sbuf[0] << 8) | Sbuf[1];
}

/**  update -- allow FSM to have time slice
 */
void svtTPMSsensor::update(){ 
    Status(); // update status
    FSM();
}

/** readSensor - overrides virtual base class function
 *  data for CAN msgs stored in state vector, and which should be current
 *  puts message on CAN output queue.
 */
int svtTPMSsensor::readSensor(){
    CAN_Message msg;
    msg.id     = CANbase + 1;
    msg.len    = 8;
    msg.type   = CANData;
    msg.format = SVTCANFORMAT;
    msg.data[0] =  sv.P1;
    msg.data[1] =  sv.P2;
    msg.data[2] =  sv.P3;
    msg.data[3] =  sv.P4; 
    msg.data[4] =  ((sv.T1 & 0x07) << 4) | (sv.T2 & 0x07);
    msg.data[5] =  ((sv.T3 & 0x07) << 4) | (sv.T4 & 0x07);
    msg.data[6] =  ((sv.alarm12 & 0x0F) << 4) | (sv.alarm25 & 0x0F);
    msg.data[7] =  ((sv.seen & 0x0F) << 4) | (sv.missing & 0x0F);
    if(sendOutMsg(msg) != 0){
	udebug.printf(" mem fail in sendOutMsg 1 \r\n"); 
    }
    return 0;
}

/** command - responds to Beaglebone commands.
 * @param m - CAN message from Beaglebone. First byte is command.
 */
int svtTPMSsensor::command(CAN_Message& m){
    switch(m.data[0]){
    case 0: // delete tires in second byte
        deleteTires(m.data[1]);
	break;
    case 1:  // update local serial numbers
        readSN();
	break;
    case 2:  // send first two serial numbers
    {
	CAN_Message msg;
	msg.id     = CANbase + 2;
	msg.len    = 8;
	msg.type   = CANData;
	msg.format = SVTCANFORMAT;
	msg.data[0] =  (sv.SN1 & 0x0000000FF);
	msg.data[1] =  (sv.SN1 & 0x00000FF00) >> 8;
	msg.data[2] =  (sv.SN1 & 0x000FF0000) >> 16;
	msg.data[3] =  (sv.SN2 & 0x0000000FF);
	msg.data[4] =  (sv.SN2 & 0x00000FF00) >> 8;
	msg.data[5] =  (sv.SN2 & 0x000FF0000) >> 16;
	msg.data[6] =  0;
	msg.data[7] =  0;
	if(sendOutMsg(msg) != 0){
	    udebug.printf(" mem fail in sendOutMsg 1 \r\n"); 
	}
    }
    break;
    case 3:  // send SN 3 and 4
    {
	CAN_Message msg;
	msg.id     = CANbase + 3;
	msg.len    = 8;
	msg.type   = CANData;
	msg.format = SVTCANFORMAT;
	msg.data[0] =  (sv.SN3 & 0x0000000FF);
	msg.data[1] =  (sv.SN3 & 0x00000FF00) >> 8;
	msg.data[2] =  (sv.SN3 & 0x000FF0000) >> 16;
	msg.data[3] =  (sv.SN4 & 0x0000000FF);
	msg.data[4] =  (sv.SN4 & 0x00000FF00) >> 8;
	msg.data[5] =  (sv.SN4 & 0x000FF0000) >> 16;
	msg.data[6] =  0;
	msg.data[7] =  0;
	if(sendOutMsg(msg) != 0){
	    udebug.printf(" mem fail in sendOutMsg 1 \r\n"); 
	}
    }
	break;
    case 4:
	break;
    default:
	    udebug.printf("tmps received unknown command \r\n");
	break;
    }
    return 0;
}

/**  dump - debugging data dump
 *
 */
void svtTPMSsensor::dump(){
    //if(udebug != NULL){
        udebug.printf("\r\n tmps status                    %x \r\n", status);
        udebug.printf("current %2x \r\n", current);
        udebug.printf("P1 %3d, P2 %3d, P3 %3d, P4 %3d \r\n", sv.P1, sv.P2, sv.P3, sv.P4);
        udebug.printf("T1 %3d, T2 %3d, T3 %3d, T4 %3d \r\n", sv.T1, sv.T2, sv.T3, sv.T4);
        udebug.printf("alarm12: %2x, alarm25: %2x \r\n", sv.alarm12, sv.alarm25);
        udebug.printf("seen:    %2x, missing: %2x \r\n", sv.seen,    sv.missing);    
	udebug.printf("sn1 %4x \r\n", sv.SN1);
	udebug.printf("sn2 %4x \r\n", sv.SN2);
	udebug.printf("sn3 %4x \r\n", sv.SN3);
	udebug.printf("sn4 %4x \r\n", sv.SN4);
	// }
}
