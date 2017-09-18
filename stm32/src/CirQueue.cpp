/***************************************************
 *
 * File: CirQueue.cpp
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
#include "CirQueue.h"

extern RawSerial udebug;

/** Constructor
 *
*/
CirQueue::CirQueue(int sz){
    // attach interrupt
    qs = sz;
    next = 0;
    first= 0; 
    d.resize(qs);      
}

/** Destructor
*
*/
CirQueue::~CirQueue(){

}

/** emptyQ
 *   is queue empty?
 *   @return true/false
*/
bool CirQueue::emptyQ(){
    return (next == first);
}

/** lengthQ  
 *  @param - index of first data byte in queue
 *  @return - number of bytes in queue, starting at i
*/
int CirQueue::lengthQ(int i){
    if(next >= i) return(next - i);
    return(next + qs - i);
}

/** sizeQ - 
 * @return - number of bytes in queue
*/
int CirQueue::sizeQ(){
    return lengthQ(first);
}

/** freeQ - how much free space?
 *  @return number of free bytes
*/
int CirQueue::freeQ(){
    return (qs - lengthQ(first));
}

/** clearQ - empty queue
*/
void CirQueue::clearQ(){
    first = 0;
    next = 0;
}

/** inqueue - is i within stored data (inclusive)
 * @param true/false
*/
bool CirQueue::inqueue(int i){
    int n = next;
    if(i < first) i += qs;
    if(n < first) n += qs;
    return( (first <= i) && (i< n));
}

/** popQ - get one byte from  queue
*
*/
uint8_t CirQueue::popQ(void){
    uint8_t x;
    x = d[first];
    first = (first + 1) % qs;
    return x;
}

/** pushQ - adds one byte to queue, if overflow oldest is destroyed
*   @param x - byte to add to queue
*/
void CirQueue::pushQ(uint8_t x){
    int n = (next + 1) % qs;
    if(n == first){  // would over flow, delete oldest
        popQ();
    }
    else{
        d[next] = x;
        next = n;
    }
}

/** dump
 *
 */
void CirQueue::dump(){
    int i;
    i = first;
    while( i != next){
	udebug.printf("%c", d[i]);
        i = (i+1)%qs;
    }
    udebug.printf("\r\n");
}


