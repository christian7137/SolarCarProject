#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;

ifstream inFile;

int main(){

	int pid;
	int i;
	char x[100];
	int my_pipe[2];

	//system("sudo ip link set can0 up type can bitrate 100000");

	pipe(my_pipe);
	pid = fork();
	if(pid){
		//put into buffer
		close(my_pipe[1]);
		
		while(1){
			while(read(my_pipe[0], x, 1) > 0){
				//write(1, x, 1);
				printf("%s", x);
				for(i=0; i<6; i++){
					char* token = strtok(x, "\n");
					printf("%s", token);
				}
			}
		}
	} else {
		//child reading can0
		close(my_pipe[0]);
		dup2(my_pipe[1], 1);
		system("candump can0");
	}
}
