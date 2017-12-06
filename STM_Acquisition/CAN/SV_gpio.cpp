/****************************************************
 -
 - File: SV_gpio.cpp
 -
 - Purpose: Instantiates class for setting up and controlling
 -     a gpio pin.
 -
 - Author: J. C. Wiley, Nov 2012 -
 -
 ****************************************************/
#include "SV_stdlibs.h"
#include "SV_gpio.h"

#define SYSFS_MUX_PREFIX    "/sys/kernel/debug/omap_mux"
#define SYSFS_CLASS_PREFIX  "/sys/class/gpio/"
#define SYSFS_GPIO          "/gpio"
#define SYSFS_EXPORT       "export"
#define SYSFS_UNEXPORT     "unexport"
#define SYSFS_VALUE        "/value"
#define SYSFS_DIRECTION    "/direction"


/**************************************************
-
- Constructor
-
*************************************************/
SV_gpio::SV_gpio(){
    debug =false;
}

/**************************************************
-
- Destructor
-             unexport pin
-
*************************************************/
SV_gpio::~SV_gpio(){
	ofstream sysfsfid_unexport;

	if(debug) cout << "SV_gpio destructor called. " << endl;
	clear();

	sysfsfid_unexport.open(sysfsfile_unexport.str().c_str());
	if(sysfsfid_unexport.fail()){
		msg << "failed to open " << sysfsfile_unexport.str() << " unexport pin in destructor"<< endl;
		SV_perror(msg.str().c_str(), 0 , -2);
	}

	sysfsfid_unexport << dec << gpio_number << endl;
}




/**************************************************
-
- init
-   input:
-    pin  - the gpio pin number, this is chip offset + pin offset
-    pin_name - the mode0 name of the pin. Used for pin mux
-    mux_mode - lower 6 bits determine mode, direction, pull-up or pull-down
-    dir - pin direction: "out" or "in"
-    Note: no consistency check of the data is made
-
*************************************************/
int SV_gpio::init(int pin,  string pin_name, string direction){
	stringstream sysfsfile_export;
	stringstream sysfsfile_dir;

	ofstream sysfsfid_export;
	ofstream sysfsfid_dir;
	stringstream msg;

	gpio_number = pin;

	// construct file names
	sysfsfile_export   << SYSFS_CLASS_PREFIX << SYSFS_EXPORT;
	sysfsfile_unexport << SYSFS_CLASS_PREFIX << SYSFS_UNEXPORT;
	sysfsfile_value    << SYSFS_CLASS_PREFIX << SYSFS_GPIO << pin << SYSFS_VALUE;
	sysfsfile_dir      << SYSFS_CLASS_PREFIX << SYSFS_GPIO << pin << SYSFS_DIRECTION;

	// export pin
	sysfsfid_export.open(sysfsfile_export.str().c_str());
	if(sysfsfid_export.fail()){
		msg << "could not open " << sysfsfile_export.str() << "gpio export" << endl;
		SV_perror(msg.str().c_str(), 0, -1);
		return -2;
	}
	sysfsfid_export << gpio_number << endl;
	sysfsfid_export.close();

	// set direction
	sysfsfid_dir.open(sysfsfile_dir.str().c_str());
	if(sysfsfid_dir.fail()){
		msg << "could not open " << sysfsfile_dir.str() << "set direction" << endl;
		SV_perror(msg.str().c_str(), 0, -1);
		return -3;
	}
	sysfsfid_dir << direction << endl;
	sysfsfid_dir.close();

	clear();
	return 0;
}

/**************************************************
-
- set
-
*************************************************/
int SV_gpio::set(){
	ofstream sysfsfid_value;


	sysfsfid_value.open(sysfsfile_value.str().c_str());
	if(sysfsfid_value.fail()){
		msg << "failed to open " << sysfsfile_value.str() << "  gpio set value" << endl;
		SV_perror(msg.str().c_str(), 0, -1);
		return -4;
	}
    sysfsfid_value << "1" << endl;
    sysfsfid_value.close();
    pinstate = true;
    return 0;
}

/**************************************************
-
- clear
-
*************************************************/
int SV_gpio::clear(){
	ofstream sysfsfid_value;

	sysfsfid_value.open(sysfsfile_value.str().c_str());
	if(sysfsfid_value.fail()){
		msg << "failed to open" << sysfsfile_value.str() << " gpio clear value" << endl;
		SV_perror(msg.str().c_str(), 0, -1);
		return -5;
	}
    sysfsfid_value << "0" << endl;
    sysfsfid_value.close();
    pinstate = false;
    return 0;
}

/**************************************************
-
- read - returns true if pin is high.
-
*************************************************/
uint16_t SV_gpio::read(){
	ifstream sysfsfid_value;
	uint8_t x;

	sysfsfid_value.open(sysfsfile_value.str().c_str(), ifstream::in);
	if(sysfsfid_value.fail()){
		msg << "failed to open " << sysfsfile_value.str() << " gpio read" << endl;
		SV_perror(msg.str().c_str(), 0, -1);
	}
    x = sysfsfid_value.get();
    sysfsfid_value.close();
    if (x == '1') return 1;
    return 0;
}

/**************************************************
-
- getState
-
*************************************************/
bool SV_gpio::getState(){
	 return this->pinstate;
}

