/*****************************************************
 *
 * SVT_stmBuffer.h
 *
 * Author: James Creamer
 *
 * Defines buffer class for STMs
 *
 *****************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h> //for the time
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;
using std::vector;




class STM {

	public:

		vector<vector<uint8_t>> msgData;
		vector<int> ts;
		int bufIdx;
		int size;
		int sensorNum;
		string *sensors;
		string *ids;
		
			
		void make_space(int, int, string*, string*);
};

void STM::make_space(int bufsize, int numSensors, string* Sense, string* ID){
			
	int i;
	bufIdx = 0;
	size = bufsize;
	sensors = Sense;
	ids = ID;
	sensorNum = numSensors;
	msgData.resize(bufsize);
	ts.resize(bufsize);
	for(i=0; i<bufsize; i++){
		msgData[i].resize(8);
	}

}


class STMBuffers {

	public:
		vector<STM> stm;
		int size;

		void make_buffer(int, int, string**, string**, int*);
	

};


void STMBuffers::make_buffer(int numSTM, int bufsize, string** sensors, string** ids, int* tokc){

	int i;
	size = numSTM;
	stm.resize(numSTM);;
	for(i=0; i<numSTM; i++){
		stm[i].make_space(bufsize, tokc[i], sensors[i], ids[i]);
	}

}
