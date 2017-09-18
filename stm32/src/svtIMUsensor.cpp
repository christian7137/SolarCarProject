/******************************************************
  -
  - file - svtIMUsensor.cpp
  -
  - purpose - I2C interface to Sparkfun 9DOF board.
  -
  - author - J. C. Wiley, Feb. 2015
  -
****************************************************/
#include "svtIMUsensor.h"

extern RawSerial udebug;

/** Constructor
 *
 */
svtIMUsensor::svtIMUsensor(PinName sda, PinName scl, int addr){
    // activate the peripheral
    m_i2c = new I2C(sda, scl); // PTB3, PTB4
    HMCaddr = HMC5883LW;
    ADXaddr = ADXL345W;
    ITGaddr = ITG3200W;
    m_i2c->frequency(200000);
    t.start(); // start timer
    // turn on chips and set initial configurations
    ADXpowerOn();
    HMCstartup();
    ITGstartup();
}

/** Destructor
 *
 */
svtIMUsensor::~svtIMUsensor(){
}

/** init - initializations
 *
 */
int svtIMUsensor::init(){
    int rtn = 0;

    // check device IDs
    if(ADXid() != 0xE5  ) rtn = rtn | 0x01;
    if(HMCid() != 0x48  ) rtn = rtn | 0x02;
    if(ITGid() != 0x69  ) rtn = rtn | 0x04;
    return rtn;
}

/** update
 *
*/
void svtIMUsensor::update(){
    // ADX
    ADXreadAxes();
    // HMC5883L
    HMCreadAxes();
    // ITG
    ITGreadAxes();
    // send CAN msgs
    readSensor();
}

/**
   readRegs I2C read 
*/
void svtIMUsensor::readRegs(int devaddr, int addr, uint8_t *data, int len){
    char t[1] = {addr};
    m_i2c->write(devaddr, t, 1, true);
    m_i2c->read(devaddr, (char *)data, len);
}

/**
   writeRegs I2C write 
*/
void svtIMUsensor::writeRegs(int devaddr, uint8_t *data, int len){
    m_i2c->write(devaddr, (char *)data, len);
}

/** ADXpowerOn -- turns on power
 *
 */
void svtIMUsensor::ADXpowerOn(){
    Sbuf[0] = ADXL345_POWER_CTL; // turn power on
    Sbuf[1] =  0x08;
    writeRegs(ADXaddr, Sbuf, 2);
}

/** HMCstartup - sets running conditions
 *
 */
void svtIMUsensor::HMCstartup(){
    Sbuf[0] = CONFIGA_REG; 
    Sbuf[1] =  0x08;  //one sample, 3 Hz, normal meas.
    writeRegs(HMC5883LW, Sbuf, 2);
    Sbuf[0] = CONFIGB_REG; 
    Sbuf[1] =  0x20;  // default gain
    writeRegs(HMC5883LW, Sbuf, 2);
    Sbuf[0] = MODE_REG; 
    Sbuf[1] =  0x00; // continuous
    writeRegs(HMC5883LW, Sbuf, 2);
}

/** ITGstartup - sets running conditions
 * 
 */
void svtIMUsensor::ITGstartup(){
    Sbuf[0] =  REG_GYRO_PWR_MGM; 
    Sbuf[1] =  0x01;  // power on
    writeRegs(ITG3200W, Sbuf, 2);
    Sbuf[0] = REG_SMPLRT_DIV; 
    Sbuf[1] =  0x07;  // sampling
    writeRegs(ITG3200W, Sbuf, 2);
    Sbuf[0] = REG_DLPF_FS; 
    Sbuf[1] =  0x1C; //filter
    writeRegs(ITG3200W, Sbuf, 2);
}

uint8_t svtIMUsensor::ADXid(){
    readRegs(ADXL345W, ADXL345_DEVID, Sbuf, 1);
    udebug.printf("adx ID %x \r\n", Sbuf[0]);
    return Sbuf[0];

}
uint8_t svtIMUsensor::HMCid(){
    readRegs(HMC5883LW, IDA_REG, Sbuf, 1);
    udebug.printf("hmc ID, %x \r\n", Sbuf[0]);
    return Sbuf[0];

}
uint8_t svtIMUsensor::ITGid(){
    readRegs(ITG3200W, REG_WHO_AM_I, Sbuf, 1);
    udebug.printf("itg ID, %x \r\n", Sbuf[0]);
    return Sbuf[0];
}

void svtIMUsensor::HMCreadAxes(){
    readRegs(HMC5883LW, XMSB_REG, Sbuf, 6);
    sv.HMC_XMSB = Sbuf[0];
    sv.HMC_XLSB = Sbuf[1];
    sv.HMC_YMSB = Sbuf[4];
    sv.HMC_YLSB = Sbuf[5];
    sv.HMC_ZMSB = Sbuf[2];
    sv.HMC_ZLSB = Sbuf[3];    
}

void svtIMUsensor::ADXreadAxes(){
    readRegs(ADXL345W, ADXL345_DATAX0, Sbuf, 6);
    sv.ADX_XLSB = Sbuf[0];
    sv.ADX_XMSB = Sbuf[1];
    sv.ADX_YLSB = Sbuf[2];
    sv.ADX_YMSB = Sbuf[3];
    sv.ADX_ZLSB = Sbuf[4];
    sv.ADX_ZMSB = Sbuf[5];    
}

void svtIMUsensor::ITGreadAxes(){
    readRegs(ITG3200W, REG_GYRO_TEMP_H, Sbuf, 8);
    sv.ITG_TMSB = Sbuf[0];
    sv.ITG_TLSB = Sbuf[1];
    sv.ITG_XMSB = Sbuf[2];
    sv.ITG_XLSB = Sbuf[3];
    sv.ITG_YMSB = Sbuf[4];
    sv.ITG_YLSB = Sbuf[5];
    sv.ITG_ZMSB = Sbuf[6];
    sv.ITG_ZLSB = Sbuf[7];    
}

/** ReadSensor - Queues 3 CAN messages from IMU
 *
 */
int svtIMUsensor::readSensor(){
     CAN_Message msg;
     msg.id     = CANbase + 1;
     msg.len    = 8;
     msg.type   = CANData;
     msg.format = SVTCANFORMAT;
     msg.data[0] =  sv.ITG_XMSB;
     msg.data[1] =  sv.ITG_XLSB;
     msg.data[2] =  sv.ITG_YMSB;
     msg.data[3] =  sv.ITG_YLSB; 
     msg.data[4] =  sv.ITG_ZMSB;
     msg.data[5] =  sv.ITG_ZLSB;
     msg.data[6] =  sv.ITG_TMSB;
     msg.data[7] =  sv.counter;
     if(sendOutMsg(msg) != 0){
     	 udebug.printf(" mem fail in sendOutMsg 1 \r\n"); 
     }

     msg.id     = CANbase + 2;
     msg.len    = 8;
     msg.type   = CANData;
     msg.format = SVTCANFORMAT;
     msg.data[0] =  sv.ADX_XMSB;
     msg.data[1] =  sv.ADX_XLSB;
     msg.data[2] =  sv.ADX_YMSB;
     msg.data[3] =  sv.ADX_YLSB; 
     msg.data[4] =  sv.ADX_ZMSB;
     msg.data[5] =  sv.ADX_ZLSB;
     msg.data[6] =  sv.ITG_TLSB;
     msg.data[7] =  sv.counter;
     if(sendOutMsg(msg) != 0){
     	 udebug.printf(" mem fail in sendOutMsg 2 \r\n"); 
     }

     msg.id     = CANbase + 3;
     msg.len    = 8;
     msg.type   = CANData;
     msg.format = SVTCANFORMAT;
     msg.data[0] =  sv.HMC_XMSB;
     msg.data[1] =  sv.HMC_XLSB;
     msg.data[2] =  sv.HMC_YMSB;
     msg.data[3] =  sv.HMC_YLSB; 
     msg.data[4] =  sv.HMC_ZMSB;
     msg.data[5] =  sv.HMC_ZLSB;
     msg.data[6] =  sv.status;
     msg.data[7] =  sv.counter;
     if(sendOutMsg(msg) != 0){
     	 udebug.printf(" mem fail in sendOutMsg 3 \r\n"); 
     }
     return 0;
}

/** ADXselftest -
 * Self-test change is defined as the output (g) when the SELF_TEST bit = 1 
 * (in the DATA_FORMAT register) minus the output (g) when the SELF_TEST bit = 0 
 * (in the DATA_FORMAT register). Due to device filtering, the output reaches its 
 * final value after 4 × τ when enabling or disabling self-test, where τ = 1/(data rate).
 */
void svtIMUsensor::ADXselftest(){


}

/**
    command  -- responds to commands
*/
int svtIMUsensor::command(CAN_Message& m){
    return 0;
}
