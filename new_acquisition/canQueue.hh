/*!
 * \file canQueue.hh
 * \class canQueue
 * \brief Defines FIFO class that holds the outgoing CAN Messages.
 * We may extend this to incorporate more storage elements to handle incoming messages off
 * CAN Bus.
 *
 * \author Beau Roland
*/

#ifndef CAN_QUEUE_HH
#define CAN_QUEUE_HH

#include <queue>
#include "can_structs.hh"

class canQueue
{
    public: /* public member objects */
    
    private:/* private member objects */
    std::queue<CAN_MSG> fifo;
    
    public:
		/// Constructor
        canQueue();
		/// Destructor
        ~canQueue();
        
	public: /* public functions */
		void push(CAN_MSG msg);
		CAN_MSG getNextCanMsg(void);
		bool queueEmpty(void);
		
    private: /* private functions */

};

#endif


