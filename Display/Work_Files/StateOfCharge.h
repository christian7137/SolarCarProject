//#include <string.h>
//#include <sstream>
class StateOfCharge{
	
	private:	
		bool valid;
		int timestamp;//each one is 4 bytes -> atleast for ints
		int sensor_id;
		int data[4];

	public:
		//Constructor
		StateOfCharge(){
			resetSensor();
		}
		
		
		//Set the sensor data
		void setSensor(int16_t ts, int sen_id, int temp_data[4]){
			valid = true;
			timestamp = ts;
			sensor_id = sen_id;
			data[0] = temp_data[0];
			data[1] = temp_data[1];
			data[2] = temp_data[2];
			data[3] = temp_data[3];
		}
		
		
		/*Gather the data into a string to give to the python parser
		* The different data is seperated by ","
		*
		* How the string is created:
		* timestamp,sensor id,data[0],data[1],data[2],data[3]
		*/
		std::string getSensor(){
			if(valid){
				std::stringstream sstm;
				sstm << timestamp << "," << sensor_id << ",";
				
				sstm << data[0] << "," << data[1] << ",";
				sstm << data[2] << "," << data[3];
				std::string socString = sstm.str();
				std::cout << socString << std::endl;
				return socString;
			}
			else//not valid
				return "None";
		}
		
		/*std::stringstream test(){
			std::stringstream sstm;
				sstm << timestamp << "," << sensor_id << ",";
				return sstm;
		}*/
		
		
		//Resets the valid bit
		void resetSensor(void){
			valid = false;
		}
	
	
		//Print the state of charge data
		void print(){
			std::cout << "State of Charge: " << std::endl;

			std::cout << "Timestamp: " << timestamp << " Sensor ID: " << sensor_id << std::endl;			
			
			std::cout << "Values: ";
			if(valid){
				std::cout << data[0] << " " << data[1] << " ";
				std::cout << data[2] << " " << data[3];
			}
			else{
				std::cout << "INVALID DATA";
			}
			std:: cout << std::endl << std::endl;
		}
};