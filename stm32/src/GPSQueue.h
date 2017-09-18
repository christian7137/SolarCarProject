/**
 *
 * File: GPSQueue.h
 *
 * Purpose: Defines simple circular queue for a uart input
 *
 * Author: J. C. Wiley, Oct. 2014
 *
 * Uses: circular queue, on overflow overwrites
 * oldest data.
 *
*/

#ifndef SERIALINCIRQUEUE_H_
#define SERIALINCIRQUEUE_H_
#include "mbed.h"
#include <stdlib.h>
#define BUFFERSZ 500
class GPSQueue{
    public:
         GPSQueue(PinName tx, PinName rx);
        ~GPSQueue();
         int init();
         void clearQ(); // empty queue
         bool emptyQ(); // is queue empty?
         int freeQ();  // space available in queue
         int sizeQ();  // number of elements in queue
         void pushQ(uint8_t x); // add element to queue
         uint8_t popQ(void);
         int scanQ(char* d, int n);  // scans characters in queue for valid message
         void baud(int r); // set baud rate of serial port
         void setTerminalQ(uint8_t tc){termchar = tc;}
         void setStartQ(uint8_t sc){startchar = sc;}
         int writestr( char *s); // sends string to serial port
    private:
         RawSerial ux;
         int qs;  // queue size
         volatile int next;   // points to next free position
         volatile int first;  // points to first valid character
         volatile int msgend;
         uint8_t termchar;  // character that terminates a command
         uint8_t startchar; // character that starts message
         int lengthQ(int i);
         bool inqueue(int i);
         void readIRQ();  // IRQ read routine
         IRQn_Type irqn;  // IRQ number for specific uart
         uint8_t d[BUFFERSZ];
};

#endif
