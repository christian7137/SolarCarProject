//this is the json message format that will be sent between the pit and car RPi
//Note: Cannot use strings. When a string is created, it is a pointer to the string, not the string value.
//Thus, the value associated with the string will be pointer, which will cause a seg. fault
#include <string.h>
#include <sstream>
//sizeof returns the size in bytes
class Json_Message{
	private:
		
		struct StateOfCharge{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		
		struct All_Json{
			struct StateOfCharge stateOfCharge;
			struct Orientation orientation;
			struct Luminosity luminosity;
			struct GPS gps;
			
			//delete everything after this
			/*struct StateOfCharge stateOfCharge1;
			struct Orientation orientation1;
			struct Luminosity luminosity1;
			struct GPS gps1;
			struct StateOfCharge stateOfCharge2;
			struct Orientation orientation2;
			struct Luminosity luminosity2;
			struct GPS gps2;
			struct StateOfCharge stateOfCharge3;
			struct Orientation orientation3;
			struct Luminosity luminosity3;
			struct GPS gps3;
			struct StateOfCharge stateOfCharge4;
			struct Orientation orientation4;
			struct Luminosity luminosity4;
			struct GPS gps4;
			struct StateOfCharge stateOfCharge5;
			struct Orientation orientation5;
			struct Luminosity luminosity5;
			struct GPS gps5;
			struct StateOfCharge stateOfCharge6;
			struct Orientation orientation6;
			struct Luminosity luminosity6;
			struct GPS gps6;
			struct StateOfCharge stateOfCharge7;
			struct Orientation orientation7;
			struct Luminosity luminosity7;
			struct GPS gps7;
			struct StateOfCharge stateOfCharge8;
			struct Orientation orientation8;
			struct Luminosity luminosity8;
			struct GPS gps8;
			struct StateOfCharge stateOfCharge9;
			struct Orientation orientation9;
			struct Luminosity luminosity9;
			struct GPS gps9;*/
			
		};
		//REMOVE THE STRUCTS AFTER THIS COMMENT. THIS IS FOR TESTING ONLY
		struct StateOfCharge1{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation1{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity1{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS1{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		struct StateOfCharge2{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation2{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity2{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS2{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		struct StateOfCharge3{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation3{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity3{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS3{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};struct StateOfCharge4{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation4{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity4{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS4{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		struct StateOfCharge5{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation5{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity5{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS5{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		struct StateOfCharge6{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation6{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity6{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS6{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};struct StateOfCharge7{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation7{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity7{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS7{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		struct StateOfCharge8{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation8{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity8{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS8{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		struct StateOfCharge9{
			bool valid;
			int timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation9{
			bool valid;
			int timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity9{
			bool valid;
			int timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS9{
			bool valid;
			int timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
	public:
	All_Json all_json;
	
	
	Json_Message(){
		resetJson();
	}
	
	//makes all the json messages invalid
	void resetJson(){
		all_json.stateOfCharge.valid = false;
		all_json.orientation.valid = false;
		all_json.luminosity.valid = false;
		all_json.gps.valid = false;
	}
	
	/****************************************************************************************************************
	* Print Methods
	****************************************************************************************************************/
	void printJson(){
		printSOC();
		printOrientation();
		printLuminosity();
		printGPS();
		std::cout << "----------------------------------END OF MESSAGE----------------------------------" << std::endl;
	}
	
	void printSOC(){
		std::cout << "State of Charge: " << std::endl;

		std::cout << "Is this sensor valid? " << all_json.stateOfCharge.valid << std::endl;
		
		std::cout << "Timestamp: " << all_json.stateOfCharge.timestamp << " Sensor ID: ";
		std::cout << all_json.stateOfCharge.sensor_id << std::endl;
		
		std::cout << "Values: " << all_json.stateOfCharge.data[0] << " " << all_json.stateOfCharge.data[1] << " ";
		std::cout << all_json.stateOfCharge.data[2] << " " << all_json.stateOfCharge.data[3] << std::endl << std::endl;
	}
	
	void printOrientation(){
		std::cout << "Orientation: " << std::endl;
		std::cout << "Is this sensor valid? " << all_json.orientation.valid << std::endl;
		
		std::cout << "Timestamp: " << all_json.orientation.timestamp << " Sensor ID: ";
		std::cout << all_json.orientation.sensor_id << std::endl;
		
		std::cout << "Angle Values: " << all_json.orientation.angle[0] << " ";
		std::cout << all_json.orientation.angle[1] << std::endl;
	
		std::cout << "Acceleration Values: " << all_json.orientation.accel[0] << " ";
		std::cout << all_json.orientation.accel[1] << " " << all_json.orientation.accel[2] << std::endl;
			
		std::cout << "Gyr Values: " << all_json.orientation.gyr[0] << " ";
		std::cout << all_json.orientation.gyr[1] << " " << all_json.orientation.gyr[2] << std::endl;
		
		std::cout << "Mag Values: " << all_json.orientation.mag[0] << " ";
		std::cout << all_json.orientation.mag[1] << " " << all_json.orientation.mag[2] << std::endl << std::endl;
	}
	
	void printLuminosity(){
		std::cout << "Luminosity: " << std::endl;
		std::cout << "Is this sensor valid? " << all_json.luminosity.valid << std::endl;
		
		std::cout << "Timestamp: " << all_json.luminosity.timestamp << " Sensor ID: ";
		std::cout << all_json.luminosity.sensor_id << std::endl;
		
		std::cout << "Data: " << all_json.luminosity.data << std::endl << std::endl;
	}
	
	void printGPS(){
		std::cout << "GPS: " << std::endl;
		std::cout << "Is this sensor valid? " << all_json.gps.valid << std::endl;
		std::cout << "Timestamp: " << all_json.gps.timestamp << " Sensor ID: " << all_json.gps.sensor_id << std::endl;
		std::cout << "Location: " << all_json.gps.location[0] << " " << all_json.gps.location[1] << std::endl << std::endl;
	}
	
	
	/****************************************************************************************************************
	* Set Methods
	****************************************************************************************************************/
	//LOOK IF THE USER INPUTS NULL!@!@!##!@@#!@$!@$&$^!&@$%!@^&$%!@^&$*@$@!$@!^$*@*^@
	void setLumSensor(int ts, int id, int tempData){
		all_json.luminosity.valid = true;
		all_json.luminosity.timestamp = ts;
		all_json.luminosity.sensor_id = id;
		all_json.luminosity.data = tempData;
	}
	
	void setSOCSensor(int ts, int id, int inputData[4]){
		all_json.stateOfCharge.valid = true;
		all_json.stateOfCharge.timestamp = ts;
		all_json.stateOfCharge.sensor_id = id;
		
		all_json.stateOfCharge.data[0] = inputData[0];
		all_json.stateOfCharge.data[1] = inputData[1];
		all_json.stateOfCharge.data[2] = inputData[2];
		all_json.stateOfCharge.data[3] = inputData[3];
	}
			
	void setOriSensor(int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]){
		all_json.orientation.valid = true;
		all_json.orientation.timestamp = ts;
		all_json.orientation.sensor_id = id;
		
		all_json.orientation.angle[0] = inputAngle[0];
		all_json.orientation.angle[1] = inputAngle[1];
		
		all_json.orientation.accel[0] = inputAcc[0];
		all_json.orientation.accel[1] = inputAcc[1];
		all_json.orientation.accel[2] = inputAcc[2];
		
		all_json.orientation.gyr[0] = inputGyr[0];
		all_json.orientation.gyr[1] = inputGyr[1];
		all_json.orientation.gyr[2] = inputGyr[2];
		
		all_json.orientation.mag[0] = inputMag[0];
		all_json.orientation.mag[1] = inputMag[1];
		all_json.orientation.mag[2] = inputMag[2];
	}
	
	void setGPSSensor(int ts, int id, float inputLoc[2]){
		all_json.gps.valid = true;
		all_json.gps.timestamp = ts;
		all_json.gps.sensor_id = id;
		
		all_json.gps.location[0] = inputLoc[0];
		all_json.gps.location[1] = inputLoc[1];
	}
	
	/****************************************************************************************************************
	* Get Methods
	* The get methods put all the information for each message and returns it via string.
	* The receiver of the function call will need to know which value corresponds to what value.
	* Each value is seperated by a ','
	****************************************************************************************************************/
	
	std::string getSOCValue(){
		if(all_json.stateOfCharge.valid == true){
			std::stringstream sstm;
			sstm << all_json.stateOfCharge.timestamp << "," << all_json.stateOfCharge.sensor_id << ",";
			
			sstm << all_json.stateOfCharge.data[0] << "," << all_json.stateOfCharge.data[1] << ",";
			sstm << all_json.stateOfCharge.data[2] << "," << all_json.stateOfCharge.data[3];
			std::string socString = sstm.str();
			std::cout << socString << std::endl;
			return socString;//return null if not valid
		}
		else//not valid
			return "None";

	}
			
	std::string getLumValue(){
		if(all_json.luminosity.valid == true){
			std::stringstream sstm;
			sstm << all_json.luminosity.timestamp << "," << all_json.luminosity.sensor_id << ",";
			sstm << all_json.luminosity.data;
			
			std::string lumString = sstm.str();
			std::cout << lumString << std::endl;
			return lumString;//return null if not valid
		}
		else
			return "None";//not valid
	}
	
	std::string getOriValue(){
		if(all_json.orientation.valid == true){
			std::stringstream sstm;
			sstm << all_json.orientation.timestamp << "," << all_json.orientation.sensor_id << ",";
			
			sstm << all_json.orientation.angle[0] << "," << all_json.orientation.angle[1] << ",";
			
			sstm << all_json.orientation.accel[0] << "," << all_json.orientation.accel[1] << ",";
			sstm << all_json.orientation.accel[2] << ",";
			
			sstm << all_json.orientation.gyr[0] << "," << all_json.orientation.gyr[1] << ",";
			sstm << all_json.orientation.gyr[2] << ",";
			
			sstm << all_json.orientation.mag[0] << "," << all_json.orientation.mag[1] << ",";
			sstm << all_json.orientation.mag[2];
			
			std::string oriString = sstm.str();
			std::cout << oriString << std::endl;
			return oriString;//return null if not valid
		}
		else
			return "None";//not valid
	}
	
	std::string getGPSValue(){
		if(all_json.gps.valid == true){
			std::stringstream sstm;
			sstm << all_json.gps.timestamp << "," << all_json.gps.sensor_id << ",";
			
			sstm << all_json.gps.location[0] << "," << all_json.gps.location[1] << ",";
			
			std::string gpsString = sstm.str();
			std::cout << gpsString << std::endl;
			return gpsString;//return null if not valid
		}
		else
			return "None";//not valid
	}
};
