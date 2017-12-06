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
//void error(const char *);

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
            serverAddr.sin_addr.s_addr = inet_addr("192.168.0.106");//127.0.0.1, 192.168.0.255, 169.254.255.255 <- this one computer to RPi
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
