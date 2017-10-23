//UDP Client: receives packets from the server
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

void error(const char *msg){
    perror(msg);
    exit(0);
}

class Client {
	public:
	/*struct Json_Message{
		int timestamp;
        char header[];
        float sensor_values[];
		
		
		char message[3];//this is for testing, remove when done testing
	};*/
        int sock, possError, serverSize, port;
        char buf[256];
        socklen_t sourceSize;
        struct sockaddr_in server;
        struct sockaddr_in sourceAddress;
		Json_Message json_message;
        //struct Json_Message *json_message;
            
        Client(){//initalize the client side
            port = 1500;
            serverSize = sizeof(server);
            server.sin_family=AF_INET;
            server.sin_addr.s_addr=INADDR_ANY;
            server.sin_port=htons(port);//atoi(argv[1])
            sourceSize = sizeof(struct sockaddr_in);
			//json_message = (struct Json_Message*)malloc(sizeof(Json_Message));
        }
        
        bool sockConnect(void){
            bool validSock = true;
            if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                error("Opening socket");
                validSock = false;
            }
            return validSock;
        }
        
        bool bindClient(void){
            bool validBinding = true;
            if (bind(sock,(struct sockaddr *)&server,serverSize)<0){
                 error("binding");
                 validBinding = false;
            } 
            return validBinding;
        }
        
        void receivePacket(void){
            //std::cout << "wating for data..." << std::endl;
            possError = recvfrom(sock, buf, 256, 0,(struct sockaddr *)&sourceAddress,&sourceSize);
            if (possError < 0){ 
                error("recvfrom");
            }               
        }
        
        void closeSocket(void){
            //free(json_message);//free the malloc message
			close(sock);
        }
        
};

int main(int argc, char *argv[]){
   Client client;
   if(!client.sockConnect())
        return 0;
   if(!client.bindClient())
        return 0;
	//Json_Message temp;
	//std::cout << "hi" << std::endl;
	//std::cout << temp.json->timestamp << std::endl;
    //struct Json_Message *json_message = (struct Json_Message*)malloc(sizeof(Json_Message));
	//std::cout << client.json_message->timestamp << std::endl;
	//client.json_message.json.timestamp = 1;
	
	for(int i = 0; i <20; i++){
		client.possError = recvfrom(client.sock, (struct Json_Message*)&client.json_message.all_json, sizeof(client.json_message.all_json), 0,(struct sockaddr *)&client.sourceAddress,&client.sourceSize);//(struct Json_Message*)&json_message
		//std::cout << "Time is: " <<client.json_message.all_json.json2.time << std::endl;
		client.json_message.printJson();
		//std::cout << "Count is at: " << i << " Timestamp is: " << client.json_message.all_json.json1.timestamp << std::endl;
	}
	
   int count = 0;//testing
   /*
   while (1) {
       //client.possError = recvfrom(client.sock, client.buf, 256, 0,(struct sockaddr *)&client.sourceAddress,&client.sourceSize);
       client.receivePacket();
       //code below is for testing
       std::cout << client.buf << std::endl;
       count++;
       std::cout << "Received packet # " << count << std::endl;
      // if(count >= 100) break; 
   }*/
   
   client.closeSocket();
   return 0;
}



/*THIS IS THE BASE. WORKS WHEN INFO IS NOT IN json_message.h
//UDP Client: receives packets from the server
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

void error(const char *msg){
    perror(msg);
    exit(0);
}

class Client {
	public:
	struct Json_Message{
		int timestamp;
        char header[];
        float sensor_values[];
		
		
		char message[3];//this is for testing, remove when done testing
	};
        int sock, possError, serverSize, port;
        char buf[256];
        socklen_t sourceSize;
        struct sockaddr_in server;
        struct sockaddr_in sourceAddress;
		//Json_Message json_message;
        struct Json_Message *json_message;
            
        Client(){//initalize the client side
            port = 1500;
            serverSize = sizeof(server);
            server.sin_family=AF_INET;
            server.sin_addr.s_addr=INADDR_ANY;
            server.sin_port=htons(port);//atoi(argv[1])
            sourceSize = sizeof(struct sockaddr_in);
			json_message = (struct Json_Message*)malloc(sizeof(Json_Message));
        }
        
        bool sockConnect(void){
            bool validSock = true;
            if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                error("Opening socket");
                validSock = false;
            }
            return validSock;
        }
        
        bool bindClient(void){
            bool validBinding = true;
            if (bind(sock,(struct sockaddr *)&server,serverSize)<0){
                 error("binding");
                 validBinding = false;
            } 
            return validBinding;
        }
        
        void receivePacket(void){
            //std::cout << "wating for data..." << std::endl;
            possError = recvfrom(sock, buf, 256, 0,(struct sockaddr *)&sourceAddress,&sourceSize);
            if (possError < 0){ 
                error("recvfrom");
            }               
        }
        
        void closeSocket(void){
            //free(json_message);//free the malloc message
			close(sock);
        }
        
};

int main(int argc, char *argv[]){
   Client client;
   if(!client.sockConnect())
        return 0;
   if(!client.bindClient())
        return 0;
	//Json_Message temp;
	//std::cout << "hi" << std::endl;
	//std::cout << temp.json->timestamp << std::endl;
    //struct Json_Message *json_message = (struct Json_Message*)malloc(sizeof(Json_Message));
	std::cout << client.json_message->timestamp << std::endl;
	client.possError = recvfrom(client.sock, client.json_message, 256, 0,(struct sockaddr *)&client.sourceAddress,&client.sourceSize);//(struct Json_Message*)&json_message
	
	std::cout << client.json_message->timestamp << std::endl;
   int count = 0;//testing
   /*
   while (1) {
       //client.possError = recvfrom(client.sock, client.buf, 256, 0,(struct sockaddr *)&client.sourceAddress,&client.sourceSize);
       client.receivePacket();
       //code below is for testing
       std::cout << client.buf << std::endl;
       count++;
       std::cout << "Received packet # " << count << std::endl;
      // if(count >= 100) break; 
   }
   
   client.closeSocket();
   return 0;
}
*/

/*
int main(int argc, char *argv[]){
   int sock, length, n;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[256];
   int port = 1500;
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(port);//atoi(argv[1])
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
       error("binding");
   fromlen = sizeof(struct sockaddr_in);
   int count = 0;
   while (1) {
       n = recvfrom(sock,buf,256,0,(struct sockaddr *)&from,&fromlen);
       if (n < 0) error("recvfrom");
       std::cout << buf << std::endl;
       count++;
       std::cout << "Received packet # " << count << std::endl;
       if(count >= 100) break; 
       //write(1,"Received a datagram: ",21);
       //write(1,buf,n);
       //n = sendto(sock,"Got your message\n",17,
      //            0,(struct sockaddr *)&from,fromlen);
       //if (n  < 0) error("sendto");
   }
   
   return 0;
}
*/