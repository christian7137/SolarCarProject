//UDP Client: receives packets from the server
#include "client.h"


int main(int argc, char *argv[]){
   Client client;

   int count = 0;   
   while (1) {
       client.receiveMessage();
	  
       //client.json_message.printLuminosity();//printJson();
	   count++;
       std::cout << "Received packet # " << count << std::endl << std::endl;	  
	  //client.json_message.printJson();      
      // if(count >= 100) break; 
   }
   
   client.closeSocket();
   return 0;
}

