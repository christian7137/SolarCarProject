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
		
		
	public:
	Json1 json;//get error when putting this in private, cant send a temp variable
	
	Json_Message(){
		//json = (struct Json*)malloc(sizeof(Json));
		json.timestamp = 0;
	}
	/*Json_Message(1){
		//json = (struct Json*)malloc(sizeof(Json));
		json.timestamp = 0;
	}*/
	
	void printJson(){
		std::cout << "Timestamp: " << json.timestamp << std::endl;
	}

	//set methods
	void setTimestamp(int temp_ts){
		json.timestamp = temp_ts;
	}
	
	void setHeader(char tempHead[]){
		
	}
	
	void setSensor(float temp_sensor[]){
		
	}
	
	//get methods
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
