// UDP server: sends data to the client 
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
#include "json_message.h"



//using namespace std;
void error(const char *);

class Server {
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
            serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//127.0.0.1, 192.168.0.255, 169.254.255.255 <- this one computer to RPi
            serverAddr.sin_port = htons(port);//atoi(argv[2])
            length=sizeof(struct sockaddr_in);
			connectSock();
		}  
        
        void sendPacket(){
			possError = sendto(sock,(struct Json_Message*)&json_message.all_json, sizeof(json_message.all_json),0,(const struct sockaddr *)&serverAddr,length);
			json_message.resetJson();
		}
        
        void closeSocket(void){
            close(sock);
        }
};


//testing the class, delete or comment when testing is complete
int main(int argc, char *argv[])
{
	int two[] ={3,4};
	float twoFloat[] = {10.5, 45.23};
	int16_t three1[] = {1,2,3};
	int16_t three2[] = {2,4,6};
	int16_t three3[] = {3,6,9};
	int16_t three4[] = {4,8,12};
	int four[] = {4,8,12,16};
    
	Server server;
	std::cout << "Size of the json message is: " << sizeof(server.json_message.all_json) << std::endl;
	int count = 0;      //testing
	//if(i%4 == 0)
		

//int start_s=clock();
	// the code you wish to time goes here
		server.json_message.setLumSensor(count,3,20-count);//int ts, int id, int tempData
		server.json_message.setOriSensor(count,5,two,three1,three2,three3);//int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]
		server.json_message.setSOCSensor(count, 1, four);
		server.json_message.setGPSSensor(count, 2, twoFloat);
	//int stop_s=clock();
	//std::cout << "Start: " << start_s << " End: " << stop_s << std::endl;
	//std::cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;

	int i = 0;
	for(i; i < 10; i++){
		/*if(i%4 == 0)
			server.json_message.setLumSensor(i,3,20-i);//int ts, int id, int tempData
		if(i%5 == 0)
			server.json_message.setOriSensor(i,5,two,three1,three2,three3);//int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]
		if(i%10 == 0)
			server.json_message.setSOCSensor(i, 1, four);
		if(i%2 == 0)
			server.json_message.setGPSSensor(i, 2, twoFloat);
		*/
		//server.json_message.printJson();
		if(i%2 == 0){
			server.json_message.setSOCSensor(i, 1, four);
			server.json_message.setGPSSensor(i, 2, twoFloat);
		}
		else{
			server.json_message.setLumSensor(i,3,20-i);//int ts, int id, int tempData
			server.json_message.setOriSensor(i,5,two,three1,three2,three3);//int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]
		}
			
		if(server.json_message.getSOCValue().compare("None") == 0)
			std::cout << "SOC VALUE IS NOT VALID" << std::endl;
		if(server.json_message.getGPSValue().compare("None") == 0)
			std::cout << "GPS VALUE IS NOT VALID" << std::endl;
		if(server.json_message.getLumValue().compare("None") == 0)
			std::cout << "Lum VALUE IS NOT VALID" << std::endl;
		if(server.json_message.getOriValue().compare("None") == 0)
			std::cout << "ORI VALUE IS NOT VALID" << std::endl;
		server.sendPacket();
		
		//server.json_message.setLumSensor(count,3,20-count);//int ts, int id, int tempData
		//server.json_message.setOriSensor(count,5,two,three1,three2,three3);//int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]
		//server.json_message.setGPSSensor(count, 2, twoFloat);
		std::cout << "Sent message # " << i << std::endl;
		//usleep(10000);//microseconds
	}
	std::cout << "Total Messages Sent: " << i << std::endl;
	/*while(count < 100){
       server.sendPacket(buffer);
       //code below is testing
       count++;
       std::cout << "number of packets sent is: " << count << std::endl;
       usleep(10000);//microseconds
    }*/
   
   server.closeSocket();
   return 0;
}


