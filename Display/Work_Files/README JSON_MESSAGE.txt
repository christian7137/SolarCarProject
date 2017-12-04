README
Note: This works for only numbers. This does not work if there are strings or char* in the sensor data field.
	  If char* or strings want to be used, serialize the string, send as a char*, then de-serialize.
	  
	  
For creating and updating the Json_message there are a couple things that must be done.

1. Create a class that has the following implementations:
	-Make the class a .h file so it can be easily included into the json_message.h
	-Class data -> bool valid, int timestamp, int sensor_id, Sensor Data (make these private)
	-Class Methods (make these public)
		-void setSensor
			-Sets the sensor values
	
		-std::string getSensor		
			/*Gather the data into a string to give to the python parser
			* The different data is seperated by ","
			*
			* How the string is created:
			* timestamp,sensor id,location[0],location[1] this is an example on one
			*/
		
		-void resetSensor
			-Resets the sensor
		
		-void print
			Prints the data
	
		-Constructor that calls resetSensor()
		
		-Look at the existing sensor classes to get an idea. Use these exact method names to make it
		 easy to call different sensor methods (don't have to remember method names for different sensors)
		 
2. Update json_message.h
	-Include the class in the file ( "className.h" ) with className being the name of the new sensor class
	-Create an instance of the new class in the All_Json struct
	-Add the resetSensor method in the resetJson() method
	-Add the print method in the printJson() method
		 