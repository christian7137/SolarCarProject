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
		struct Json{
			int timestamp;
			char header[];
			float sensor_values[];
			
			
			char message[3];//this is for testing, remove when done testing
		};
	
	public:
	
	
	
	Json json;
	
	Json_Message(){
		//json = (struct Json*)malloc(sizeof(Json));
		json.timestamp = 2;
	}
		
	void printJson(){
		std::cout << "Timestamp: " << json.timestamp << std::endl;
	}

	void setTimestamp(int tempTS){
		json.timestamp = tempTS;
	}
};
