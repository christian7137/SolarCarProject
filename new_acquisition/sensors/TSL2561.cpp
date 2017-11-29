
#include "TSL2561.h"
#include "config.h"

extern Serial pc;

#define DEBUG_PRINTX(z,x)             if(z==1) pc.printf(x);
#define DEBUG_PRINTLNX(z,x)           if(z==1) {pc.printf(x);        pc.printf("\r\n");}
#define DEBUG_PRINTXY(z,x, y)         if(z==1) pc.printf(x, y);
#define DEBUG_PRINTLNXY(z,x, y)       if(z==1) {pc.printf(x, y);     pc.printf("\r\n");}

TSL2561::TSL2561(int sensorID, uint8_t addr, int periodMs, PinName sda, PinName scl):svtSensor(sensorID, periodMs, 1.0), i2c(sda,scl) {

  _addr = addr;
  _initialized = false;
  _integration = TSL2561_INTEGRATIONTIME_13MS;
  _gain = TSL2561_GAIN_16X;
}

TSL2561::~TSL2561(){}

int TSL2561::getSizeOfData(){
    return (sizeof(uint16_t));
}
uint16_t TSL2561::getLuminosity (uint8_t channel) {

  uint32_t x = getFullLuminosity();

  if (channel == 0) {
    // Reads two byte value from channel 0 (visible + infrared)
    
    return (x & 0xFFFF);
  } else if (channel == 1) {
    // Reads two byte value from channel 1 (infrared)
    
    return (x >> 16);
  } else if (channel == 2) {
    // Reads all and subtracts out just the visible!
    
    return ( (x & 0xFFFF) - (x >> 16) );
  }
  
  // unknown channel!
  return 0;
}

uint32_t TSL2561::getFullLuminosity (void)
{
  if (!_initialized) begin();

  // Enable the device by setting the control bit to 0x03
  enable();

  // Wait x ms for ADC to complete
  switch (_integration)
  {
    case TSL2561_INTEGRATIONTIME_13MS:
      wait_ms(14);
      break;
    case TSL2561_INTEGRATIONTIME_101MS:
      wait_ms(102);
      break;
    default:
      wait_ms(403);
      break;
  }

//DEBUG_PRINTLNXY(0," Integration:= %d",_integration);

  uint32_t x;
  x = read16(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_CHAN1_LOW);
  
 // DEBUG_PRINTLNXY(0," x:= %d",x);
  
  x <<= 16;
  x |= read16(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_CHAN0_LOW);

  //DEBUG_PRINTLNXY(0," x:= %d",x);
  
    //wait(3);
  disable();

  return x;
}

void TSL2561::init(){
    pc.printf("init\r\n");
    enable();
}
bool TSL2561::begin(void) {
    pc.printf("begin\r\n");
    char  reg[1];
    reg[0] = TSL2561_REGISTER_ID;
    char receivedata[1];
    char read;
    i2c.write(_addr<<1, reg, 1);
    i2c.read(_addr<<1, receivedata, 1);
    read=receivedata[0];
            
  if (read & 0x0A ) {
    pc.printf("Read 0x%x => Found TSL2561\r\n",read);
  } else {
    return false;
  } 
    _initialized = true;
     
    // Set default integration time and gain
    setTiming(_integration);
    setGain(_gain);
  
    // Note: by default, the device is in power down mode on bootup
    disable();    
    
    return true;
 }
 
 void TSL2561::readSensor(timeout_state* pToState, char * pData){
    uint16_t lumons;
    lumons = getLuminosity(0);
    pData[1] = (lumons>>8) & 0xff;
    pData[0] = lumons & 0xff;
    
    *pToState = finished;
    
 }
 
void TSL2561::readSensor(char * pData){

}
 uint16_t TSL2561::read16(uint8_t reg)
{
    uint16_t x; 
    uint16_t t;
    char _x;
    char _t;
    char r[1];
    r[0] = reg;
    char receivedata[2];
    
    i2c.write(_addr<<1, r, 1);
    i2c.read(_addr<<1, receivedata, 2);  
    
    _t=receivedata[0];
    _x=receivedata[1];
    
    //pc.printf("_t:=0x%x\r\n",_t);
    //pc.printf("_x:=0x%x\r\n",_x);      
   
    t=(uint16_t)_t;
    x=(uint16_t)_x;
    x <<= 8;
    x |= t;
    
    //pc.printf("x:= %d\r\n",x);      
    
    return x;
}
 
 void TSL2561::write8 (uint8_t reg, uint8_t value)
{ 
    i2c.start();
    i2c.write(_addr<<1);
    i2c.write(reg);
    i2c.write(value);
    i2c.stop(); 
}

void TSL2561::setTiming(tsl2561IntegrationTime_t integration){

if (!_initialized) begin();

else pc.printf("--------------Set Timing---------\r\n");

  enable();
  
  _integration = integration;
  
  //pc.printf("Integration: 0x%x\n",_integration);
  //pc.printf("Gain: 0x%x\n",_gain);
  //pc.printf("Integration | Gain: 0x%x\n",_integration | _gain);
  
  write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_TIMING, _integration | _gain);  
  
  disable();
  
  pc.printf("--------------Complete Set Timing-------------\r\n");
  
  //wait(1);

}

void TSL2561::setGain(tsl2561Gain_t gain) {

if (!_initialized) begin();
else    pc.printf("-------------Set Gain--------------\r\n");


  enable();
  
  //pc.printf("Intergration: 0x%x\r\n",_integration);
  //pc.printf("Gain: 0x%x\n",_gain);
  //pc.printf("Intergration | Gain: 0x%x\r\n",_integration | _gain);
  
  _gain = gain;
  write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_TIMING, _integration | _gain);  
  //write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_TIMING,  _gain);  
  disable();
  
  pc.printf("---------------Complete Set Gain----------------\r\n");
  //wait(1);
  
}

void TSL2561::enable(void)
{
  
  if (!_initialized) 
    begin();

  // Enable the device by setting the control bit to 0x03
  //pc.printf("Power On\r\n");
  write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWERON);
}

void TSL2561::disable(void)
{
  if (!_initialized) begin();

  // Disable the device by setting the control bit to 0x03
  //pc.printf("Power Off\r\n");
  write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWEROFF);
}


void TSL2561::selftest(){
    while(1){
        char data[8];
        timeout_state a;
        readSensor(&a, data);
        int16_t * pInt;
        pInt = (int16_t *) data;
        pc.printf("%d\r\n", *pInt);
        wait(1);
    }
}