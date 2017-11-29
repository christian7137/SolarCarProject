/*!*************************************************************
	\file  classQueue.h
	\class canQueue
	
	\brief Defines FIFO class that holds the outgoing CAN Messages.
	We may extend this to incorporate more storage elements to handle incoming messages off
	CAN Bus.
	
	\author Beau Roland, Oct. 2017
***************************************************************/
#ifndef CAN_QUEUE_HH
#define CAN_QUEUE_HH

#include <queue>
#include "can_structs.h"

class canQueue
{
    public: /* public member objects */
    
    private:/* private member objects */
    std::queue<CAN_MSG> fifo;	///< \brief can fifo queue that stores can messages to be sent
    
    public:
		/// Constructor
        canQueue();
		/// Destructor
        ~canQueue();
        
	public: /* public functions */
		void push(CAN_MSG msg);			///< \brief function that pushes a can message onto the fifo stack
		CAN_MSG getNextCanMsg(void);	///< \brief function that returns a list a can messages that dictate the current state
		bool queueEmpty(void);			///< \brief helper function that returns whether the fifo queue is empty
		
    private: /* private functions */

};

#endif