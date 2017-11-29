class Luminosity{
	private:
		bool valid;
		int timestamp;
		int sensor_id;
		int data;			
		
		
	public:
		//Constructor
		Luminosity(){
			resetSensor();
		}
		
		
		//Set the Luminosity data
		void setSensor(int ts, int id, int tempData){
			valid = true;
			timestamp = ts;
			sensor_id = id;
			data = tempData;
		}
		
		
		/*Gather the data into a string to give to the python parser
		* The different data is seperated by ","
		*
		* How the string is created:
		* timestamp,sensor id,data
		*/
		std::string getSensor(){
			if(valid){//all_json.luminosity.valid == true){
				std::stringstream sstm;
				sstm << timestamp << "," << sensor_id << "," << data;
				
				std::string lumString = sstm.str();
				std::cout << lumString << std::endl;
				return lumString;
			}
			else
				return "None";//not valid
		}
		
		
		//Resets the Luminosity sensor
		void resetSensor(void){
			valid = false;
		}
		
		
		//Print Luminosity data
		void print(){
			std::cout << "Luminosity: " << std::endl;

			std::cout << "Timestamp: " << timestamp << " Sensor ID: " << sensor_id << std::endl;
			
			std::cout << "Data: ";
			if(valid){//<< std::endl;
				std::cout << data; 
			}	
			else{
				std::cout << "INVALID DATA";
			}
			std::cout << std::endl << std::endl;
		}
};
		