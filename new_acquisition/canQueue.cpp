/*!
 * \file canQueue.cpp
 * \class canQueue
 *
 * \author Beau Roland
 *
 * \brief Fifo queue that holds the can messages to be sent
*/

#include "mbed.h"
#include "canQueue.h"
#include "can_structs.h"

/// \warning The class file holds the instantiation of the global object. Other files access this via a global extern.
/// \brief Global instantiation for the canQueue class.
canQueue CQ;

/// \brief external accessor to print log messages to the pc console
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

