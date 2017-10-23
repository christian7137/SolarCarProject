//this is the json message format that will be sent between the pit and car RPi
//Note: Cannot use strings. When a string is created, it is a pointer to the string, not the string value.
//Thus, the value associated with the string will be pointer, which will cause a seg. fault
/*struct Json_Message{{
		int timestamp;
        char header[];
        float sensor_values[];
		
		
		char message[3];//this is for testing, remove when done testing
};*/

class Json_Message{
	private:
		
		struct Json1{
			int timestamp;
			char header[];
			float sensor_values[];		
		};
		
		
		struct Json2{
			int time;
			int header[];
			int sensor_values[];
		};
		
		struct Luminosity{
			int data;
			int sensor_id;
		};
		//value ID of sensor
		//luminosity,state of charge, orientation
		struct All_Json{
			struct Json1 json1;
			struct Json2 json2;
			struct Luminosity luminosity_message;
		};
		
	public:
	All_Json all_json;
	Json1 json;//get error when putting this in private, cant send a temp variable
	Json2 json_temp;
	Luminosity luminosity_child;
	
	Json_Message(){
		//json = (struct Json*)malloc(sizeof(Json));
		json.timestamp = 0;
		json_temp.time = 15;
		all_json.json1=json;
		all_json.json2=json_temp;
		all_json.luminosity_message=luminosity_child;		
		all_json.luminosity_message.data = 0;
		all_json.luminosity_message.sensor_id = 0;
	}
	/*Json_Message(1){
		//json = (struct Json*)malloc(sizeof(Json));
		json.timestamp = 0;
	}*/
	
	void printJson(){
		std::cout << "Timestamp: " << all_json.json1.timestamp << " Time is: " << all_json.json2.time << std::endl;
		std::cout << "Lum data: " << all_json.luminosity_message.data << " Lum ID " << all_json.luminosity_message.sensor_id << std::endl;
	}
	
	void setTime(int i){
		//std::cout << "VALUE FROM TIME IS: " << i << std::endl;
		json_temp.time = i;
	}
	void setAllJson(){
		all_json.json1=json;
		all_json.json2=json_temp;
	}
	//set methods
	void setLumData(int i){
		luminosity_child.data = i;
		all_json.luminosity_message = luminosity_child;
	}
	
	void setLumID(int i){
		luminosity_child.sensor_id = i;
		all_json.luminosity_message = luminosity_child;
	}
	
	void setTimestamp(int temp_ts){
		//std::cout << "VALUE FROM TIMESTAMP IS: " << temp_ts << std::endl;
		json.timestamp = temp_ts;
	}
	
	void setHeader(char tempHead[]){
		
	}
	
	void setSensor(float temp_sensor[]){
		
	}
	
	//get methods
	int getLumData(){
		return all_json.luminosity_message.data;
	}
	
	int getLumID(){
		return all_json.luminosity_message.sensor_id;
	}
	int getTimestamp(){
		return json.timestamp;
	}
	
	char* getHeader(){//DONT KNOW IF THIS WORKS
		return json.header;
	}
	
	float* getSenor(){//DONT KNOW IF THIS WORKS
		return json.sensor_values;
	}
};
