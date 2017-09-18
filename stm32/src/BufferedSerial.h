/**
 *
 * File: BufferedSerial.h
 *
 * Purpose: Uses circular queues to buffer serial I/O
 *
 * Author: J. C. Wiley, Mar. 2015
 *          
 *
 * Uses: circular queue, on overflow overwrites
 * oldest data.
 *
*/

#ifndef BUFFEREDSERIAL_H
#define BUFFEREDSERIAL_H
#include "CirQueue.h"
#include "RawSerial.h"

class BufferedSerial{
public:
    BufferedSerial(PinName tx, PinName rx, int insz, int outsz):ux(tx, rx), inQ(insz), outQ(outsz){
           validmsg = false;
   /*  // irq number for given uart pin (K64F) */
   /*  if(tx == PTB17){ */
   /*      irqn = UART0_RX_TX_IRQn; */
   /*  } */
   /*  else if(tx == PTC4){ */
   /*      irqn = UART1_RX_TX_IRQn; */
   /*  } */
   /*  else if(tx == PTD3 ){ */
   /*      irqn = UART2_RX_TX_IRQn; */
   /*  } */
   /* else if(tx == PTC17){ */
   /*      irqn = UART3_RX_TX_IRQn; */
   /*  } */
   /*  else if(tx == PTC15 ){ */
   /*      irqn = UART4_RX_TX_IRQn; */
    }

    ~BufferedSerial();
    bool hasMsg(){return validmsg;} // does queue contain a valid message?
    virtual int extractMsg(uint8_t* d, int n) = 0;
    void scanQ();  // scans characters in input queue for valid message
    void clearQ(){inQ.clearQ();} // clear input queue
    void setTerminalQ(uint8_t tc){termchar = tc;}
    void setStartchar(uint8_t sc){startchar = sc;}
    void baud(int r); // set baud rate of serial port
    void sendA();
    int inQsize(){return inQ.sizeQ();}
protected:
    RawSerial ux;
    CirQueue inQ;
    CirQueue outQ;
    volatile int msgend;
    volatile bool validmsg;  // valid message at start of queue?
    uint8_t termchar;  // character that terminates a message
    uint8_t startchar; // character that starts a mesage
    void prime(void);
    virtual void readIRQ()=0;  // IRQ read routine
    virtual void sendIRQ()=0;
    IRQn_Type irqn;  // IRQ number for specific uart
};

#endif
