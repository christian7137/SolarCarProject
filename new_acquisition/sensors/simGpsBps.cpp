#include "simGpsBps.h"


/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
/**************************************************************************/
/*!
    @brief  Instantiates a new simulated GPS BPS sensor class
*/
/**************************************************************************/
simGpsBps::simGpsBps( int sensorID, int periodMs, PinName uartRxPin, PinName gpioPin) : svtSensor(sensorID, periodMs, 1.0), uart(HAT_UNUSED, uartRxPin), gpio_request_data(gpioPin)
{
}
simGpsBps::~simGpsBps(){}
/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

void simGpsBps::init(){
    // perform initalization if need be
    // gpio set up
    gpio_request_data = 1;
    
}

//same read sensor function but this can handle timeouts
void simGpsBps::readSensor(timeout_state* pToState, char* pData){
    //tell board that we want data
    gpio_request_data = 0;
    while(uart.readable() == false){}
    gpio_request_data = 1;
    
    //we just want to keep printing characters from the device out to the usb serial port
    char curr;
    char line[255];
    int a=0;
    do{
        curr = uart.getc();
        line[a] = curr;
        a++;
    }while((curr != '\n') && (a<255));
    line[a] = 0;
    //pc.printf("%s\r", line);
    
    char * pCh;
    SIM_GPS_BPS_DATA * pStruct = (SIM_GPS_BPS_DATA *) pData;
    float lat,lon;
    
    pCh = strtok(line, ",");
    lat = atof(pCh);
    pCh = strtok(NULL, ",");
    lon = atof(pCh);
    
    pStruct->fields.lat = lat;
    pStruct->fields.lon = lon;
}
 
void simGpsBps::readSensor(char* pData){  

}
 
int simGpsBps::getSizeOfData(){
    return (sizeof(SIM_GPS_BPS_DATA));
}


/*!
 * \brief Continues to sample the sensor and print the data
 */
void simGpsBps::selftest(){
    gpio_request_data = 1;
    while(1) {
        gpio_request_data = 0;
        while(uart.readable() == false){}
        gpio_request_data = 1;
        //we just want to keep printing characters from the device out to the usb serial port
        char curr;
        do{    
            curr = uart.getc();
            pc.putc(curr);
        }while(curr != '\n');
        pc.printf("\r");
        wait(1);
    }  
}