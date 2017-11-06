#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include "json_message.h"


//Once a Client object is created, everything will be initialized. Only maintenance is to
//close the socket before exiting the program. client.closeSocket() must be called.
class Client {
	private:
		int sock, possError, serverSize, port;
        socklen_t sourceSize;
        struct sockaddr_in server;
        struct sockaddr_in sourceAddress;
		
		void error(const char *msg){
			perror(msg);
			exit(0);
		}
		
        void sockConnect(void){
            if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                error("Opening socket");
            }
        }
        
        void bindClient(void){
            if (bind(sock,(struct sockaddr *)&server,serverSize)<0){
                 error("binding");
		    } 
        }
		
		
	public:
		Json_Message json_message;
           
        Client(){//initalize the client side
            port = 1500;
            serverSize = sizeof(server);
            server.sin_family=AF_INET;
            server.sin_addr.s_addr=INADDR_ANY;
            server.sin_port=htons(port);
            sourceSize = sizeof(struct sockaddr_in);
			
			sockConnect();
			bindClient();
        }
        
        void receiveMessage(void){
            //std::cout << "wating for data..." << std::endl;
            possError = recvfrom(sock, (struct Json_Message*)&json_message.all_json, sizeof(json_message.all_json), 0,
				(struct sockaddr *)&sourceAddress,&sourceSize);

            if (possError < 0){ 
                error("recvfrom");
            }               
        }
        
        void closeSocket(void){
            close(sock);
        }
};
