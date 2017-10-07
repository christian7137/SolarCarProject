/***************************************************************
   -
   - file:  svtSTM32CAN.h
   -
   - purpose: handles connection to solar car main CAN bus
   -
   - author: J. C. Wiley, March 2017
   -         uses STM32_CAN which uses mbed can driver.
   -         for thread within class see: https://mbed.org/forum/mbed/topic/4388/
   -
***************************************************************/
#ifndef _SVTSTM32CAN_H_
#define _SVTSTM32CAN_H_

#include "mbed.h"
#include "rtos.h"
#include "PeripheralPins.h"
#include "PinNames.h"
//#define MBED_CAN_HELPER_H 1
#include "svtCAN.h"
//#include "stm32_CAN.h"

extern RawSerial udebug;  // crutch for testing

/** svtSTM32CAN class
 *   Uses mbed CAN library 
 */
class svtSTM32CAN: public svtCAN { 
public:
    /** svtSTM32CAN - constructor
     */
svtSTM32CAN():
    svtCAN(),
        _stm32can(PB_8, PB_9) {
	_canmsg.len = 0;
        _errcount = 0;
        _unsentcnt = 0;
	_inbufcnt = 0;
    };
 
    /** svtSTM32CAN - destructor
     */
    ~svtSTM32CAN();

    /** init - initialize STM32 CAN pins and clock
     */
    int init(int canspeed, CANFormat format = SVTCANFORMAT, uint32_t canaddr = 0x0234, svtSensorMgr* s=0);

    /** Filter - sets upto six input id filters, which when combined with
     *  @param mask  32 bit mask, where a 1 indicates bit will be checked between
     *     incoming-id and filter-id, 0 is pass all. The underlying hardware may support
     *     more complex masking, but here only single global mask used.
     *   mask, determine which messages are read from bus.
     *   @param idfilters - pointer to an array of filters
     *   @param n - number of filters supplied
     *   @return 0 - success, 1 failure
     */
    int Filter(int32_t mask, int32_t* idfilters, int32_t n);

    /** Set the frequency of the CAN interface
     *
     *  @param hz The bus frequency in hertz
     *
     *  @returns
     *    1 if successful,
     *    0 otherwise
     */
    int frequency(int hz);

    /** handleInMsg - retrieve msg from CAN peripheral and put in queue
     */
    int handleInMsg();

    /** handleOutMsg - retrieve msg from OUTqueue and send CAN message.
     */
    int handleOutMsg();

/** RxErrors - returns number of read errors
 *
 */
    virtual int RxErrors();

/** TxErrors - returns number of transmit  errors
 *
 */
    virtual int TxErrors();
      
private:          
    CAN           _stm32can; 
    CANMessage   _canmsg; // single out can message 
    int           _errcount; 
    int           _unsentcnt; // counts number of repeats
    int           _inbufcnt; // number of msgs in In buffer
};
#endif

