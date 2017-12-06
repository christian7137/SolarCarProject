/****************************************************
 -
 - File: SV_gpio.h
 -
 - Purpose: defines class for setting up and controlling
 -     a gpio pin.
 -
 - Author: J. C. Wiley, Nov 2012 -
 -
 - Reference:
 -     1. https://groups.google.com/forum/?fromgroups=#!topic/beagleboard/7LveRNzTJnA
 -     2. http://www.nathandumont.com/node/250
 -
 ****************************************************/
#ifndef SVGPIO_H_
#define SVGPIO_H_

#include "SV_stdlibs.h"
class SV_gpio{
	public:
	   SV_gpio();
	   virtual ~SV_gpio();
	  // int init(int pin, int mux_mode, string pin_name, string direction);
	   int init(int pin, string pin_name, string direction);
	   int set(); // sets pin high
	   int clear(); // sets pin low
	   uint16_t read();
	   bool getState();

	private:
	   int gpio_number;
	   stringstream sysfsfile_unexport;
	   stringstream sysfsfile_value; // file name for value attribute
	   bool pinstate;
	   bool debug;
	   stringstream msg;
//	   int writeMuxStatus(stringstream &ss);
};

#endif

