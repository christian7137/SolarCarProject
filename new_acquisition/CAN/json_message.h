#include <string.h>
#include <sstream>
#include <limits.h>


/*
* This is the json message format that will be sent between the Pit RPi and Solar Car RPi
*
* Note: When adding new sensors (new structs), add the struct in the All_Json Struct. Then add getter
* and setter method. Additionally, a new print method could be created, but this is just for testing purposes.
*
* Note: When adding new data to an existing struct, update the struct and update the getter and setter methods.
* This will need to be communicated to the python wrapper and the function that calls these methods needs to be updated
*/
		
		
/*
* The following is what bits in the 32 bit validValues correlate to. MSB is bit 31, LSB is 0
* Bit 0 - State of charge data
* Bit 1 - Orientation Angle
* Bit 2 - Orientation Accel
* Bit 3 - Orientation Gyr
* Bit 4 - Orientation Mag
* Bit 5 - Luminosity Data
* Bit 6 - GPS Location
* Bit 7 - Reserved from and and down to bit 31
* Bit 8
* Bit 9
* Bit 10
* Bit 11
* Bit 12
* Bit 13
* Bit 14
* Bit 15
* Bit 16
* Bit 17
* Bit 18
* Bit 19
* Bit 20
* Bit 21
* Bit 22
* Bit 23
* Bit 24
* Bit 25
* Bit 26
* Bit 27
* Bit 28
* Bit 29
* Bit 30
* Bit 31
* Bit 32
*/
			
class Json_Message{
	private:
		uint32_t TS_OFFSET;
		int INVALID_VALUE;//if this value is inputted, the value is NULL
		
		int STATE_OF_CHARGE_DATA, ORIENTATION_ANGLE, ORIENTATION_ACCEL, ORIENTATION_GYR, ORIENTATION_MAG, LUMINOSITY_DATA, GPS_LOCATION;
		
		struct StateOfCharge{
			//bool valid;
			uint16_t timestamp;//each one is 4 bytes -> atleast for ints
			int sensor_id;
			int data[4];
		};
		
		struct Orientation{
			//bool valid;
			uint16_t timestamp;
			int sensor_id;
			int angle[2];//int angle (dir, pitch)
			int16_t accel[3];//acceleration x,y,z int16
			int16_t gyr[3];//gyr x,y,z, int 16
			int16_t mag[3];//mag x,y,z int 16
		};
		
		struct Luminosity{
			//bool valid;
			uint16_t timestamp;
			int sensor_id;
			int data;			
		};
		
		struct GPS{
			//bool valid;
			uint16_t timestamp;
			int sensor_id;
			float location[2];//float lat and long
		};
		
		struct All_Json{
			int validValues;//notes on which bits are for what are in the top of this file
			uint16_t topTimestamp;
			struct StateOfCharge stateOfCharge;
			struct Orientation orientation;
			struct Luminosity luminosity;
			struct GPS gps;
		};
		
		
		//combine the ts in the total json message with the ts in the sensor data
		uint32_t getTotalTS(uint32_t sensorTS){
			return (uint32_t) ((all_json.topTimestamp << 16) + sensorTS);
		} 
		
		
		
		/*
		* This see's if the value is valid by inputting the bit location.
		* This bitwise AND with the all_json valid bit with the bit location.
		* If the value is not zero, then the data is valid. Else the value is not valid.
		*/
		bool isSensorValid(int offsetAmount){
			if(offsetAmount & all_json.validValues)//valid
				return true;
			else
				return false;
		}
	
	
	
	public:
	All_Json all_json;
	
	
	Json_Message(){
		TS_OFFSET = 0x0000FFFF;
		INVALID_VALUE = -100;
		STATE_OF_CHARGE_DATA = 0x01;
		ORIENTATION_ANGLE = 0x02;
		ORIENTATION_ACCEL = 0x04;
		ORIENTATION_GYR = 0x08;
		ORIENTATION_MAG = 0x10;
		LUMINOSITY_DATA = 0x20;
		GPS_LOCATION = 0x40;
		resetJson();
	}
	
	//makes all the json messages invalid
	void resetJson(){
		all_json.validValues = 0;
		//all_json.stateOfCharge.valid = false;
		//all_json.orientation.valid = false;
		//all_json.luminosity.valid = false;
		//all_json.gps.valid = false;
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

		//std::cout << "Is this sensor valid? " << isSensorValid(STATE_OF_CHARGE_DATA) << std::endl;
		
		std::cout << "Timestamp: " << getTotalTS(all_json.stateOfCharge.timestamp) << " Sensor ID: ";
		std::cout << all_json.stateOfCharge.sensor_id << std::endl;
		
		
		
		std::cout << "Values: ";
		if(isSensorValid(STATE_OF_CHARGE_DATA)){//<< std::endl;
			std::cout << all_json.stateOfCharge.data[0] << " " << all_json.stateOfCharge.data[1] << " ";
			std::cout << all_json.stateOfCharge.data[2] << " " << all_json.stateOfCharge.data[3];
		}
		else{
			std::cout << "INVALID DATA";
		}
		std:: cout << std::endl << std::endl;
	}
	
	
	void printOrientation(){
		std::cout << "Orientation: " << std::endl;
		
		std::cout << "Timestamp: " << getTotalTS(all_json.orientation.timestamp) << " Sensor ID: ";
		std::cout << all_json.orientation.sensor_id << std::endl;
		
		
		std::cout << "Angle Values: ";
		if(isSensorValid(ORIENTATION_ANGLE)){//<< std::endl;
			std::cout << all_json.orientation.angle[0] << " " << all_json.orientation.angle[1];
		}
		else{
			std::cout << "INVALID DATA";
		}
		std::cout << std::endl;
		
		
		std::cout << "Acceleration Values: ";
		if(isSensorValid(ORIENTATION_ACCEL)){//<< std::endl;
			std::cout << all_json.orientation.accel[0] << " ";
			std::cout << all_json.orientation.accel[1] << " " << all_json.orientation.accel[2];
		}	
		else{
			std::cout << "INVALID DATA";
		}
		std::cout << std::endl;
			
		
		std::cout << "Gyr Values: ";
		if(isSensorValid(ORIENTATION_GYR)){//<< std::endl;
			std::cout << all_json.orientation.gyr[0] << " ";
			std::cout << all_json.orientation.gyr[1] << " " << all_json.orientation.gyr[2]; 
		}	
		else{
			std::cout << "INVALID DATA";
		}
		std::cout << std::endl;
	
		
		
		
		std::cout << "Mag Values: ";
		if(isSensorValid(ORIENTATION_MAG)){
			std::cout << all_json.orientation.mag[0] << " ";
			std::cout << all_json.orientation.mag[1] << " " << all_json.orientation.mag[2];
		}	
		else{
			std::cout << "INVALID DATA";
		}
		std::cout << std::endl << std::endl;
	}
	
	void printLuminosity(){
		std::cout << "Luminosity: " << std::endl;

		std::cout << "Timestamp: " << getTotalTS(all_json.luminosity.timestamp) << " Sensor ID: ";
		std::cout << all_json.luminosity.sensor_id << std::endl;
		
		std::cout << "Data: ";
		if(isSensorValid(LUMINOSITY_DATA)){//<< std::endl;
			std::cout << all_json.luminosity.data; 
		}	
		else{
			std::cout << "INVALID DATA";
		}
		std::cout << std::endl << std::endl;
	}
	
	
	void printGPS(){
		std::cout << "GPS: " << std::endl;
		std::cout << "Timestamp: " << getTotalTS(all_json.gps.timestamp) << " Sensor ID: " << all_json.gps.sensor_id << std::endl;
		
		
		std::cout << "Location: ";
		if(isSensorValid(LUMINOSITY_DATA)){//<< std::endl;
			std::cout << all_json.gps.location[0] << " " << all_json.gps.location[1]; 
		}	
		else{
			std::cout << "INVALID DATA";
		}
		std::cout << std::endl << std::endl;
	}
	
	
	/****************************************************************************************************************
	* Set Methods
	****************************************************************************************************************/
	void set_ts_json(uint32_t ts){
		all_json.topTimestamp = (ts&0xFFFF0000)>>16;
	}
	
	void setLumSensor(uint32_t ts, int id, int tempData){
		all_json.validValues |= LUMINOSITY_DATA;//make lum sensor value valid
		set_ts_json(ts);
		all_json.luminosity.timestamp = (ts&TS_OFFSET);
		all_json.luminosity.sensor_id = id;
		all_json.luminosity.data = tempData;
	}
	
	void setSOCSensor(uint32_t ts, int id, int inputData[4]){
		all_json.validValues |= STATE_OF_CHARGE_DATA;
		set_ts_json(ts);
		all_json.stateOfCharge.timestamp = (ts&TS_OFFSET);
		all_json.stateOfCharge.sensor_id = id;
		
		all_json.stateOfCharge.data[0] = inputData[0];
		all_json.stateOfCharge.data[1] = inputData[1];
		all_json.stateOfCharge.data[2] = inputData[2];
		all_json.stateOfCharge.data[3] = inputData[3];
	}
			
	void setOriSensor(uint32_t ts, int id, int inputAngle[2], int16_t inputAcc[3], int16_t inputGyr[3], int16_t inputMag[3]){
		set_ts_json(ts);
		all_json.orientation.timestamp = (ts&TS_OFFSET);
		all_json.orientation.sensor_id = id;
		
		//if the program goes through any of these if statements that means that part of the message is valid
		if(inputAngle[0] != INT_MAX){
			all_json.validValues |= ORIENTATION_ANGLE;
			all_json.orientation.angle[0] = inputAngle[0];
			all_json.orientation.angle[1] = inputAngle[1];			
		}
		if(inputAcc[0] != SHRT_MAX){
			all_json.validValues |= ORIENTATION_ACCEL;
			all_json.orientation.accel[0] = inputAcc[0];
			all_json.orientation.accel[1] = inputAcc[1];
			all_json.orientation.accel[2] = inputAcc[2];			
		}
		if(inputGyr[0] != SHRT_MAX){
			all_json.validValues |= ORIENTATION_GYR;
			all_json.orientation.gyr[0] = inputGyr[0];
			all_json.orientation.gyr[1] = inputGyr[1];
			all_json.orientation.gyr[2] = inputGyr[2];
		}
		if(inputMag[0] != SHRT_MAX){
			all_json.validValues |= ORIENTATION_MAG;
			all_json.orientation.mag[0] = inputMag[0];
			all_json.orientation.mag[1] = inputMag[1];
			all_json.orientation.mag[2] = inputMag[2];
		}	
	}
	
	void setGPSSensor(uint32_t ts, int id, float inputLoc[2]){
		all_json.validValues |= GPS_LOCATION;
		set_ts_json(ts);
		all_json.gps.timestamp = (ts&TS_OFFSET);
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
	//blank if the value is invalid
	std::string getSOCValue(){
		if(isSensorValid(STATE_OF_CHARGE_DATA)){//all_json.stateOfCharge.valid == true){
			std::stringstream sstm;
			sstm << getTotalTS(all_json.stateOfCharge.timestamp) << "," << all_json.stateOfCharge.sensor_id << ",";
			
			sstm << all_json.stateOfCharge.data[0] << "," << all_json.stateOfCharge.data[1] << ",";
			sstm << all_json.stateOfCharge.data[2] << "," << all_json.stateOfCharge.data[3];
			std::string socString = sstm.str();
			std::cout << socString << std::endl;
			return socString;
		}
		else//not valid
			return "None";
	}
			
	std::string getLumValue(){
		if(isSensorValid(LUMINOSITY_DATA)){//all_json.luminosity.valid == true){
			std::stringstream sstm;
			sstm << getTotalTS(all_json.luminosity.timestamp) << "," << all_json.luminosity.sensor_id << ",";
			sstm << all_json.luminosity.data;
			
			std::string lumString = sstm.str();
			std::cout << lumString << std::endl;
			return lumString;
		}
		else
			return "None";//not valid
	}
	
	std::string getOriValue(){
		//at least one sensor is valid
		if(isSensorValid(ORIENTATION_ACCEL | ORIENTATION_ANGLE | ORIENTATION_GYR | ORIENTATION_MAG)){//all_json.orientation.valid == true){
			std::stringstream sstm;
			sstm << getTotalTS(all_json.orientation.timestamp) << "," << all_json.orientation.sensor_id << ",";
			
			if(isSensorValid(ORIENTATION_ANGLE)){
				sstm << all_json.orientation.angle[0] << "," << all_json.orientation.angle[1] << ",";
			}
			else{
				sstm << ",,";
			}
			
			if(isSensorValid(ORIENTATION_ACCEL)){
				sstm << all_json.orientation.accel[0] << "," << all_json.orientation.accel[1] << ",";
				sstm << all_json.orientation.accel[2] << ",";
			}
			else{
				sstm << ",,,";
			}
			
			
			if(isSensorValid(ORIENTATION_GYR)){
				sstm << all_json.orientation.gyr[0] << "," << all_json.orientation.gyr[1] << ",";
				sstm << all_json.orientation.gyr[2] << ",";
			}
			else{
				sstm << ",,,";
			}
			
			
			if(isSensorValid(ORIENTATION_MAG)){
				sstm << all_json.orientation.mag[0] << "," << all_json.orientation.mag[1] << ",";
				sstm << all_json.orientation.mag[2];					
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
	
	std::string getGPSValue(){
		if(isSensorValid(GPS_LOCATION)){//all_json.gps.valid == true){
			std::stringstream sstm;
			sstm << getTotalTS(all_json.gps.timestamp) << "," << all_json.gps.sensor_id << ",";
			
			sstm << all_json.gps.location[0] << "," << all_json.gps.location[1] << ",";
			
			std::string gpsString = sstm.str();
			std::cout << gpsString << std::endl;
			return gpsString;
		}
		else
			return "None";//not valid
	}
};

