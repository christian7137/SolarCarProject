/**
   -
   - file:  STM32ID.cpp
   -
   - purpose: Extracts local process unique ID
   -    which will be useful for a network of processors
   -
   - author: J. C. Wiley, March 2017
   -
   - Notes:
   -     Translate internal 96 bit ID to local integer. (K64F is 128 bits)
   -     The Unique Device Id's 96 bits or 3 32 bit words are constructed from:
   -      1 - x, y coordinates of die on waffer
   -      2 - Lot number (3 ascii characters) + waffer number in lot (typ <24)
   -      3 - Lot number (4 ascii characters)
   -      or 7 character lot number + waffer number(byte) + 
   -         x coor (2 bytes) + y coor (2 bytes)   
   -
*/
#include "STM32ID.h"
extern RawSerial udebug;

/** Constructor
 *    Database of known Nucleo boards.
 */
STM32ID::STM32ID(){ 
    // STM-1 
    knownUID.push_back(0x498cbe);
    knownCAN.push_back(0x1);
    // STM-2
    knownUID.push_back(0x27839d);
    knownCAN.push_back(0x2);
}

/** Destructor
 */
STM32ID::~STM32ID(){

}

/* hash
 * @param b - pointer to base address of Unique Device Id
 *   use last four digits of lot number shift by 4 bits  times x*100 plus y positions
 *   to compute a unique id
 **/
uint32_t STM32ID::hash(uint8_t* b){
    uint32_t h;
    h = (((b[8] -0x30) << 20) |
	 ((b[9] -0x30) << 16) |
	 ((b[10]-0x30) << 12) |
	 ((b[11]-0x30) <<  8) |
	 (b[2] << 4) | b[0] );
    return h;
}

/** getID - 
    @return - local id number from 96 bit internal ID. -1 - indicates processor not known.
*/
int STM32ID::getID(){
    // For STM-32 F429-ZI
    uint8_t* UIDBASE = (uint8_t*)0x1FFF7A10;  // 96 bit Unique Device ID Register (sec. 39.1 manual)
    uint32_t nucleoID = hash(UIDBASE);

    int k = -1;
    // look for address in database
    for(unsigned int i=0; i < knownUID.size()+1; i++){
	if(nucleoID == knownUID[i]){
	    udebug.printf("UID 0x%08X \r\n", nucleoID);
	    k = knownCAN[i];
	    break;
	}
    }

    // k = -1, id not found in database, print report
    if(k == -1){
	udebug.printf("unknown processor\r\n");
	udebug.printf("base + Unique ID, 0x%08X  0x%08X 0x%08X 0x%08X\r\n",
		      UIDBASE,  UIDBASE[2], UIDBASE[1], UIDBASE[0]);

	uint8_t* base = UIDBASE;
	char lot[7];
	uint8_t wafernum;
	uint16_t xcoor, ycoor;
	wafernum = (uint8_t)base[4];
	xcoor = (uint16_t)((base[1]<<8) + base[0]);
	ycoor = (uint16_t)((base[3]<<8) + base[2]);
	lot[0] = base[5];
	lot[1] = base[6];
	lot[2] = base[7];
	lot[3] = base[8];
	lot[4] = base[9];
	lot[5] = base[10];
	lot[6] = base[11];
	udebug.printf(" %7.7s %d (%d, %d)\r\n", lot, wafernum, xcoor, ycoor); 
	for(unsigned int i=0; i<12; i++){
	    udebug.printf(" %x %x \r\n", i, base[i]);
	}
	udebug.printf(" nucleoID %x \r\n", nucleoID);
    }
    return k;
}

/** getCANaddress - assign CAN address based on ID.
    @param - int ID, local id from tables, counting number.
    @return - int, CAN address
*/
int STM32ID::getCANaddress(int id){
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




