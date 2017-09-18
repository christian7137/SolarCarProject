/**
 *
 * File: BufferedNGM.h
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

#ifndef BUFFEREDNGM_H
#define BUFFEREDNGM_H

#include "BufferedSerial.h"
#include <string>
#define NGM_CR 0x0D
#define NGM_LF 0x0A
#define NGM_HT 0x09

class BufferedNGM: public BufferedSerial{
public:
BufferedNGM(PinName tx, PinName rx, int insz, int outsz):BufferedSerial(tx,rx,insz,outsz){
	ux.attach(this, &BufferedNGM::readIRQ, RawSerial::RxIrq); 
	ux.attach(this, &BufferedNGM::sendIRQ, RawSerial::TxIrq);
          lastchar = 0;
    }
    ~BufferedNGM();
    int extractMsg(uint8_t* d, int n);
    void scanQ();  // scans characters in queue for valid message
    int putNGM(string& s);
    int putNGM(const char *s);
private:
    volatile int msgend;
    void readIRQ();  // IRQ read routine
    void sendIRQ();
    uint8_t lastchar;

};

#endif
