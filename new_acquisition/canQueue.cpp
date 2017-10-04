#include "mbed.h"
#include "canQueue.hh"
#include "can_structs.hh"

canQueue CQ;
extern Serial pc;

canQueue::canQueue()
{
}
canQueue::~canQueue()
{
}

void canQueue::push( CAN_MSG msg )
{
    fifo.push(msg);
}

CAN_MSG canQueue::getNextCanMsg( void )
{
	CAN_MSG nextMsg;
	nextMsg = fifo.front();
	fifo.pop();
	return nextMsg;
}

bool canQueue::queueEmpty(void)
{
	return fifo.empty();
}


