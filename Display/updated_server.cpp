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
//#include <unistd.h> //for the time
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
        }
        
        void closeSocket(void){
            close(sock);
        }
};


//testing the class, delete or comment when testing is complete
int main(int argc, char *argv[])
{
	
    Server server;
 
    int count = 0;      //testing
	for(int i = 0; i < 20; i++){
		server.json_message.setLumData(i+20);
		server.json_message.setLumID(i+100);
		server.json_message.setTimestamp(i);
		int temp = 20 - i;
		server.json_message.setTime(temp);
		server.json_message.combineIntoSingleJson();
		server.json_message.printJson();
		//std::cout << "Timestamp: " << server.json_message.all_json.json1.timestamp << " Time: " << server.json_message.all_json.json2.time << std::endl;
		server.sendPacket();
		//server.possError = sendto(server.sock,(struct Json_Message*)&server.json_message.all_json, sizeof(server.json_message.all_json),0,(const struct sockaddr *)&server.serverAddr,server.length);//change strlen(buffer)
	}
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


