/**
 *
 * File: CirQueue.h
 *
 * Purpose: Defines simple circular queue for a uart input
 *
 * Author: J. C. Wiley, Mar. 2015
 *          Extracted from SerialCirInQueue.
 *
 * Uses: circular queue, on overflow overwrites
 * oldest data.
 *
*/

#ifndef CIRQUEUE_H_
#define CIRQUEUE_H_
#include "mbed.h"

#include <vector>

class CirQueue{
public:
    CirQueue(int sz);
    ~CirQueue();
    void clearQ(); // empty queue
    bool emptyQ(); // is queue empty?
    int freeQ();  // space available in queue
    int sizeQ();  // number of elements in queue
    void pushQ(uint8_t x); // add element to queue
    uint8_t popQ(void);
    void dump();
    volatile int next;   // points to next free position
    volatile int first;  // points to first valid character
    bool inqueue(int i);
    int qs;  // queue size
    vector<uint8_t> d;
private:
    int lengthQ(int i);
};

#endif
