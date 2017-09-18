/**
 *
 * File: BufferedNGM.cpp
 *
 * Purpose: Uses circular queues to buffer serial I/O
 *
 * Author: J. C. Wiley, Mar. 2015
 *          
 * Uses: circular queue, on overflow overwrites
 * oldest data.
 *
 */
#include "BufferedNGM.h"

extern RawSerial udebug;
extern DigitalOut ledR;

/** Destructor
 *
 */
BufferedNGM::~BufferedNGM(){
    ux.attach(NULL, RawSerial::RxIrq);  // connect readIRQ to the uart read interrupt;
    ux.attach(NULL, RawSerial::TxIrq);
}

/** readIRQ - interrupt routine for uart, read character and push in queue
 */
void BufferedNGM::readIRQ(){
    uint8_t c;

    NVIC_DisableIRQ(irqn);     // critical section
    c = ux.getc(); // read character to clear interrupt
    if((c >= NGM_HT) && (c <= 0x90)){  // is valid ASCII char?
	if( c != NGM_CR && c != NGM_LF){ // normal character
	    inQ.pushQ(c);
	    lastchar = c; 
	} 
	else{  // control character
	    if(lastchar != NGM_CR && lastchar != NGM_LF){ // first cc? otherwise skip
		inQ.pushQ(NGM_CR);
		inQ.pushQ(NGM_LF);
		lastchar = NGM_LF;             
	    } 
	}          
    }
    NVIC_EnableIRQ(irqn);  // critical section
}

/** sendIRQ -Tx interrupt routine for uart, write character to uart
 */
void BufferedNGM::sendIRQ(){
    while( ux.writeable() && !outQ.emptyQ() ) {
	ux.putc(outQ.popQ());
    }
    if(outQ.emptyQ()) ux.attach(NULL, RawSerial::TxIrq);
}

/** scanQ - scans queue for valid message, which is defined here:
 *  
 *    A valid message: is sequence of bytes starting with and ending with LF or CR 
 */
void BufferedNGM::scanQ(){
    // udebug.printf("Qs %d\r\n", inQ.sizeQ());
    // if(inQ.sizeQ() > 10){
    // 	inQ.dump();
    // }
    NVIC_DisableIRQ(irqn);     // critical section
    volatile int p = inQ.first;
    volatile int n = inQ.next;
    int i = p;
    if(i != n) i = (i + 1) % inQ.qs;
    if(i != n) i = (i + 1) % inQ.qs;
    // if(i != n) i = (i + 1) % inQ.qs;
    // if(i != n) i = (i + 1) % inQ.qs;  // if msg in queue, should point to CR.

    if( !validmsg && inQ.sizeQ() >= 2 ){ // only look if don't already have valid message
        while( (i != n) && ( inQ.d[i] != NGM_CR) && ( inQ.d[(i+1)%inQ.qs] != NGM_LF) ) i = (i + 1) %  inQ.qs;
        if( ((i != n) && ( inQ.d[i] == NGM_CR) && ( inQ.d[(i+1)%inQ.qs] == NGM_LF))  ) {
            msgend = i; // point to CR
            validmsg = true;
        }
    }
    NVIC_EnableIRQ(irqn);  // critical section
}
 
/** hasMsg - 
 *  @return - true/false if valid msg in queue
 */
// code in header

/** extractMsg - retrieve message into buffer
 *   @param msgbuf - buffer to receive message
 *   @param n - msgbuf length, if more characters are in queue, additional characters are lost
 *   @return - length of message,  should end in CR/LF but these should be stripped.
 */
int BufferedNGM::extractMsg(uint8_t* msgbuf, int n){
    int i = 0;
    int m = n - 1;
    if(hasMsg()){
	NVIC_DisableIRQ(irqn);  // critical section
	do{
	    msgbuf[i++] = inQ.popQ();
	} while( (inQ.first != msgend) &&  (i < m));
	if(inQ.first == msgend) {
	    inQ.popQ(); //cr
	    inQ.popQ(); //lf
	}
	NVIC_EnableIRQ(irqn);  // end critical section
	msgbuf[i] = 0; // terminate string
	validmsg = false;
    } 
    return i;
}

/**
-
- putNGM- sends message
*/
int BufferedNGM::putNGM(string& s){
    int n = s.length();
    
    if(ux.writeable() && outQ.emptyQ()) {
	for(int i=0; i<n; i++){
	    outQ.pushQ(s[i]);
	} 
	outQ.pushQ(NGM_CR);
	outQ.pushQ(NGM_LF);
	prime();
	return 0;
    }
    return 1;
}

/**
-
- putNGM - composes NGM message and starts transmission
-          if queue not empty returns 1 - failure.
*/
int BufferedNGM::putNGM(const char *s){
    int i;
    int n = strlen(s);
    
    if(ux.writeable() && outQ.emptyQ()) {
        for(i=0; i<n; i++){
	    outQ.pushQ(s[i]);
	} 
	outQ.pushQ(NGM_CR);
	outQ.pushQ(NGM_LF);
	prime();
	return 0;
    }
    return 1;
}

 
