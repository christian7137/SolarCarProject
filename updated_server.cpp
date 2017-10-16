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


//using namespace std;
void error(const char *);

class Server {
    public:
        int sock, possError,port; 
        unsigned int length;
        struct sockaddr_in server; 
        
        Server(){
            port = 1500;    
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr("127.0.0.1");
            server.sin_port = htons(port);//atoi(argv[2])
            length=sizeof(struct sockaddr_in);
        }
        
        bool connectSock(void){
            bool sockConnect = true;
            if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                 error("socket");
                 sockConnect = false;
            }
            
            return sockConnect;
        }
        
        void sendPacket(char buffer[]){
            possError = sendto(sock,buffer, strlen(buffer),0,(const struct sockaddr *)&server,length);
        }
        
        void closeSocket(void){
            close(sock);
        }
};


int main(int argc, char *argv[])
{
    Server server;
    if(!server.connectSock())
        return 0;
    char buffer[256] = {"hi does 4129 29565 -89 13205 1413741618 4129 29565 -89 13205 0 1 29430 -89 13215 77 1 1413741619 5131 29565 -89 13200 0 1 29430 -89 13215 77 11413741617 3127 29565 -89 13205 0 1 29385 -89 13215 77 1 1413741623 9145 29565 -89 13200 0 1 29430 -89 13215 77 1"};
            
    int count = 0;      //testing
    while(count < 100){
       server.sendPacket(buffer);
       //code below is testing
       count++;
       std::cout << "number of packets sent is: " << count << std::endl;
       //usleep(100);//microseconds
    }
   
   server.closeSocket();
   return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}