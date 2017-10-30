//UDP Client: receives packets from the server
#include "client.h"


//testing the class, delete or comment when testing is complete
int main(int argc, char *argv[]){
   Client client;

	
   int count = 0;//testing
   
   while (1) {
       //client.possError = recvfrom(client.sock, client.buf, 256, 0,(struct sockaddr *)&client.sourceAddress,&client.sourceSize);
       client.receiveMessage();
	   count++;
       std::cout << "Received packet # " << count << std::endl << std::endl;
       //code below is for testing
       //client.json_message.printLuminosity();//printJson();
	   //client.json_message.printJson();      
      // if(count >= 100) break; 
   }
   
   client.closeSocket();
   return 0;
}

