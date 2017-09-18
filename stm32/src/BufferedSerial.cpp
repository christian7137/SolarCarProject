/**
 *
 * File: BufferedSerial.cpp
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
#include "BufferedSerial.h"

extern RawSerial udebug;
extern DigitalOut ledR;

void BufferedSerial::sendA(){
    ux.putc('A');
}

/** Destructor
 *
 */
BufferedSerial::~BufferedSerial(){

}

/** baud - set baud rate of contained serial port
 *  @param r - the baudrate, 9600 default
 */
void BufferedSerial::baud(int r){
    ux.baud(r);
}

// /** readIRQ - interrupt routine for uart, read character and push in queue
//  */
// void BufferedSerial::readIRQ(){
//     inQ.pushQ( ux.getc()); // read character to clear interrupt
// }

// /** sendIRQ -Tx interrupt routine for uart, write character to uart
//  */
// void BufferedSerial::sendIRQ(){
//     while( ux.writeable() && !outQ.emptyQ() ) {
// 	ux.putc(outQ.popQ());
//     } 
//     if(outQ.emptyQ()) ux.attach(NULL, RawSerial::TxIrq);
// }

/** prime -  starts serial transmition with interrupts - see Sam Grove's BufferedSerial example @ mbed
 *
 */
void BufferedSerial::prime(void){
    if( ux.writeable() ){
	ux.attach(NULL, RawSerial::TxIrq);
	sendIRQ();
	ux.attach(this, &BufferedSerial::sendIRQ, RawSerial::TxIrq);
    }
}

/** scanQ - scans queue for valid message, which is defined here:
 *  
 *    A valid message: is sequence of bytes starting with 0x01 ending in terminal byte 0xFE
 */
void BufferedSerial::scanQ(){

} 

/** hasMsg - 
 *  @return - true/false if valid msg in queue
 */
// code in header  



 
