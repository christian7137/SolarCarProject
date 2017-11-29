class GPS{
	private:
		bool valid;
		int timestamp;
		int sensor_id;
		float location[2];//float lat and long
		
		
	public:
		//Constructor
		GPS(){
			resetSensor();
		}
	
		//Sets the GPS sensor values
		void setSensor(int ts, int id, float inputLoc[2]){
			valid = true;
			timestamp = ts;
			sensor_id = id;
			
			location[0] = inputLoc[0];
			location[1] = inputLoc[1];
		}
		
		
		
		/*Gather the data into a string to give to the python parser
		* The different data is seperated by ","
		*
		* How the string is created:
		* timestamp,sensor id,location[0],location[1]
		*/
		std::string getSensor(){
			if(valid){//all_json.gps.valid == true){
				std::stringstream sstm;
				sstm << timestamp << "," << sensor_id << ",";
				
				sstm << location[0] << "," << location[1] << ",";
				
				std::string gpsString = sstm.str();
				std::cout << gpsString << std::endl;
				return gpsString;
			}
			else
				return "None";//not valid
		}
		
		
		//Resets the GPS sensor
		void resetSensor(void){
			valid = false;
		}
		
		//Prints the GPS data
		void print(){
			std::cout << "GPS: " << std::endl;
			std::cout << "Timestamp: " << timestamp << " Sensor ID: " << sensor_id << std::endl;
			
			
			std::cout << "Location: ";
			if(valid){//<< std::endl;
				std::cout << location[0] << " " << location[1]; 
			}	
			else{
				std::cout << "INVALID DATA";
			}
			std::cout << std::endl << std::endl;
		}
		
};