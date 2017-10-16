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


void error(const char *msg){
    perror(msg);
    exit(0);
}

class Client {
    public:
        int sock, possError, serverSize, port;
        char buf[256];
        socklen_t sourceSize;
        struct sockaddr_in server;
        struct sockaddr_in sourceAddress;
            
            
        Client(){//initalize the client side
            port = 1500;
            serverSize = sizeof(server);
            server.sin_family=AF_INET;
            server.sin_addr.s_addr=INADDR_ANY;
            server.sin_port=htons(port);//atoi(argv[1])
            sourceSize = sizeof(struct sockaddr_in);
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
            std::cout << "wating for data..." << std::endl;
            possError = recvfrom(sock, buf, 256, 0,(struct sockaddr *)&sourceAddress,&sourceSize);
            if (possError < 0){ 
                error("recvfrom");
            }               
        }
        
        void closeSocket(void){
            close(sock);
        }
        
};


int main(int argc, char *argv[]){
   Client client;
   if(!client.sockConnect())
        return 0;
   if(!client.bindClient())
        return 0;
        
   int count = 0;//testing
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