/***************************************************************************

    \brief      This is a child sensor class that simulates the gps and bps data.
    \details    The data is stored on an SD Card and polled from an external board.
                The external board waits for a GPIO line to be raised, reads a line
                from the data file, and sends the data to this board via a
                UART (Serial) line. The data will be a string that contains 2 floats
                seperated by a comma. These floats describe latitude and longitude
                respectifully. <br><br>
                Upon recieving this,we are going to parse, store, and collect
                this data with the same structure as any other sensor.
 ***************************************************************************/

#ifndef __SIM_GPS_BPS_H__
#define __SIM_GPS_BPS_H__

#include "mbed.h"
#include "svtSensor.h"
#include "nucleo_hat_pin_names.h"

extern Serial pc;

#define BNO055_ADDRESS_A (0x28)
#define BNO055_ADDRESS_B (0x29)
#define BNO055_ID        (0xA0)

#define NUM_BNO055_OFFSET_REGISTERS (22)

typedef union{
    char data[8];
    struct{
        float lat;
        float lon;
    }fields;
}SIM_GPS_BPS_DATA;
 
class simGpsBps : public svtSensor
{
  public:
    simGpsBps( int sensorID, int periodMs, PinName uartRxPin, PinName gpioPin);
    
    ~simGpsBps();
  
  private:
    Serial uart;
    DigitalOut gpio_request_data;
    
  public:
    virtual void init();
    virtual void readSensor(timeout_state* pToState, char * pData);
    virtual void readSensor(char * pData);
    virtual int getSizeOfData();
    
    void selftest();
    
  private:
  
};

#endif