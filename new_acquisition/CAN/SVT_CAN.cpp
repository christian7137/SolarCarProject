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
int* tokc;
STMBuffers stmbufs;



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

/*class Server {
	private:
		int sock, possError,port; 
        unsigned int length;
        struct sockaddr_in serverAddr; 
    
		void error(const char *msg){
			perror(msg);
			exit(0);
		}

        void connectSock(void){
            if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                 error("socket");
            }
        }
		
	public: 
		Json_Message json_message;
		
        Server(){
            port = 1500;    
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = inet_addr("192.168.0.101");//127.0.0.1, 192.168.0.255, 169.254.255.255 <- this one computer to RPi
            serverAddr.sin_port = htons(port);//atoi(argv[2])
            length=sizeof(struct sockaddr_in);
			connectSock();
		}  
        
        void sendPacket(){
			possError = sendto(sock,(struct Json_Message*)&json_message.all_json, sizeof(json_message.all_json),0,(const struct sockaddr *)&serverAddr,length);
        }
        
        void closeSocket(void){
            close(sock);
        }
};*/

/**************************************************
-
- Destructor
-
*************************************************/
SVT_CAN::~SVT_CAN(){

}

int SVT_CAN::init_log(){

	int i;	
	int MAX_SENSE = 0;
	int STM_BUF_SIZE = 0;
	string line;
	char *token, *subtoken, *saveptr1, *saveptr2;

	while(getline(configFile, line)){

		char *dup = strdup(line.c_str());
		token = strtok_r(dup, "'", &saveptr1);

		if(!strcmp(token, "NUM_STM_IN_SYS:")){

			token = strtok_r(NULL, "'", &saveptr1);
			NUM_STM = atoi(token);

		} else if(!strcmp(token, "STM_BUFFER_SIZE:")){

			token = strtok_r(NULL, "'", &saveptr1);
			STM_BUF_SIZE = atoi(token);

		} else if(!strcmp(token, "MAX_NUMBER_SENSORS:")){

			token = strtok_r(NULL, "'", &saveptr1);
			MAX_SENSE = atoi(token);				

		} else if(!strcmp(token,"SENSORS:")){
			
			tokc = new int [NUM_STM];
			fileNames = new string [NUM_STM];	
			token = strtok_r(NULL, "'", &saveptr1);
			for(i=0; i<NUM_STM; i++){
				tokc[i] = 0;
			}
			sensors = new string* [NUM_STM];
			ids = new string* [NUM_STM];
			for(i=0; i<NUM_STM; i++){
				sensors[i] = new string [MAX_SENSE];
				ids[i] = new string [MAX_SENSE];	
			}
			while(token != NULL){
	
				if(strcmp(token, ",")){
					subtoken = strtok_r(token, "=", &saveptr2);
					if(subtoken != NULL){
						i = 0;
						char *temp = new char[strlen(subtoken) + 1];
						strcpy(temp, subtoken);
						strcat(temp, ".csv");
						if(stms == 0){
							fileNames[stms] = temp;
							stms++;
						} else {
							for(i=0; i<stms; i++){
								if(!strcmp(temp, fileNames[i].c_str())){
									break;
								}
							}
							if(stms == i){
								if(stms != NUM_STM){
									fileNames[stms] = temp;
								} else {
									cout << "ERROR: Provided more STM file names than a lotted. Increase NUM_STM_IN_SYS" << endl;
								}
							}
						}
						delete []temp;
						subtoken = strtok_r(NULL, "=", &saveptr2);
						if(subtoken != NULL){
							sensors[i][tokc[i]] = subtoken;
							subtoken = strtok_r(NULL, "=", &saveptr2); 
							ids[i][tokc[i]] = subtoken; 
							tokc[i] += 1;
						} else {
							cout << "ERROR: wrong format for config.txt SENSORS: \'stm=name=id\'" << endl;
						}
					} else {
						cout << "ERROR: wrong format for config.txt SENSORS: \'stm=name=id\'" << endl;
					}
				}
				token = strtok_r(NULL, "'", &saveptr1);

			}
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

		}
		free(dup);
		

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
   /*if(std::ifstream("output.txt")){
	outFile.open("output.txt", ios::out | ios::app);
	init_log();
   } else {
	outFile.open("output.txt", ios::out | ios::app);
	init_log();
	for(int i=0; i<tokc; i++){
		outFile << sensors[i] << ",\t";
	}
	outFile << endl;
   }*/

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
	
	switch(pParsed->payload.type)
	{
		case(3):{
			buf << pParsed->payload.data.type3.sensor1Data; 
			server.json_message.setLumSensor((time(0) + 65536), pParsed->payload.type, pParsed->payload.data.type3.sensor1Data);
			server.json_message.printJson();
			server.sendPacket();
		}break;
		/*case(1):{
			
			setOriSensor(int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]){
			server.json_message.setOriSensor((time(0) + 65536), pParsed->payload.type, [0,0], [pParsed->payload.data.type1.sensor1Data);
			server.json_message.printJson();
			server.sendPacket();
		}break;*/
	}
}
/*int SVT_CAN::parse_canframe(struct can_frame& cf, int sensor, stringstream& buf, int idx){

	int value;
	int i;
	string LIGHT = "LIGHT";
	string ORIEN = "ORIENTATION";
	string GPS = "GPS";
	string CHARGE = "CHARGE";

	for(i=0; i<tokc; i++){
		if(sensor == atoi(ids[i].c_str())){
			break;
		}
	}

	int msgid = int(cf.data[idx]);
	msgname[msgid]++;
	
	if(!sensors[i].compare(LIGHT)){
		value = int(cf.data[idx]) * 256;
		idx++;
		value += int(cf.data[idx]);
		buf << value;
	} else if(!sensors[i].compare(ORIEN)){
	} else if(!sensors[i].compare(GPS)){
	} else if(!sensors[i].compare(CHARGE)){
	} else {
		buf << int(cf.data[idx]);
	}

	//UDP_send(uptime.count(), sensor, value);
	UDP_send(time(0), sensor, value);
	return idx;

}*/

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
				for(int k=0; k<8; k++){
					stmbufs.stm[i].msgData[stmbufs.stm[i].bufIdx][k] = int(cf.data[k]);
				}
				stmbufs.stm[i].bufIdx += 1;
				if(stmbufs.stm[i].bufIdx == stmbufs.stm[i].size){
					stmbufs.stm[i].bufIdx = 0;
					//DO I WANT TO DO THE PARSED STRING INSTEAD?
					//store_canBuffer(stmbufs.stm[i].msgData);
				}
				i = NUM_STM + 1;
				break;
			}

		}

	}
    	outFile << buf.str();
    	outFile.flush();

}



void SVT_CAN::UDP_send(int time, int ID, int val){

	server.json_message.setLumSensor(time, ID, val);
	server.json_message.printJson();
	cout << "send UDP packet" << endl;
	server.sendPacket();

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

