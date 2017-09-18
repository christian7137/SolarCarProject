/**

   File: TPMSsensor.h

   Purpose - Uses I2C interface to manage tire pressure sensor.
 
   Author - J. C. Wiley,  Feb. 2015

*/
#ifndef svtTPMSsensor_H
#define svtTPMSsensor_H
#include "svtSensor.h"

#include <stdint.h>

// block read commands (smart mode)
#define CurrentTire   0xC0   // CurrentTire[5] & Indicates which tire(s) is (are) being added or deleted
#define ActiveTires   0xC1   // ActiveTires[5] & Which tires are active in this monitor
#define SensorSeen    0xC2   // SensorSeen[5]  & Sensor Received Bitmap \footnote{Cleared every 15 min, then bits set as tires come in}
#define MSWarning     0xC3   // MSWarning[5]   & Indicates which tires are missing\footnote{
#define ACK_Alarm     0xC4   // ACK_Alarm[5]   & Indicates which warnings are being masked
#define LowBatt       0xC5   // LowBatt[5]     & Low Battery Bitmap for tires
#define Warning12     0xC6   // Warning12[5]   & Which tires are 12.5\% low, and missing sensors
#define Warning25     0xC7   // Warning25[5]   & Indicates which tires are 25\% low
#define SensorValue0  0xCA   // SensorValue 1st 5 of 40 &  Latest pressure value (byte 4) from each sensor (and special codes)
#define SensorValue5  0xCB   // SensorValue offset 5    & (2nd 5 of 40)
#define SensorValue35 0xD1   // SensorValue offset 35
#define SensorCntr0   0xD2   // SensorCntr 1st 5 of 4   & Individual Sensor RX'ed Packet Count
#define SensorCntr35  0xD9   // SensorCntr offset 35    &
#define SensorTemp0   0xDA   // SensorTemp 1st 5 of 40  & Latest status/temperature from each sensor
#define SensorTemp35  0xE1   // SensorTemp offset 35    &
#define EE_Tires      0xE2   // EE_Tires[5]
#define EE_SNmbrs0    0xE3   // EE_SNmbrs 1st 5 of 120 & Array of sensor serial numbers, stored locally in EEPROM
#define EE_SNmbrs1    0xE4   // EE_SNmbrs 2ed 5 of 120 & Array of sensor serial numbers, stored locally in EEPROM
#define EE_SNmbrs2    0xE5   // EE_SNmbrs 3ed 5 of 120 & Array of sensor serial numbers, stored locally in EEPROM

#define EE_SNmbrs115  0xFA   // EE_SNmbrs offset 115
#define EE_Flags      0xFB   // EE_Flags   & 1st byte is version in two nibbles, x.x, 3rd is default power-up mode

// temp sensor back byte, top 4 bits are signal strength, bottom 3 bits = n in
//    temperature formula  (n-2)*20 C - steps of [-40C, -20C, 0C, 20C, 40C, 80C, 100C]
// MSWarnings - really sensors seen, since last clear (every 15 minutes)
// sensorvalue - (pressure) two codes: FA (250) - packet not seen
//                                     FB (251) - unprogrammed
// commands
#define PASSTHRUCMD 0x01
#define FILTEREDCMD 0x02
#define SMARTCMD    0x03
#define AddTire     0xFC
// Flag unit to accept the next unknown (new) ?new air? sensor S/N
//  for the lowest numbered bit flagged in the CurrentTire array.
#define AddTireCan  0xFD
// Cancel ?add tire? state, presumably due to user action
// or timeout at higher layers.
#define DelTire     0xFE
// Delete the tire(s) flagged in CurrentTire,
// with the associated bits in all other 5-byte
// arrays being cleared. The CurrentTire array is
// cleared, and the updated ActiveTires array is copied to EE.

// slave read and write addresses
#define TPMSWRITE 0xA8
#define TPMSREAD  0xA9

typedef struct tpmsState { 
    // pressures
    uint8_t P1; 
    uint8_t P2; 
    uint8_t P3; 
    uint8_t P4;
    // 3 bit temperature indices 
    uint8_t T1; 
    uint8_t T2; 
    uint8_t T3; 
    uint8_t T4; 
    // nibble arrays
    uint8_t alarm12; 
    uint8_t alarm25;
    uint8_t seen;
    uint8_t missing;
    // serial numbers 24 bits
    uint32_t SN1; 
    uint32_t SN2; 
    uint32_t SN3;  
    uint32_t SN4;  
} tpmsState_t;


enum FSMstate_t{normal, listning, running, clearing, starting};

/** TPMSsensor class.
 * Uses I2C interface to manage tire pressure sensor.
 *  
 */
class svtTPMSsensor: public svtSensor {
public:
    svtTPMSsensor(PinName sda, PinName scl, int addr);
    ~svtTPMSsensor();

    int init();
    virtual int readSensor();
    virtual int command(CAN_Message& m);
    virtual void update();

private:
    I2C* m_i2c;
    int maddr;
    uint8_t status;
    uint8_t current;   // current tire register
    uint8_t eeactive;
    uint8_t Sbuf[12];
    tpmsState_t sv;  
    FSMstate_t state;  
    Timer t; 
    void Status();
    void readRegs(int addr, uint8_t * data, int len);
    void writeRegs(uint8_t * data, int len); 
    void reset();
    void Current();
    void Pressure();
    void Temperature();
    void Alarm12();
    void Alarm25();
    void Seen();
    void Missing();
    void EEactive();
    void FSM();
    void readSN();
    void deleteTires(uint8_t ct);
    void dump();  // dump data to debug port
};
#endif
