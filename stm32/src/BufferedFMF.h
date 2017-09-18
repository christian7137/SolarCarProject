/**
 *
 * File: BufferedFMF.h
 *
 * Purpose: Uses circular queues to buffer serial I/O, specialized for FMF
 *
 * Author: J. C. Wiley, Mar. 2015
 *          
 * Uses: circular queue, on overflow overwrites
 * oldest data.
 *
*/

#ifndef BUFFEREDFMF_H
#define BUFFEREDFMF_H
#include "BufferedSerial.h"

class BufferedFMF: public BufferedSerial{
public:
BufferedFMF(PinName tx, PinName rx, int insz, int outsz):BufferedSerial(tx,rx,insz,outsz){
     ux.attach(this, &BufferedFMF::readIRQ, RawSerial::RxIrq);  // connect readIRQ to the uart read interrupt;
     ux.attach(this, &BufferedFMF::sendIRQ, RawSerial::TxIrq);
     ux.baud(9600);
}
    ~BufferedFMF();
    virtual int extractMsg(uint8_t* d, int n);
    void scanQ();  // scans characters in queue for valid message
    int putFMF(uint8_t cmd, uint8_t devID, vector<uint8_t> &data);

    bool validCKS(int i); // is the checksum valid for the message
    void setTerminalQ(uint8_t tc){termchar = tc;}
    void setStartchar(uint8_t sc){startchar = sc;}
    bool isStop(int i); // checks if there is a valid stop byte for the message

private:
    volatile int msgend;
    virtual void readIRQ();  // IRQ read routine
    virtual void sendIRQ();
};

#endif
