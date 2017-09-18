/**
   -
   - file:  K64FID.cpp
   -
   - purpose: Extracts local process unique ID
   -    which will be useful for a network of processors
   -
   - author: J. C. Wiley, Oct. 2014
   -
   - Notes:
   -     Translate internal 128 bit ID to local integer.
   -     
   -
*/
#include "K64FID.h"
extern RawSerial udebug;

/** Constructor
 */
K64FID::K64FID(){
 
    // Texsun SensorOne module 
    knownUIDH.push_back(0x0001FFFFF);
    knownUIDMH.push_back(0xFFFFFFFF);
    knownUIDML.push_back(0x4D441504);
    knownUIDL.push_back( 0x9016001D);
    knownCAN.push_back(0x1);
    
    // Spare MotorControl shield
    knownUIDH.push_back(0x00011FFFF);
    knownUIDMH.push_back(0xFFFFFFFF);
    knownUIDML.push_back(0x4D441504);
    knownUIDL.push_back (0x90130013);
    knownCAN.push_back(0x2);
    //
    knownUIDH.push_back(0x00011FFFF);
    knownUIDMH.push_back(0xFFFFFFFF);
    knownUIDML.push_back(0x4D441504);
    knownUIDL.push_back( 0x9016002C);
    knownCAN.push_back(0x3);
    
    // Texsun MC module
    knownUIDH.push_back(0x00016FFFF);
    knownUIDMH.push_back(0xFFFFFFFF);
    knownUIDML.push_back(0x4D441504);
    knownUIDL.push_back( 0x9016000F);
    knownCAN.push_back(0x4);
    //
    knownUIDH.push_back(0x0002DFFFF);
    knownUIDMH.push_back(0xFFFFFFFF);
    knownUIDML.push_back(0x4D441504);
    knownUIDL.push_back( 0x90160024);
    knownCAN.push_back(0x5);
    //    
    knownUIDH.push_back(0x0002DFFFF);
    knownUIDMH.push_back(0xFFFFFFFF);
    knownUIDML.push_back(0x4D441504);
    knownUIDL.push_back( 0x9016001B);
    knownCAN.push_back(0x6);

    //---
    knownUIDH.push_back(0x0002BFFFF);
    knownUIDMH.push_back(0xFFFFFFFF);
    knownUIDML.push_back(0x4D441504);
    knownUIDL.push_back( 0x9011001D);
    knownCAN.push_back(0x7);

    knownUIDH.push_back(0x00016FFFF);
    knownUIDMH.push_back(0xFFFFFFFF);
    knownUIDML.push_back(0x4E453103);
    knownUIDL.push_back( 0x70070005);
    knownCAN.push_back(0x8);
}

/** Destructor
 */
K64FID::~K64FID(){

}

/** getID - 
   @return - local id number from 128 bit internal ID. -1 - indicates processor not known.
*/
int K64FID::getID(){
    uint32_t UIDH, UIDMH, UIDML, UIDL;
    // lookup values in registers
    UIDH = SIM_UIDH; 
    UIDMH = SIM_UIDMH;
    UIDML = SIM_UIDML;
    UIDL = SIM_UIDL;
    
    int k = -1;
    // test low word only, until we know better
    for(unsigned int i=0; i < knownUIDL.size()+1; i++){
	if(UIDL == knownUIDL[i]){
	    udebug.printf("UIDL 0x%08X \r\n", UIDL);
	    k = knownCAN[i];
            break;
	}
    }
    return k;
}

/** getCANaddress - assign CAN address based on ID.
    @param - int ID, local id from tables, counting number.
    @return - int, CAN address
*/
int K64FID::getCANaddress(int id){
    int k = -1;
    if( id < int(knownCAN.size() +1)){
	switch(id){
	case 4:
	case 7:
	    k = 3;
	    break;
	case 2:
	case 5:
	    k = 4;
	    break;
	case 1:
	case 8:
	    k = 1;
	    break;
	default:
	    break;
	}
	udebug.printf("assigned adr %d, from id %d\r\n", k, id);
    } 
    return k;
}




