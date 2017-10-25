//this is the json message format that will be sent between the pit and car RPi
//Note: Cannot use strings. When a string is created, it is a pointer to the string, not the string value.
//Thus, the value associated with the string will be pointer, which will cause a seg. fault

class Json_Message{
	private:
		
		struct StateOfCharge{
			int timestamp;
			char header[];
			float sensor_values[];		
		};
		
		
		struct Orientation{
			int time;
			int header[];
			int sensor_values[];
		};
		
		struct Luminosity{
			int data;
			int sensor_id;
		};
		
		struct All_Json{
			struct StateOfCharge stateOfCharge_message;
			struct Orientation orientation_message;
			struct Luminosity luminosity_message;
		};
		
		
		//define all the sensor structs to use 
		StateOfCharge stateOfCharge_child;
		Orientation orientation_child;
		Luminosity luminosity_child;
	
	public:
	//define the main json struct as public
	All_Json all_json;
	
	
	Json_Message(){
		stateOfCharge_child.timestamp = 0;
		orientation_child.time = 0;
		luminosity_child.data = 0;
		luminosity_child.sensor_id = 0;
		combineIntoSingleJson();
	}
	
	//put all of the structs into a single json message/struct
	void combineIntoSingleJson(){
		all_json.stateOfCharge_message=stateOfCharge_child;
		all_json.orientation_message=orientation_child;
		all_json.luminosity_message=luminosity_child;
	}
	
	void printJson(){
		//std::cout << "Timestamp: " << all_json.json1.timestamp << " Time is: " << all_json.json2.time << std::endl;
		std::cout << "Lum data: " << all_json.luminosity_message.data << " Lum ID " << all_json.luminosity_message.sensor_id << std::endl;
	}
	
	/****************************************************************************************************************
	* Set Methods
	****************************************************************************************************************/
	void setLumData(int i){
		luminosity_child.data = i;
		all_json.luminosity_message = luminosity_child;
	}
	
	void setTime(int i){
		orientation_child.time = i;
		all_json.orientation_message = orientation_child;
	}
	
	void setLumID(int i){
		luminosity_child.sensor_id = i;
		all_json.luminosity_message = luminosity_child;
	}
	
	void setTimestamp(int temp_ts){
		stateOfCharge_child.timestamp = temp_ts;
		all_json.stateOfCharge_message = stateOfCharge_child;
	}
	
	/*
	void setHeader(char tempHead[]){
		
	}
	
	void setSensor(float temp_sensor[]){
		
	}*/
	
	/****************************************************************************************************************
	* Get Methods
	****************************************************************************************************************/
	int getLumData(){
		return all_json.luminosity_message.data;
	}
	
	int getLumID(){
		return all_json.luminosity_message.sensor_id;
	}
	
	int getTimestamp(){
		return all_json.stateOfCharge_message.timestamp;
	}
	/*
	char* getHeader(){//DONT KNOW IF THIS WORKS
		return all_json.json1.header;
	}
	
	float* getSenor(){//DONT KNOW IF THIS WORKS
		return all_json.json1.sensor_values;
	}*/
};
