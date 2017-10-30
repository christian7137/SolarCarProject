#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include <vector>


struct Orientation{
			int time;
			int header;
			int sensor_values;
		};
		
		struct Luminosity{
			int data;
			int sensor_id;//header
			int timestamp;
		};
		
		
int main(int argc, char *argv[]){
	std::vector<Orientation> ori;
	std::cout << "Num Elements: " << ori.size() << " Size: " << sizeof(ori) << std::endl;
	Orientation tempOri;// =  new Orientation;
	tempOri.time = 3;
	tempOri.header = 10000;
	tempOri.sensor_values=4034434;
	ori.push_back(tempOri);
	std::cout << "Num Elements: " << ori.size() << " Size: " << sizeof(ori) << std::endl;
	
	std::cout << " Size of struct: " << sizeof(tempOri) << std::endl;
	
	Orientation tempOri1;// =  new Orientation;
	tempOri1.time = 3;
	tempOri1.header = 10000;
	tempOri1.sensor_values=4034434;
	ori.push_back(tempOri1);
	std::cout << "Num Elements: " << ori.size() << " Size: " << sizeof(ori) << std::endl;
	
	
	return 0;
}