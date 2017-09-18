 /******************************************************
  -
  - file - svtIMUsensor.h
  -
  - purpose - I2C interface to Sparkfun 9DOF board.
  -
  - author - J. C. Wiley, Feb. 2015
  -
****************************************************/
#ifndef svtIMUSENSOR_H
#define svtIMUSENSOR_H

#include "mbed.h"
#include <stdint.h>
#include "svtSensor.h"

#define HMC5883LW 0x3C  // write address for HMC5883L
#define HMC5883LR 0x3D  // read  address for HMC5883L
#define ADXL345W  0xA6  // write address for ADXL345
#define ADXL345R  0xA7  // read  address for ADXL345
#define ITG3200W  0xD0  // write address for ITG3200W
#define ITG3200R  0xD1  // read  address for ITG3200W

// Registers for ADXL345
#define  ADXL345_DEVID      0x00
#define  ADXL345_THRESH_TAP 0x1D
#define  ADXL345_OFSX       0x1E
#define  ADXL345_OFSY       0x1F
#define  ADXL345_OFSZ       0x20
#define  ADXL345_DUR        0x21
#define  ADXL345_Latent     0x22
#define  ADXL345_Window     0x23
#define  ADXL345_THRESH_ACT 0x24
#define  ADXL345_THRESH_INACT  0x25
#define  ADXL345_TIME_INACT    0x26
#define  ADXL345_ACT_INACT_CTL 0x27
#define  ADXL345_THRESH_FF     0x28
#define  ADXL345_TIME_FF       0x29
#define  ADXL345_TAP_AXES      0x2A
#define  ADXL345_ACT_TAP_STATUS 0x2B
#define  ADXL345_BW_RATE        0x2C
#define  ADXL345_POWER_CTL      0x2D
#define  ADXL345_INT_ENABLE     0x2E
#define  ADXL345_INT_MAP        0x2F
#define  ADXL345_INT_SOURCE     0x30
#define  ADXL345_DATA_FORMAT    0x31
#define  ADXL345_DATAX0         0x32   // lsb
#define  ADXL345_DATAX1         0x33   // msb
#define  ADXL345_DATAY0         0x34   // lsb
#define  ADXL345_DATAY1         0x35   // msb
#define  ADXL345_DATAZ0         0x36   // lsb
#define  ADXL345_DATAZ1         0x37   // msb
#define  ADXL345_FIFO_CTL       0x38
#define  ADXL345_FIFO_STATUS    0x39
//HMC5883L
#define CONFIGA_REG 0x00
#define CONFIGB_REG 0x01
#define MODE_REG    0x02
#define XMSB_REG    0x03
#define XLSB_REG    0x04
#define YMSB_REG    0x07
#define YLSB_REG    0x08
#define ZMSB_REG    0x05
#define ZLSB_REG    0x06
#define STATUS_REG  0x09
#define IDA_REG     0x0A
#define IDB_REG     0x0B
#define IDC_REG     0x0C
// Registers for ITG3200
#define REG_WHO_AM_I      0x00
#define REG_SMPLRT_DIV    0x15
#define REG_DLPF_FS       0x16
#define REG_GYRO_TEMP_H   0x1B
#define REG_GYRO_TEMP_L   0x1C
#define REG_GYRO_XOUT_H   0x1D
#define REG_GYRO_XOUT_L   0x1E
#define REG_GYRO_YOUT_H   0x1F
#define REG_GYRO_YOUT_L   0x20
#define REG_GYRO_ZOUT_H   0x21
#define REG_GYRO_ZOUT_L   0x22
#define REG_GYRO_PWR_MGM  0x3E


typedef struct imu_state{
    uint8_t HMC_XMSB;
    uint8_t HMC_XLSB;
    uint8_t HMC_YMSB;
    uint8_t HMC_YLSB;
    uint8_t HMC_ZMSB;
    uint8_t HMC_ZLSB;
    uint8_t ADX_XMSB;
    uint8_t ADX_XLSB;
    uint8_t ADX_YMSB;
    uint8_t ADX_YLSB;
    uint8_t ADX_ZMSB;
    uint8_t ADX_ZLSB;
    uint8_t ITG_XMSB;
    uint8_t ITG_XLSB;
    uint8_t ITG_YMSB;
    uint8_t ITG_YLSB;
    uint8_t ITG_ZMSB;
    uint8_t ITG_ZLSB;
    uint8_t ITG_TMSB;
    uint8_t ITG_TLSB;
    uint8_t counter;
    uint8_t status;
} imuState_t;

class svtIMUsensor: public svtSensor{
public:
    svtIMUsensor(PinName sda, PinName scl, int addr);
    ~svtIMUsensor();
    virtual int init();
    virtual int readSensor();
    virtual void update();
    virtual int command(CAN_Message& m);
private:
    I2C* m_i2c;
    int HMCaddr;
    int ADXaddr; 
    int ITGaddr;
    uint8_t Sbuf[12];
    void readRegs(int devaddr, int addr, uint8_t * data, int len);
    void writeRegs(int devaddr, uint8_t * data, int len); 

    imuState_t sv; 
    Timer t;
    // checks for chip id, to see if chip is active
    uint8_t ADXid();
    uint8_t HMCid();
    uint8_t ITGid();
    // initialize and turn on chips
    void ADXpowerOn();
    void HMCstartup();
    void ITGstartup();
    // get axis data
    void HMCreadAxes();
    void ADXreadAxes();
    void ITGreadAxes();
    // calibration/testing
    void ADXselftest();
};
#endif
