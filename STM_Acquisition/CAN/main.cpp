/**************************************
 -
 - file - main.cpp
 -
 - purpose - program to test CAN interface and communication with FRDM-K64Fs
 -
 - author - J. C. Wiley, Jan 2015
 -
***************************************/

#include "SVT_CAN.h"
#include <iostream>
#include <string>
#include "SV_gpio.h"

#define LISTENONLY 1
#define NORESET

int main(){
	SVT_CAN* can0;    // Beaglebone threaded socketcan
	int ret;
	int err;

#ifndef NORESET
	// Set up reset lines
	SV_gpio Reset1;
	SV_gpio Reset3;
	SV_gpio Reset2;
	SV_gpio Reset4;

  	string pinmn_Reset1("GPIO0_26");  // 8_14 gpio0_26 = GPIO26
  	string pinmn_Reset2("GPIO1_15");  // 8_15 gpio1_15 = GPIO47
  	string pinmn_Reset3("GPIO1_14");  // 8_16 gpio1_14 = GPIO46
  	string pinmn_Reset4("GPIO0_27");  // 8_17 gpio0_27 = GPIO27

  	// gpio pin numbers  gpioX_Y = X*32 + Y
  	int pinno_Reset1 = 26;      // 8_14
  	int pinno_Reset2 = 47;      // 8_15
  	int pinno_Reset3 = 46;      // 8_16
  	int pinno_Reset4 = 27;      // 8_17

	if((err = Reset1.init(pinno_Reset1,  pinmn_Reset1, string("out")))){
		SV_perror("error in Reset1.init ", 0, err);
		return -11;
	}

	if((err = Reset3.init(pinno_Reset3,  pinmn_Reset3, string("out")))){
		SV_perror("error in Reset3.init ", 0, err);
		return -11;
	}

	if((err = Reset2.init(pinno_Reset2,  pinmn_Reset2, string("out")))){
		SV_perror("error in Reset2.init ", 0, err);
		return -11;
	}

	if((err = Reset4.init(pinno_Reset4,  pinmn_Reset4, string("out")))){
		SV_perror("error in Reset4.init ", 0, err);
		return -11;
	}
#endif
	// create can bus
	can0 = new SVT_CAN;

#ifndef NORESET
	// reset K64F boards
	int pulsewidth =   100000;  //us

	Reset3.clear();
	Reset2.clear();
	Reset4.clear();
	Reset1.clear();

	// hold in reset for pulsewidth microseconds
	Reset3.set();  // TexsunOne
	//Reset2.set();  //?
	Reset4.set();    // MotorController
	Reset1.set();    //sensorOne
	usleep(pulsewidth);

	Reset3.clear();
	Reset2.clear();
	Reset4.clear();
	Reset1.clear();
#endif

	printf("can reader: \n");
	//-------------- can0 test ----------------------------
	if( (err = can0->init()) ){
		cout << "can0 failed to initialize with " << err << endl;
		return -1;
	}
	printf("can0 initialized \n");

#ifdef PINGPONG
	while(1){
		if( (ret = can0->readmsg(rxframe)) ){ // if 0, no message available
			if(ret < 0){
				cout << "readmsg error " << ret << endl;
				return (-1);
			}
			else if(ret > 0){ // read message
				count--;
				can0->print_canframe("rx ",rxframe);
				// send reply
				txframe.data[0] = rxframe.data[0];
				usleep(100000);
				if( (ret = can0->send(txframe))){
					if(ret < 0){
						cout << "CAN_test:: can0 send failed: "<< txframe.data[0] << " err: "<< err << endl;
						return -2;
					}
					else if(ret > 0){
						cout << "sent " << ret << endl;
						can0->print_canframe("tx ",txframe);
					}
				}
			}
		}
	}
#elif LISTENONLY
	can_frame rxframe;
	while(1){
		if( (ret = can0->readmsg(rxframe)) ){ // if 0, no message available
			if(ret < 0){
				cout << "readmsg error " << ret << endl;
				return (-1);
			}
			else if(ret > 0){ // print message
				can0->print_canframe("rx ",rxframe);
			}
		}
		//cout << "..." <<endl;
	}
#else  // just send
	can_frame txframe;
	txframe.can_id = 701;
	txframe.can_dlc = 8;
	txframe.data[0] = 1;
	txframe.data[1] = 1;
	txframe.data[2] = 2;
	int cnt = 0;
	while(cnt < 200){
		if( (ret = can0->send(txframe))){
			if(ret < 0){
				stringstream errmsg;
				errmsg <<"CAN_test:: can0 send failed: "<< txframe.data[0] << endl;;
				SV_perror(errmsg.str().c_str(), 0, err);
				// return -2;
			}
			else if(ret > 0){
				cout << "sent " << ret << endl;
				can0->print_canframe("tx ",txframe);
				txframe.data[0] += 1;
			}
			usleep(200000);
			cnt++;
			txframe.data[7] = cnt;
		}
	}
#endif

}

