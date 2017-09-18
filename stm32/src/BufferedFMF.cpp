/**
 *
 * File: BufferedFMF.cpp
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
#include "BufferedFMF.h"

extern RawSerial udebug;
extern DigitalOut ledR;

/** Destructor
 *
 */
BufferedFMF::~BufferedFMF(){
    ux.attach(NULL, RawSerial::RxIrq);  // connect readIRQ to the uart read interrupt;
    ux.attach(NULL, RawSerial::TxIrq);
}

/** readIRQ - interrupt routine for uart, read character and push in queue
 */
void BufferedFMF::readIRQ(){
    inQ.pushQ( ux.getc()); // read character to clear interrupt
}

/** sendIRQ -Tx interrupt routine for uart, write character to uart
 */
void BufferedFMF::sendIRQ(){
    while( ux.writeable() && !outQ.emptyQ() ) {
	ux.putc(outQ.popQ());
    }
    if(outQ.emptyQ()) ux.attach(NULL, RawSerial::TxIrq);
}

/** scanQ - scans queue for valid message, which is defined here:
 *  
 *    A valid message: is sequence of bytes starting with 0x01 ending in terminal byte 0xFE
 */
void BufferedFMF::scanQ(){
    volatile int p = inQ.first;
    volatile int n = inQ.next;
    if(!validmsg){              // don't look, if valid message is already available
	{
	    NVIC_DisableIRQ(irqn);     // critical section
	    while( (p != n) && ( (inQ.d[p] != startchar) || !isStop(p) ) ) p = (p+1)%inQ.qs; 
	    if( (p != n) && validCKS(p) ) {
		inQ.first = p;
		msgend = (p + inQ.d[(p+1) % inQ.qs] + 6) % inQ.qs; // point to position after stop
		validmsg = true;
		
	    }
	    NVIC_EnableIRQ(irqn);  // critical section
	    //if( (irqn == UART1_RX_TX_IRQn) && (inQ.first != inQ.next) ) ledR = !ledR;
	}
    }
} 

/***************************************************
 *
 *  isStop - Is there a stop byte at appropriate
 *     place for message starting at i?
 *
 ****************************************************/
bool BufferedFMF::isStop(int i){
    int  ip1, iplp5;
    ip1 = (i + 1) % inQ.qs;
    if( ip1 != inQ.next){  // still within queue?
        iplp5 = (i + inQ.d[ip1] + 5) % inQ.qs;
        if(inQ.inqueue(iplp5)){
	    return (inQ.d[iplp5] == termchar);
        }
    }
    return false;
}

/***************************************************
 *
 * ValidCKS - verifies message checksum
 *
 ****************************************************/
bool BufferedFMF::validCKS(int i){
    uint8_t s = 0;
    int k = (i + 1) % inQ.qs;                // points to length
    if( k != inQ.next){
        int ckend = (i + inQ.d[k] + 4) % inQ.qs; // points to checksum
        if( inQ.inqueue(ckend) ){
            while(k != ckend){
		s += inQ.d[k];
		k = (k + 1) % inQ.qs;
            }
            return ((s ^ 0xFF) == inQ.d[ckend]);
        }
    }
    return false;
}
 
/** hasMsg - 
 *  @return - true/false if valid msg in queue
 */
// code in header

/** getMsg - retrieve message into buffer
 *   @param msgbuf - buffer to receive message
 *   @param n - msgbuf length, if more characters are in queue, additional characters are lost
 *   @return - length of message
 */
int BufferedFMF::extractMsg(uint8_t* msgbuf, int n){
    int i = 0;
    int m = n - 1;
    if(hasMsg()){
	NVIC_DisableIRQ(irqn);  // critical section
	do{
	    msgbuf[i++] = inQ.popQ();
	} while( (inQ.first != msgend) &&  (i < m));
	//if(inQ.first == msgend) inQ.popQ();
	validmsg = false;
	NVIC_EnableIRQ(irqn);  // end critical section
	msgbuf[i] = 0; // terminate string
    } 
    return i;
}

/**
-
- putFMF - composes FMF message and starts transmission
-          if queue not empty returns 1 - failure.
*/
int BufferedFMF::putFMF(uint8_t cmd, uint8_t devID, vector<uint8_t> &data){
    int i;
    int n = data.size();
    uint8_t cksum;
    if(ux.writeable() && outQ.emptyQ()) {
	outQ.pushQ(startchar); cksum = 0;
	outQ.pushQ(n);         cksum += n;
	outQ.pushQ(devID);     cksum += devID;
	outQ.pushQ(cmd);       cksum += cmd;
	for(i=0; i<n; i++){
	    outQ.pushQ(data[i]); cksum += data[i];
	} 
	cksum ^= 0xFF;
	outQ.pushQ(cksum);
	outQ.pushQ(termchar);
	prime();
	return 0;
    }
    return 1;
}

 
