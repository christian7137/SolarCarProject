#include <string.h>
#include <sstream>
#include <limits.h>
#include "StateOfCharge.h"
#include "Orientation.h"
#include "Luminosity.h"
#include "GPS.h"


/*
* This is the json message format that will be sent between the Pit RPi and Solar Car RPi
*
* Note: When adding new sensors (new structs), add the struct in the All_Json Struct. Then add getter
* and setter method. Additionally, a new print method could be created, but this is just for testing purposes.
*
* Note: When adding new data to an existing struct, update the struct and update the getter and setter methods.
* This will need to be communicated to the python wrapper and the function that calls these methods needs to be updated
*/
			
class Json_Message{
	private:
		
		struct All_Json{
			class StateOfCharge soc;
			class Orientation ori;
			class Luminosity lum;
			class GPS gps;
		};
	
	public:
	All_Json all_json;
	
	
	Json_Message(){
		resetJson();
	}
	
	//makes all the json messages invalid
	void resetJson(){
		all_json.soc.resetSensor();
		all_json.ori.resetSensor();
		all_json.lum.resetSensor();
		all_json.gps.resetSensor();
	}
	
	
	/****************************************************************************************************************
	* Print Methods
	****************************************************************************************************************/
	
	//Prints all of the sensor Values
	void printJson(){
		all_json.soc.print();
		all_json.ori.print();
		all_json.lum.print();
		all_json.gps.print();
		std::cout << "----------------------------------END OF MESSAGE----------------------------------" << std::endl;
	}
	
};
