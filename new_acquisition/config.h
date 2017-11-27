/*
    This file is used to define which data this code will collect.
    Meaning, there will be a list of defines below,
    only one of them will be defined at any one time
    
    By commenting out all but one, the rest of the code will dynamically compile for
    the given configuration.
*/

//CONFIGURATIONS
                                //Collects data from what sensors
                                //-------------------------------
//#define CONFIG_1                //Accelerometer - BNO055, Lumen Flux - TSL
#define CONFIG_2                //Simulated GPS - Uart commanded by GPIO

//------------------------------------------------------------------------------------------
//#define EN_PRINTF

//SENSOR_IDS
#define SENSOR_ID_ACC       2   //no reason we started on 2...
#define SENSOR_ID_LIGHT     3
#define SENSOR_ID_GPS       4