#ifndef SVCAN_H_
#define SVCAN_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "SV_error.h"
#include "SV_Thread.h"


//struct can_frame can_frame_t;
/**
 * CAN class with included thread.
 */
class SVT_CAN: public SV_Thread {
	public:
		enum Sensors {
			LIGHT,
			ORIENTATION,
			GPS,
			CHARGE	
		};

		SVT_CAN();
		~SVT_CAN();
		int init();
		int init_log();
		int Update();
		bool msgReady(void){return msg_ready;}
		bool sendReady(void){return send_ready;}
		int getCANMSG(uint32_t &id, uint8_t &dlc, vector<uint8_t> &data);
		int putCANMSG(uint32_t id,  vector<uint8_t> &data);
		int readmsg(struct can_frame& frame);
		int send(struct can_frame&);
		int parse_canframe(struct can_frame& cf, int sensor, stringstream& buf, int idx);
		void parse_canframe_struct(uint8_t * pData, stringstream& buf);
		void store_canframe(struct can_frame& cf);
		void store_canBuffer(int stmNum, int size, vector<vector<uint8_t>> Msg);
		void print_canframe(string header, struct can_frame& cf);
		void InternalThreadEntry();
		void UDP_send(int time, int ID, int val);
	private:
		int handleInput(void);
		int handleOutput(void);
		int skt;                 //socket
		fd_set rdfs;
		struct timeval timeout;
	    char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
	    struct iovec iov;
	    struct msghdr RxMsg;
	    struct msghdr TxMsg;
	    struct iovec TxIov;
	    struct sockaddr_can addr;
	    bool msg_ready; // Flag to denote message in input buffer ready to read
	    bool send_ready; // Flag to denote message in output buffer ready to send

		can_frame txframe;
		can_frame rxframe;
};
#endif
