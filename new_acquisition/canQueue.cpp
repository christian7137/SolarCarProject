/*!
 * \file canQueue.cpp
 * \class canQueue
 *
 * \author Beau Roland
*/

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

/*!
 *	\brief Public function that pushes a can message into the CAN Message FIFO
 *	\param The CAN Message to be pushed on the stack
 */
void canQueue::push( CAN_MSG msg )
{
    fifo.push(msg);
}

/*!
 *	\brief Public function that pulls and removes the last can message from the FIFO Queue
 *	\return The last CAN Message in the FIFO Queue
 */
CAN_MSG canQueue::getNextCanMsg( void )
{
	CAN_MSG nextMsg;
	nextMsg = fifo.front();
	fifo.pop();
	return nextMsg;
}
/*!
 *	\brief Public assessor function that tells whether there are any CAN Messages in the FIFO
 *	\return boolean describing whether the FIFO is empty
 */
bool canQueue::queueEmpty(void)
{
	return fifo.empty();
}


