#include <string.h>
#include <sstream>
//sizeof returns the size in bytes
class Json_Message{
	private:
		int OFFSET;
		struct StateOfCharge{
			bool valid;
			int16_t timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation{
			bool valid;
			int16_t timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity{
			bool valid;
			int16_t timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS{
			bool valid;
			int16_t timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		
		struct All_Json{
			int16_t topTimestamp;
			struct StateOfCharge stateOfCharge;
			struct Orientation orientation;
			struct Luminosity luminosity;
			struct GPS gps;
		};
		
	public:
	All_Json all_json;
	
	
	Json_Message(){
		OFFSET = 0x0000FFFF;
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
	
	//combine the ts in the total json message with the ts in the sensor data
	int getTotalTS(int sensorTS){
		return ((all_json.topTimestamp << 16) + sensorTS);
	}
	
	void printSOC(){
		std::cout << "State of Charge: " << std::endl;

		std::cout << "Is this sensor valid? " << all_json.stateOfCharge.valid << std::endl;
		
		std::cout << "Timestamp: " << getTotalTS(all_json.stateOfCharge.timestamp) << " Sensor ID: ";
		std::cout << all_json.stateOfCharge.sensor_id << std::endl;
		
		std::cout << "Values: " << all_json.stateOfCharge.data[0] << " " << all_json.stateOfCharge.data[1] << " ";
		std::cout << all_json.stateOfCharge.data[2] << " " << all_json.stateOfCharge.data[3] << std::endl << std::endl;
	}
	
	void printOrientation(){
		std::cout << "Orientation: " << std::endl;
		std::cout << "Is this sensor valid? " << all_json.orientation.valid << std::endl;
		
		std::cout << "Timestamp: " << getTotalTS(all_json.orientation.timestamp) << " Sensor ID: ";
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
		
		std::cout << "Timestamp: " << getTotalTS(all_json.luminosity.timestamp) << " Sensor ID: ";
		std::cout << all_json.luminosity.sensor_id << std::endl;
		
		std::cout << "Data: " << all_json.luminosity.data << std::endl << std::endl;
	}
	
	void printGPS(){
		std::cout << "GPS: " << std::endl;
		std::cout << "Is this sensor valid? " << all_json.gps.valid << std::endl;
		std::cout << "Timestamp: " << getTotalTS(all_json.gps.timestamp) << " Sensor ID: " << all_json.gps.sensor_id << std::endl;
		std::cout << "Location: " << all_json.gps.location[0] << " " << all_json.gps.location[1] << std::endl << std::endl;
	}
	
	
	/****************************************************************************************************************
	* Set Methods
	****************************************************************************************************************/
	//LOOK IF THE USER INPUTS NULL!@!@!##!@@#!@$!@$&$^!&@$%!@^&$%!@^&$*@$@!$@!^$*@*^@
	void set_ts_json(int ts){
		all_json.topTimestamp = (ts&0xFFFF0000)>>16;
		//std::cout << "Top timestamp is: " << all_json.topTimestamp << std::endl;
	}
	
	void setLumSensor(int ts, int id, int tempData){
		all_json.luminosity.valid = true;
		set_ts_json(ts);
		all_json.luminosity.timestamp = (ts&OFFSET);
		all_json.luminosity.sensor_id = id;
		all_json.luminosity.data = tempData;
	}
	
	void setSOCSensor(int ts, int id, int inputData[4]){
		all_json.stateOfCharge.valid = true;
		set_ts_json(ts);
		all_json.stateOfCharge.timestamp = (ts&OFFSET);
		all_json.stateOfCharge.sensor_id = id;
		
		all_json.stateOfCharge.data[0] = inputData[0];
		all_json.stateOfCharge.data[1] = inputData[1];
		all_json.stateOfCharge.data[2] = inputData[2];
		all_json.stateOfCharge.data[3] = inputData[3];
	}
			
	void setOriSensor(int ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]){
		all_json.orientation.valid = true;
		set_ts_json(ts);
		all_json.orientation.timestamp = (ts&OFFSET);
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
		set_ts_json(ts);
		all_json.gps.timestamp = (ts&OFFSET);
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
			sstm << getTotalTS(all_json.stateOfCharge.timestamp) << "," << all_json.stateOfCharge.sensor_id << ",";
			
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
			sstm << getTotalTS(all_json.luminosity.timestamp) << "," << all_json.luminosity.sensor_id << ",";
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
			sstm << getTotalTS(all_json.orientation.timestamp) << "," << all_json.orientation.sensor_id << ",";
			
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
			sstm << getTotalTS(all_json.gps.timestamp) << "," << all_json.gps.sensor_id << ",";
			
			sstm << all_json.gps.location[0] << "," << all_json.gps.location[1] << ",";
			
			std::string gpsString = sstm.str();
			std::cout << gpsString << std::endl;
			return gpsString;//return null if not valid
		}
		else
			return "None";//not valid
	}
};

