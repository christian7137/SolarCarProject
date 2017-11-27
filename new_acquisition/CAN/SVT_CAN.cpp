/***************************************************
 *
 * File: SVT_CAN.cpp
 *
 * Purpose: Uses socketcan to communicate with onboard
 *      beagleboard CAN processor from user space.
 *
 * Author: J. C. Wiley, Nov 2012
 * REF:
 *     1. http://en.wikipedia.org/wiki/SocketCAN
 *     2. see can.tx document in www.kernel.org/doc/Documentation/networking/can.txt
 * notes:
 *     Use following command to see statistics of current interface - works on BB:
 *         ip -details -statistics link show can0
 *
****************************************************/
#include "SV_stdlibs.h"
#include "SVT_CAN.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server.h"
#include "can_structs.h"
#include "SVT_stmBuffer.h"
#include <time.h>
#include <chrono>
#include "/usr/include/linux/can/error.h"

/* At time of writing, these constants are not defined in the headers */
#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

fstream outFile;
fstream *stmFile;
fstream configFile;

int *msgname;
int msgnamec = 0;
string *fileNames;
int stms = 0;
int NUM_STM = 0;
string **sensors;
string **ids;
string ***format;
int* tokc;
STMBuffers stmbufs;

//GPS
int gathered = 0;
float Latitude;
float Longitude;



std::chrono::milliseconds uptime(0u);
int uptime_seconds;

//void SV_perror(const string& s, const int a_errno, const int err);
/**************************************************
-
- Constructor
-
*************************************************/
SVT_CAN::SVT_CAN(){
     send_ready = false;
     msg_ready = false;
}

/**************************************************
-
- Destructor
-
*************************************************/
SVT_CAN::~SVT_CAN(){

}

/***********************************************************
 *
 * init_log
 *
 * purpose: grab format from config.txt and initialze memory
 *
 ***********************************************************/

int SVT_CAN::init_log(){

	int i, j, countIdx,countIdxSense, countIdxID, initial;	
	int MAX_SENSE = 0;
	int STM_BUF_SIZE = 0;
	bool senseFlag = false;
	bool newSensor = false;
	string line;
	char *token, *subtoken, *saveptr1, *saveptr2;

	while(getline(configFile, line)){

	if(strcmp(line.c_str(),"")){
		char *dup = strdup(line.c_str());
		token = strtok_r(dup, "[", &saveptr1);

		if(!strcmp(token, "NUM_STM_IN_SYS:")){
		/* This will allocate this number of files for the number of STMs in the system */

			token = strtok_r(NULL, "]", &saveptr1);
			NUM_STM = atoi(token);

		} else if(!strcmp(token, "STM_BUFFER_SIZE:")){
		/* This will allocate this number of bytes in the buffers before they are emptied and logged */

			token = strtok_r(NULL, "]", &saveptr1);
			STM_BUF_SIZE = atoi(token);

		} else if(!strcmp(token, "MAX_NUMBER_SENSORS:")){
		/* This dictates the maximum number of total sensors */

			token = strtok_r(NULL, "]", &saveptr1);
			MAX_SENSE = atoi(token);				

		} else if(!strcmp(token,"SENSORS")){
		/* This will log all of the sensor information */
			
			senseFlag = true;
			tokc = new int [NUM_STM];
			fileNames = new string [NUM_STM];
			for(i=0; i<NUM_STM; i++){
				tokc[i] = 0;
			}
			sensors = new string* [NUM_STM];
			ids = new string* [NUM_STM];
			format = new string** [NUM_STM];
			for(i=0; i<NUM_STM; i++){
				sensors[i] = new string [MAX_SENSE];
				ids[i] = new string [MAX_SENSE];
				format[i] = new string* [MAX_SENSE];
				for(j=0; j<MAX_SENSE; j++){
					format[i][j] = new string [9];
				}
			}
		
		} else if(senseFlag && (!strcmp(token,"{"))){

			newSensor = true;

		} else if(newSensor && (!strcmp(token,"}"))){

			newSensor = false;

		} else if(newSensor){
		
			/* save all the FILE names */
			if(!strcmp(token,"FILE=")){

				token = strtok_r(NULL, "]", &saveptr1);	
				char *temp = new char[strlen(token) + 1];
				strcpy(temp, token);
				strcat(temp, ".csv");
				if(stms == 0){
					fileNames[stms] = temp;
					countIdx = 0;
					stms++;
				} else {
					for(countIdx=0; countIdx<stms; countIdx++){
						if(!strcmp(temp, fileNames[countIdx].c_str())){
							break;
						}
					}
					if(stms == countIdx){
						if(stms != NUM_STM){
							fileNames[stms] = temp;
						} else {
							cout << "ERROR: Provided more STM file names than a lotted. Increase NUM_STM_IN_SYS" << endl;
						}
					}
				}
				delete []temp;

			/* save all the sensor names */
			} else if(!strcmp(token,"NAME=")){

				token = strtok_r(NULL, "]", &saveptr1);
				if(token != NULL){
					subtoken = strtok_r(token, ",", &saveptr2);
					countIdxSense = countIdx;
					initial = tokc[countIdxSense];
					while(subtoken != NULL){
						sensors[countIdxSense][tokc[countIdxSense]] = subtoken;
						tokc[countIdxSense]++;
						subtoken = strtok_r(NULL, ",", &saveptr2);
					}
				} else {
					cout << "ERROR: Sensor does not have a name and will be ignored. This will cause errors in data." << endl;
				}

			/* save all the sensor IDs */
			} else if(!strcmp(token,"ID=")){
	
				token = strtok_r(NULL, "]", &saveptr1);
				countIdxID = tokc[countIdxSense];
				for(i=countIdx; initial<countIdxID; initial++){
					ids[i][initial] = token;
				}	
			
			/* save the message format for the can messages */
			} else if(!strcmp(token,"MSGFORMAT=")){
	
				while(token != NULL){
					token = strtok_r(NULL, "]", &saveptr1);
					subtoken = strtok_r(token, ",", &saveptr2);
					while(subtoken != NULL){
				
						for(i=0; i<tokc[countIdx]; i++){
							
							if(!strcmp(subtoken,sensors[countIdx][i].c_str())){
								break;
							}

						}
						subtoken = strtok_r(NULL, ",", &saveptr2);
						//grab the format data type
						if(subtoken != NULL){
						
							int dataIdx = 0;
							format[countIdx][i][dataIdx] = subtoken;
							dataIdx += 1;
							subtoken = strtok_r(NULL, ",", &saveptr2);
							//grab where the data is coming from
							while(subtoken != NULL){
							
								format[countIdx][i][dataIdx] = subtoken;
								dataIdx += 1;
								subtoken = strtok_r(NULL, ",", &saveptr2);
							
							}
						
						
						} else {
							cout << "ERROR: config file structure not followed." << endl;
						}
						subtoken = strtok_r(NULL, ",", &saveptr2);

					}
					token = strtok_r(NULL, "[", &saveptr1);
				}

			} 
		}
		free(dup);
	}
	}
	/* Initialize the logging files */
	stmbufs.make_buffer(NUM_STM, STM_BUF_SIZE, sensors, ids, tokc);
	stmFile = new fstream [NUM_STM];
	for(i=0; i<NUM_STM; i++){
		if(std::ifstream(fileNames[i])){
			stmFile[i].open(fileNames[i], ios::out | ios::app);
		} else {
			stmFile[i].open(fileNames[i], ios::out | ios::app);
			for(int j=0; j<stmbufs.stm[i].sensorNum; j++){
				if(j == 0){
					stmFile[i] << "Timestamp,\t" << stmbufs.stm[i].sensors[j] << ",\t";
				} else {
					stmFile[i] << stmbufs.stm[i].sensors[j] << ",\t";
				}
			}
			stmFile[i] << endl;
		}
	}
	configFile.close();

	return 0;

}

Server server;

/**************************************************
-
- init
-
*************************************************/
int SVT_CAN::init(){
   int err;

   // setup for socket can
   system("sudo ifconfig can0 down");
   system("sudo ip link set can0 type can bitrate 100000 triple-sampling off restart-ms 100");
   system("sudo ifconfig can0 up");

	// setup socket
   skt = socket( PF_CAN, SOCK_RAW, CAN_RAW );
   if(skt < 0){
		perror("socket");
		cout << "Cannot open RTDM CAN socket. Maybe driver not loaded? " <<  strerror(errno) << endl;
		return skt;
   }
    // create rdfs socket list
   FD_ZERO(&rdfs);
   FD_SET(skt, &rdfs);


   // test routines from candump
//   int rcvbuf_size = 4;
//   int curr_rcvbuf_size;
//   socklen_t curr_rcvbuf_size_len = sizeof(curr_rcvbuf_size);
//   if(setsockopt(skt, SOL_SOCKET, SO_RCVBUFFORCE, &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {
//		cout<< "setsockopt failed" << endl;
//	}
//   cout << "rcvbuf_size " << rcvbuf_size << endl;

   // set socket to no loopback,   loopback is default
   int loopback = 0; /* 0 = disabled, 1 = enabled (default) */
   if( setsockopt(skt, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback)) < 0){
	   cout << "setsockopt failed, loopback failed" << endl;
   }

   // set socket to accept error messages
   can_err_mask_t err_mask = ( CAN_ERR_TX_TIMEOUT | CAN_ERR_BUSOFF );
   setsockopt(skt, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask));


   // set filters for read socket
#undef CANFILTER
#ifdef CANFILTER
   struct can_filter rfilter[5];
   // mask logic:   (rec id) & mask == (can_id) & mask  to match
   rfilter[0].can_id   = 0x400;  // processor with can_id of 3   TPMS
   rfilter[0].can_mask = 0x700;  // pass processor id only, bits 9-11, any sensor or message bits 0-7
   rfilter[1].can_id   = 0x300;  // processor with can_id of 4
   rfilter[1].can_mask = 0x700;
   rfilter[2].can_id   = 0x100;  // green led
   rfilter[2].can_mask = 0x700;
   rfilter[3].can_id   = 0x500;  // mppt
   rfilter[3].can_mask = 0x700;
   rfilter[4].can_id   = 0x600;  // imu
   rfilter[4].can_mask = 0x700;

   if( setsockopt(skt, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)) < 0){
	   cout << "setsockopt failed, set filter failed" << endl;
   }
#endif

   /* Locate the interface you wish to use */
   struct ifreq ifr;
   strcpy(ifr.ifr_name, "can0");
   if( (err = ioctl(skt, SIOCGIFINDEX, &ifr)) ){ // ifr.ifr_ifindex gets filled
        cout << "SVT_CAN::init: ioctl failed to find socket: " <<  strerror(errno) << " " << err << endl;  // with that device's index
        return(-1);
   }
   /* Select that CAN interface, and bind the socket to it. */
   addr.can_family = AF_CAN;
   addr.can_ifindex = ifr.ifr_ifindex;
   bind( skt, (struct sockaddr*)&addr, sizeof(addr));

   // set up msghdr for receive call
   RxMsg.msg_name = &addr;
   RxMsg.msg_iov = &iov;
   RxMsg.msg_iovlen = 1;
   RxMsg.msg_control = &ctrlmsg;

   // set up msghdr for send call
   TxMsg.msg_name = &addr;
   TxMsg.msg_iov = &TxIov;
   TxMsg.msg_iovlen = 1;
   TxMsg.msg_control = &ctrlmsg;

   configFile.open("config.txt", ios::in);

   init_log();
   
   cout << "output file(s) made\n";   

   return 0;
}

/**************************************************
 *
 *   InternalThreadEntry - thread inner loop
 *      use blocking read
 *
 **************************************************/
void SVT_CAN::InternalThreadEntry(){
	int ret;
	while(1){
		if ((ret = readmsg(rxframe))) { // if 0, no message available
			if (ret < 0) {
				cout << "readmsg error " << ret << endl;
				// WaitForInternalThreadToExit();
			}
			else if (ret > 0) { // print message
				print_canframe("rx ", rxframe);
			}
		}
	}
}

/**************************************************
   -
   - Update  -
   -
 *************************************************/
int SVT_CAN::Update(){
	int err;

	// test for incoming messages
	if((err = handleInput()) != 0){
		SV_perror("SVT_CAN::Update incoming error", 0, err);
		return -1;
	}
	// if message ready to send and if port ready to send
	if((err = handleOutput()) != 0){
		SV_perror("SVT_CAN::Update output error", 0, err);
		return -1;
	}
	return 0;
}

/**************************************************
   -
   - handleInput
   -
 *************************************************/
int SVT_CAN::handleInput(void){
	int ret;

	if( !msg_ready && (ret = readmsg(rxframe)) ){ // if 0, no message available
		if(ret < 0){
			SV_perror("readmsg error ", 0, ret);
			return (-1);
		}
		else if(ret > 0){ // set message ready flag
			msg_ready = true;
		}
	}
	return 0;
}

/**************************************************
   -
   - handleOutput
   -
 *************************************************/
int SVT_CAN::handleOutput(void){
	int err;
	if(send_ready){
		if( (err = send(txframe)) < 0){
			SV_perror("SVT_CAN::handleOutput ", 0, err);
			return err;
		}
	}
	send_ready = false; // msg send, buffer free
	return 0;
}

/**************************************************
   -
   - getCANMSG
   -
 *************************************************/
int SVT_CAN::getCANMSG(uint32_t &id, uint8_t &dlc, vector<uint8_t> &data){
	id = rxframe.can_id;
	dlc = rxframe.can_dlc;
	data.clear();
	for(int i=0; i< dlc; i++){
		data.push_back( rxframe.data[i]);
	}
	msg_ready = false; // message processed, rxframe free
	return 0;
}

/**************************************************
   -
   - putCANMSG
   -
 *************************************************/
int SVT_CAN::putCANMSG(uint32_t id, vector<uint8_t> &data){
	txframe.can_id = id;
	txframe.can_dlc = data.size();
	for(int i=0; i < txframe.can_dlc; i++){
		txframe.data[i] = data[i];
	}
    send_ready = true;
	return 0;
}

/**************************************************
   -
   - Send  - Send a message to the CAN bus
   -    return 0 if nothing sent
   -           n bytes sent
   -           -1 error
   -
 *************************************************/
int SVT_CAN::send(struct can_frame& frame){
	int ret;
	int err;

	/* Send a message to the CAN bus */
	TxIov.iov_base = &frame;
	// these settings may be modified by sendmsg(), so reset each call
	TxIov.iov_len = sizeof(frame);
	TxMsg.msg_namelen = sizeof(addr);
	TxMsg.msg_controllen = sizeof(ctrlmsg);
	TxMsg.msg_flags = 0;

//	int bytes_in_queue;
//	if(ioctl(skt, TIOCOUTQ, &bytes_in_queue) == -1){
//		cout << "ioctl failed " << strerror(errno) << endl;
//	}
//	cout << "bytes in queue " << bytes_in_queue << endl;

	if( (ret = sendmsg(skt, &TxMsg, MSG_DONTWAIT)) == -1){
		err = errno;
		SV_perror("SVT_CAN::send ", err, err);
		switch(err){
			case EAGAIN:
			   cout << " The socket is marked nonblocking and the requested operation would block. POSIX.1-2001 allows either error to be returned for this case, and does not require these constants to have the same value, so a portable application should check for both possibilities." << endl;
			   return 0;
			   break;
			case EBADF:
				cout << "An invalid descriptor was specified." << endl;
				break;
			case ECONNRESET:
				cout << "A Connection reset by peer. " << endl;
				break;
			case EDESTADDRREQ:
				cout << "The socket is not connection-mode, and no peer address is set."  << endl;
			    break;
			case EFAULT:
			    cout << "An invalid user space address was specified for an argument." << endl;
	            break;
			case EINTR:
				cout << "A signal occurred before any data was transmitted; see signal(7)." << endl;
				break;
			case EINVAL:
				cout << "Invalid argument passed." << endl;
				break;
			case EISCONN:
				cout << " The connection-mode socket was connected already but a recipient was specified. (Now either this error is returned, or the recipient specification is ignored.)"  << endl;
				break;
			case EMSGSIZE:
				cout << "The socket type requires that message be sent atomically, and the size of the message to be sent made this impossible. " << endl;
				break;
			case ENOBUFS:
				cout << " The output queue for a network interface was full. This generally indicates that the interface has stopped sending, but may be caused by transient congestion. (Normally, this does not occur in Linux. Packets are just silently dropped when a device queue overflows.)"  << endl;
				return -ENOBUFS;
				break;
			case ENOMEM:
				cout << " out of memory" << endl;
				break;
			default:
				cout << "other error" << endl;
				break;
		}
		return 0;
	}
	else return ret;
}

/**************************************************
   -
   - Read - a message from the CAN bus using recvmsg set for non blocking.
   -        Setting the socket as non blocking did not work.
   -
   -    returns 0 if no message
   -            1 for message
   -           -1 error
   -
 *************************************************/
int SVT_CAN::readmsg(struct can_frame& frame){
    unsigned int ret;
    int err;

	iov.iov_base = &frame;
	// these settings may be modified by recvmsg(), so reset each call
	iov.iov_len = sizeof(frame);
	RxMsg.msg_namelen = sizeof(addr);
	RxMsg.msg_controllen = sizeof(ctrlmsg);
	RxMsg.msg_flags = 0;

	if( (signed) (ret = recvmsg(skt, &RxMsg, MSG_DONTWAIT)) == -1){
		err = errno;
		switch(err){
			case EAGAIN: //or EWOULDBLOCK
				return 0;  // set NON_BLOCK
				cout << "The socket's file descriptor is marked O_NONBLOCK and no data is waiting to be received; or MSG_OOB is set and no out-of-band data is available and either the socket's file descriptor is marked O_NONBLOCK or the socket does not support blocking to await out-of-band data.  " << endl;
				break;
			case EBADF:
				cout << "The socket argument is not a valid open file descriptor." << endl;
				break;
			case ECONNRESET:
				cout << "A connection was forcibly closed by a peer. " << endl;
				break;
			case EINTR:
				cout << "This function was interrupted by a signal before any data was available. " << endl;
				break;
			case EINVAL:
				cout << "The sum of the iov_len values overflows a ssize_t, or the MSG_OOB flag is set and no out-of-band data is available." << endl;
				break;
			case EMSGSIZE:
				cout << "The msg_iovlen member of the msghdr structure pointed to by message is less than or equal to 0, or is greater than {IOV_MAX}.  " << endl;
				break;
			case ENOTCONN:
				cout << "A receive is attempted on a connection-mode socket that is not connected.  " << endl;
				break;
			case ENOTSOCK:
				cout << "The socket argument does not refer to a socket. " << endl;
				break;
			case EOPNOTSUPP:
				cout << "The specified flags are not supported for this socket type." << endl;
				break;
			case ETIMEDOUT:
				cout << "The connection timed out during connection establishment, or due to a transmission timeout on active connection. " << endl;
				break;
			///The recvmsg() function may fail if:
			case EIO:
				cout << "An I/O error occurred while reading from or writing to the file system." << endl;
				break;
			case ENOBUFS:
				cout << "Insufficient resources were available in the system to perform the operation." << endl;
				break;
			case ENOMEM:
				cout << "Insufficient memory was available to fulfill the request." << endl;
				break;
			default:
				break;

		}
		return err;
	}
	err = RxMsg.msg_flags;
	switch(err){
		case MSG_EOR:
			 cout << "msg_flag: indicates end-of-record; the data returned completed a record (generally used with sockets of type SOCK_SEQPACKET). " << endl;
			 return err;
			 break;
		case MSG_TRUNC:
			 cout << "msg_flag: indicates that the trailing portion of a datagram was discarded because the datagram was larger than the buffer supplied. " << endl;
			 return err;
			 break;
		case MSG_CTRUNC:
			 cout << "msg_flag: indicates that some control data were discarded due to lack of space in the buffer for ancillary data. " << endl;
			 return err;
			 break;
		case MSG_OOB:
			 cout << "msg_flag: is returned to indicate that expedited or out-of-band data were received. " << endl;
			 return err;
			 break;
		case MSG_ERRQUEUE:
			 cout << "msg_flag: indicates that no data was received but an extended error from the socket error queue. " << endl;
			 return err;
			 break;
		default:
			 break;
	}
	if( ret < sizeof(struct can_frame ) ) {
		cout << "SVT_CAN::readmsg: incomplete CAN frame " << ret << endl;
		return -1;
	}
	return 1; // have message
}

/**************************************************
   -
   - parse_canframe_struct
   -
 *************************************************/
void SVT_CAN::parse_canframe_struct(uint8_t * pData, stringstream& buf){
	//HOW DO I TELL NUMS ARE CERTAIN TYPES OF SENSORS?
	CAN_MSG * pParsed;
	pParsed = (CAN_MSG * )pData;
	float Location[2];
	
	switch(pParsed->payload.type)
	{
	
		case(5):{
			if(gathered == 0){
				gathered += 1;
				Longitude = pParsed->payload.data.lon.lon;
			} else if(gathered == 1){
				Longitude = pParsed->payload.data.lon.lon;
			} else if(gathered == 2){
				Longitude = pParsed->payload.data.lon.lon;
				gathered = 0;
				Location[0] = Latitude;
				Location[1] = Longitude;
				server.json_message.setGPSSensor((time(0) + 21600), 4, Location);
				server.json_message.printJson();
				server.sendPacket();
			}
		} break;
		case(4):{
			if(gathered == 0){
				gathered += 2;
				Latitude = pParsed->payload.data.lat.lat;
			} else if(gathered == 1){
				Latitude = pParsed->payload.data.lat.lat;
				gathered = 0;
				Location[0] = Latitude;
				Location[1] = Longitude;
				server.json_message.setGPSSensor((time(0) + 21600), 4, Location);
				server.json_message.printJson();
				server.sendPacket();
			} else if(gathered == 2){
				Latitude = pParsed->payload.data.lat.lat;
			}
			
		} break;
		case(3):{
			//buf << pParsed->payload.data.type3.sensor1Data; 
			server.json_message.setLumSensor((time(0) + 21600), pParsed->payload.type, pParsed->payload.data.light.lumens);
			server.json_message.printJson();
			server.sendPacket();
		}break;
		case(2):{
			//buf << pParsed->payload.data.type2.accX << " " << pParsed->payload.data.type2.accY << " " << pParsed->payload.data.type2.accZ;
			//setOriSensor(int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]){
			int oriAng[2] = {0,0};
			int16_t inAcc[3] = {pParsed->payload.data.acc.accX, pParsed->payload.data.acc.accY, pParsed->payload.data.acc.accZ};
			int16_t inGyr[3] = {0,0,0};
			int16_t inMag[3] = {0,0,0};  
			server.json_message.setOriSensor((time(0) + 21600), pParsed->payload.type, oriAng, inAcc, inGyr, inMag);
			server.json_message.printJson();
			server.sendPacket();
		}break;
	}
}

/**************************************************
   -
   - store_canframe
   -
 *************************************************/
void SVT_CAN::store_canframe(struct can_frame& cf){
	stringstream buf;
	//int dlc = (cf.can_dlc > 8)? 8 : cf.can_dlc;
	int type;

	if(cf.can_id & CAN_RTR_FLAG) { /* there are no ERR frames with RTR */
		buf << "R";
	} else {
		type = int(cf.data[0]);
		buf << uptime.count();
		buf << ",\t";
		buf << type;
		buf << ",\t";
		parse_canframe_struct(cf.data,buf);
	}

	buf << endl;
	int i, j;
	for(i=0; i<NUM_STM; i++){

		for(j=0; j<stmbufs.stm[i].sensorNum; j++){

			if(atoi(stmbufs.stm[i].ids[j].c_str()) == type){
				stmbufs.stm[i].ts[stmbufs.stm[i].bufIdx] = (time(0));
				for(int k=0; k<8; k++){
					stmbufs.stm[i].msgData[stmbufs.stm[i].bufIdx][k] = int(cf.data[k]);
				}
				stmbufs.stm[i].bufIdx += 1;
				if(stmbufs.stm[i].bufIdx == stmbufs.stm[i].size){
					stmbufs.stm[i].bufIdx = 0;
					//DO I WANT TO DO THE PARSED STRING INSTEAD?
					store_canBuffer(i, stmbufs.stm[i].size, stmbufs.stm[i].msgData);
				}
				i = NUM_STM + 1;
				break;
			}

		}

	}
    	outFile << buf.str();
    	outFile.flush();

}

/*****************************************************
 *
 *  store_canBuffer
 *
 *  purpose: analyzes the canMsg and logs it based on format[][]
 *
 *****************************************************/

void SVT_CAN::store_canBuffer(int stmNum, int size, vector<vector<uint8_t>> Msg){

	vector<vector<int>> data;
	vector<vector<float>> GPS;
	data.resize(size);
	GPS.resize(size);
	int i, j, k, n;
	int oldts = stmbufs.stm[stmNum].ts[0];
	int type = 0;
	for(k=0; k<size; k++){
		data[k].resize(1);
		data[k][0] = (int) NULL;
		GPS[k].resize(1);
		GPS[k][0] = (int) NULL;
	}

	/* after the buffer is full log the saved messages */
	for(i=0; i<size; i++){	
		type = Msg[i][0];
		for(n=0; n<tokc[stmNum]; n++){
			if(type == atoi(ids[stmNum][n].c_str())){
				break;
			}
		}
		if(!strcmp(format[stmNum][n][0].c_str(),"int8_t")){
			
			for(k=0; k<tokc[stmNum]; k++){
				if(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
					break;
				}
			}
			while(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
				int8_t temp;
				data[k].resize(2);
				data[k][0] = 1;
				temp = Msg[i][atoi(format[stmNum][n][1].c_str())];
				data[k][1] = (int) temp;
				k++;
				n++;
			}
	
		} else if(!strcmp(format[stmNum][n][0].c_str(),"uint8_t")){

			for(k=0; k<tokc[stmNum]; k++){
				if(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
					break;
				}
			}
			while(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
				uint8_t temp;
				data[k].resize(2);
				data[k][0] = 1;
				temp = Msg[i][atoi(format[stmNum][n][1].c_str())];
				data[k][1] = (unsigned int) temp;
				k++;
				n++;
			}
				
		} else if(!strcmp(format[stmNum][n][0].c_str(),"int16_t")){
			
			for(k=0; k<tokc[stmNum]; k++){
				if(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
					break;
				}
			}
			while(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
				int16_t temp;
				data[k].resize(2);
				data[k][0] = 1;
				temp = Msg[i][atoi(format[stmNum][n][2].c_str())];
				temp += Msg[i][atoi(format[stmNum][n][1].c_str())] << 8;
				data[k][1] = (int) temp;
				k++;
				n++;			
			}

		} else if(!strcmp(format[stmNum][n][0].c_str(),"uint16_t")){
			
			for(k=0; k<tokc[stmNum]; k++){
				if(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
					break;
				}
			}	
			while(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
				uint16_t temp;
				data[k].resize(2);
				data[k][0] = 1;
				temp = Msg[i][atoi(format[stmNum][n][2].c_str())];
				temp += Msg[i][atoi(format[stmNum][n][1].c_str())] << 8;
				data[k][1] = (unsigned int) temp;
				k++;
				n++;			
			}
				
		} else if(!strcmp(format[stmNum][n][0].c_str(),"int32_t")){
				
			for(k=0; k<tokc[stmNum]; k++){
				if(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
					break;
				}
			}
			while(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
				int32_t temp;
				data[k].resize(2);
				data[k][0] = 1;
				temp = Msg[i][atoi(format[stmNum][n][4].c_str())];
				temp += Msg[i][atoi(format[stmNum][n][3].c_str())] << 8;
				temp += Msg[i][atoi(format[stmNum][n][2].c_str())] << 16;
				temp += Msg[i][atoi(format[stmNum][n][1].c_str())] << 24;
				data[k][1] = (int) temp;
				k++;
				n++;			
			}
		
		} else if(!strcmp(format[stmNum][n][0].c_str(),"uint32_t")){
			
			for(k=0; k<tokc[stmNum]; k++){
				if(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
					break;
				}
			}
			while(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
				uint32_t temp;
				data[k].resize(2);
				data[k][0] = 1;
				temp = Msg[i][atoi(format[stmNum][n][4].c_str())];
				temp += Msg[i][atoi(format[stmNum][n][3].c_str())] << 8;
				temp += Msg[i][atoi(format[stmNum][n][2].c_str())] << 16;
				temp += Msg[i][atoi(format[stmNum][n][1].c_str())] << 24;
				data[k][1] = (unsigned int) temp;
				k++;
				n++;			
			}			
	
		} else if(!strcmp(format[stmNum][n][0].c_str(),"float")){

			for(k=0; k<tokc[stmNum]; k++){
				if(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){
					break;
				}
			}
			while(type == atoi(stmbufs.stm[stmNum].ids[k].c_str())){			
				CAN_MSG * pParsed;
				uint8_t pData[8];
				for(int z=0; z<8; z++){
					pData[z] = Msg[i][z];
				}
				pParsed = (CAN_MSG * )pData;
				float temp = pParsed->payload.data.lat.lat;
				data[k][0] = 2;
				GPS[k].resize(2);
				GPS[k][0] = 2;
				GPS[k][1] = temp;
				k++;
				n++;			
			}

		}
		if(oldts == stmbufs.stm[stmNum].ts[i]){
			oldts = stmbufs.stm[stmNum].ts[i];
		} else {
			stmFile[stmNum] << oldts << ",\t";
			for(j=0; j<tokc[stmNum]; j++){
				if(data[j][0] == 1){
					stmFile[stmNum] << data[j][1];
				} else if(data[j][0] == 2){
					stmFile[stmNum] << GPS[j][1];
				}
				stmFile[stmNum] << ",\t";
			}
			oldts = stmbufs.stm[stmNum].ts[i];
			stmFile[stmNum] << endl;
			stmFile[stmNum].flush();
			for(k=0; k<size; k++){
				data[k].resize(1);
				data[k][0] = (int) NULL;
			}
		}
		

	}	

}

/**************************************************
   -
   - print_canframe
   -
 *************************************************/

//Modify this to get the data from the rxframe
void SVT_CAN::print_canframe(string header, struct can_frame& cf){
	
	store_canframe(cf);	
	
	/*stringstream buf;
	int dlc = (cf.can_dlc > 8)? 8 : cf.can_dlc;
	
	buf << header;

	if(cf.can_id & CAN_ERR_FLAG) {
		buf <<  hex << int(cf.can_id & (CAN_ERR_MASK|CAN_ERR_FLAG));
	} else if (cf.can_id & CAN_EFF_FLAG) {
		buf << hex << int(cf.can_id & CAN_EFF_MASK);
	} else {
		buf << hex <<  int(cf.can_id & CAN_SFF_MASK);
	}
	buf << " ";
	

	if(cf.can_id & CAN_RTR_FLAG) // there are no ERR frames with RTR 
		buf << "R";
	else
		for(int i = 0; i < dlc; i++) {
			buf <<  hex << int(cf.data[i]);
			if((i+1 < dlc))
				buf <<  ".";
		}
	buf << endl;
    	cout << buf.str();*/
	
}

