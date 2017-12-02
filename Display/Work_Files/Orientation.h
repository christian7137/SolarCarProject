class Orientation{
	private:
		bool angleValid;
		bool accValid;
		bool gyrValid;
		bool magValid;
		int timestamp;
		int sensor_id;
		int angle[2];//int angle (dir, pitch)
		int16_t accel[3];//acceleration x,y,z int16
		int16_t gyr[3];//gyr x,y,z, int 16
		int16_t mag[3];//mag x,y,z int 16
		
	
	public:
		//Constructor 
		Orientation(){
			resetSensor();
		}
		
		
		//Set the sensor Data
		void setSensor(int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]){
			timestamp = ts;
			sensor_id = id;
			
			//if the program goes through any of these if statements that means that part of the message is valid
			if(inputAngle[0] != INT_MAX){
				angleValid = true;
				angle[0] = inputAngle[0];
				angle[1] = inputAngle[1];			
			}
			if(inputAcc[0] != SHRT_MAX){
				accValid = true;
				accel[0] = inputAcc[0];
				accel[1] = inputAcc[1];
				accel[2] = inputAcc[2];			
			}
			if(inputGyr[0] != SHRT_MAX){
				gyrValid = true;
				gyr[0] = inputGyr[0];
				gyr[1] = inputGyr[1];
				gyr[2] = inputGyr[2];
			}
			if(inputMag[0] != SHRT_MAX){
				magValid = true;
				mag[0] = inputMag[0];
				mag[1] = inputMag[1];
				mag[2] = inputMag[2];
			}	
		}
		
		
		/*Gather the data into a string to give to the python parser
		* The different data is seperated by ","
		*
		* How the string is created:
		* timestamp,sensor id,angle[0],angle[1],accel[0],accel[1],accel[2],gyr[0],gyr[1],gyr[2],mag[0],mag[1],mag[2]
		*/
		std::string getSensor(){
			//at least one sensor is valid
			if(angleValid || accValid || gyrValid || magValid){
				std::stringstream sstm;
				sstm << timestamp << "," << sensor_id << ",";

				
				if(angleValid){
					sstm << angle[0] << "," << angle[1] << ",";
				}
				else{
					sstm << ",,";
				}

				
				if(accValid){
					sstm << accel[0] << "," << accel[1] << "," << accel[2] << ",";
				}
				else{
					sstm << ",,,";
				}

				
				if(gyrValid){
					sstm << gyr[0] << "," << gyr[1] << "," << gyr[2] << ",";
				}
				else{
					sstm << ",,,";
				}
				

				if(magValid){
					sstm << mag[0] << "," << mag[1] << "," << mag[2];					
				}
				else{
					sstm << ",,";
				}
				
				std::string oriString = sstm.str();
				std::cout << oriString << std::endl;
				return oriString;
			}
			else
				return "None";//not valid
		}
		
		
		//Resets the valid bit
		void resetSensor(void){
			angleValid = false;
			accValid = false;
			gyrValid = false;
			magValid = false;
		}
	
	
		//Print the orientation data
		void print(){
			std::cout << "Orientation: " << std::endl;
			
			std::cout << "Timestamp: " << timestamp << " Sensor ID: " << sensor_id << std::endl;
			
			
			std::cout << "Angle Values: ";
			if(angleValid){//<< std::endl;
				std::cout << angle[0] << " " << angle[1];
			}
			else{
				std::cout << "INVALID DATA";
			}
			std::cout << std::endl;
			
			
			std::cout << "Acceleration Values: ";
			if(accValid){//<< std::endl;
				std::cout << accel[0] << " " << accel[1] << " " << accel[2];
			}	
			else{
				std::cout << "INVALID DATA";
			}
			std::cout << std::endl;
				
			
			std::cout << "Gyr Values: ";
			if(gyrValid){//<< std::endl;
				std::cout << gyr[0] << " " << gyr[1] << " " << gyr[2]; 
			}	
			else{
				std::cout << "INVALID DATA";
			}
			std::cout << std::endl;
		
			
			
			std::cout << "Mag Values: ";
			if(magValid){
				std::cout << mag[0] << " " << mag[1] << " " << mag[2];
			}	
			else{
				std::cout << "INVALID DATA";
			}
			std::cout << std::endl << std::endl;
		}
	};
				