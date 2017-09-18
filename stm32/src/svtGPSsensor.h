/************************************
  -
  - file: svtGPSsensor.h  
  -
  - purpose: instance of svtSensor for GPS
  -
  - author: J. C. Wiley, Feb. 2015
  -     Adafruit GPS Ultra V3 sensor
  -
*************************************/
#ifndef svtGPSsensor_H
#define svtGPSsensor_H

#include "RawSerial.h"
#include "svtSensor.h"
#include "GPSQueue.h"

// svtGPS internal register addresses

extern RawSerial udebug;
#define CMDMAXLEN 90
struct gpscan {
    uint32_t tt;
    int8_t lat_deg;
    uint8_t lat_min;
    uint8_t statqual;
    // msg 2
    uint8_t long_deg;
    int8_t  long_min;
    uint16_t altitude;
    uint16_t speed;
    uint8_t sats;
    // msg 3
    uint16_t lat_104_min;
    uint16_t long_104_min;
    int16_t hog;
    uint8_t hdop;
    uint8_t seq;
};

/** svtGPSsensor  .
 *  
 */
class svtGPSsensor: public svtSensor {
public:
    /** svtGPSsensor - Constructor
     */
    svtGPSsensor(PinName utx, PinName urx, int addr);

    /** svtGPS - Destructor
     */
    ~svtGPSsensor();

    /** init - initializes after construction (overrides)
     */
    virtual int init();

    /** readSensor - overrides virtual base class function
     */
    virtual int readSensor();

    /** command - receive command via CAN message
     *     a command may tell sensor to change internal settings
     *     or initiate an activity
     */
    virtual int command(CANMessage& m);

    /** update - update is called at the requested interval
     */
    virtual void update();

private:
    GPSQueue sq;
    PinName uTX;
    PinName uRX;
    int cmdmax;
    char cmd[CMDMAXLEN];
    int status;
    void parseGGA(char* s, int n);
    void parseRMC(char* s, int n);
    gpscan gc;
    uint8_t lastseq;
    uint8_t seq1;
    uint8_t seq2;
};   
#endif

