/***************************************************
 *
 * File: GPSQueue.cpp
 *
 * Purpose: Implements simple circular queue.
 *
 * Author: J. C. Wiley, Oct. 2014
 *         modified Feb. 2015 for GPS
 *
 * Uses: circular queue, on overflow overwrites
 * oldest data.
 *
****************************************************/
#include "GPSQueue.h"
extern RawSerial udebug;

#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
/** Constructor
 *
*/
GPSQueue::GPSQueue(PinName tx, PinName rx): ux(tx, rx){
    // attach interrupt
    qs = BUFFERSZ;
    next = 0;
    first= 0;
    startchar ='$';
    termchar ='\n';
   //  // irq number for given uart pin (K64F)
   //  if(tx == PTB17){
   //      irqn = UART0_RX_TX_IRQn;
   //  }
   //  else if(tx == PTC4){
   //      irqn = UART1_RX_TX_IRQn;
   //  }
   //  else if(tx == PTD3 ){
   //      irqn = UART2_RX_TX_IRQn;
   //  }
   // else if(tx == PTC17){
   //      irqn = UART3_RX_TX_IRQn;
   //  }
   //  else if(tx == PTC15 ){
   //      irqn = UART4_RX_TX_IRQn;
    }        


/** Destructor
*
*/
GPSQueue::~GPSQueue(){

}

/** readIRQ - interrupt routine for uart, read character and push in queue
*/
void GPSQueue::readIRQ(){
    pushQ(ux.getc()); // read character to clear interrupt
}

/** baud - set baud rate of contained serial port
*  @param r - the baudrate, 9600 default,  1.04 ms/char
*/
void GPSQueue::baud(int r=9600){
        ux.baud(r);
}

 /** init - initialization -
 * 
 */
int GPSQueue::init(){
    writestr(PMTK_SET_NMEA_OUTPUT_RMCGGA); // tells gps which sentences to send

    ux.attach(this, &GPSQueue::readIRQ, RawSerial::RxIrq);  // connect readIRQ to the uart read interrupt;
    return 0;
}

/** emptyQ
 *   is queue empty?
 *   @return true/false
*/
bool GPSQueue::emptyQ(){
    return (next == first);
}

/** lengthQ  
 *  @param - position in queue from start
 *  @return - number of bytes in queue, starting at i
*/
int GPSQueue::lengthQ(int i){
    int x;
    NVIC_DisableIRQ(irqn);  // critical section
    if(next >= i){ 
	x = next-i;
    }
    else{
	x = next + qs - i;
    }
    NVIC_EnableIRQ(irqn);  // critical section
    return x;
}

/** sizeQ - 
 * @return - number of bytes in queue
*/
int GPSQueue::sizeQ(){
    int x;
    NVIC_DisableIRQ(irqn);  // critical section
    if(next >= first){ 
	x = next-first;
    }
    else{
	x = next + qs - first;
    }
    NVIC_EnableIRQ(irqn);  // critical section
    return x;
}

/** freeQ - how much free space?
 *  @return number of free bytes
*/
int GPSQueue::freeQ(){
    return (qs - lengthQ(first));
}

/** clearQ - empty queue
*/
void GPSQueue::clearQ(){
    first = 0;
    next = 0;
}

/** inqueue - is i within stored data (inclusive)
 * @param true/false
*/
bool GPSQueue::inqueue(int i){
    int n = next;
    if(i < first) i += qs;
    if(n < first) n += qs;
    return( (first <= i) && (i< n));
}

/** popQ - get one byte from  queue
*
*/
uint8_t GPSQueue::popQ(void){
    uint8_t x;
    x = d[first];
    d[first] = 0;
    first = (first + 1) % qs;
    return x;
}

/** pushQ - adds one byte to queue, if overflow oldest is destroyed
*   @param x - byte to add to queue
*/
void GPSQueue::pushQ(uint8_t x){
    int n = (next + 1) % qs;
    if(n == first){  // would over flow, delete oldest
        popQ();
    }
    else{
        d[next] = x;
        next = n;
    }
}

/** scanQ - scans queue for valid message, which is defined here:
*    Here:
*        A valid message is sequence of bytes beginning with $
*        and  ending in terminal byte.
*    If no valid message found returns 0.
*    If valid message found, copies into msgbuf up to limit n,
*      if msg longer than buffer end characters are lost.
*/
int GPSQueue::scanQ(char* msgbuf, int nbuf){
    int i = 0;
    int  m;
    int profirst;

    NVIC_DisableIRQ(irqn);  // critical section
    volatile int p = first;
    volatile int n = next;
    // look for start character
    while((p != n) && (d[p] != startchar)) p = (p+1)%qs;
    if(d[p] == startchar){
	profirst = p;
	while((p != n) && (d[p] != termchar)) p = (p+1)%qs; 
	if(d[p] == termchar){ // valid message
            first = profirst;
	    msgend = p;
	    m = nbuf - 1;
	    do{
		msgbuf[i++] = popQ();
	    } while( (first != msgend) && (i < m));
	    if(first == msgend) popQ();
	    msgbuf[i] = 0; // terminate string
	}
    }
    NVIC_EnableIRQ(irqn);  // critical section
    return i;
} 
    
/** writestr - write string to serial port
*   @param pointer to string 
*  
*   @return - length of message
*/
int GPSQueue::writestr(char* s){
    int i = 0;
    int n = strlen(s);
    int n2 = n+n;
    Timer t;
    t.start();
    while( (s[i] != 0) && (i < n) && (t.read_ms() < n2) ){
	if(ux.writeable()){
	    ux.putc(s[i++]);
        }  
    }
    return i;
}
