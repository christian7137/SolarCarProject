/**
   -
   - file:  STM32ID.h
   -
   - purpose: Extracts local process unique ID
   -    which will be useful for a network of processors
   -
   - author: J. C. Wiley, Oct. 2014
   -
   - Notes:
   -      Translates internal 96 bit ID to local integer.
   -      Each kinetics device has a unique 128 bit ID (4 32 bit words).
   -      The ID is in fixed registers where a register can be accessed by a macro:
   -      SIM_UIDH, SIM_UIDMH, SIM_UIDML, SIM_UIDL defined in the mbed system
   -      header files. Simply print values:
   -           udebug.printf("0x%08X 0x%08X 0x%08X 0x%08X\n", 
   -                        SIM_UIDH, SIM_UIDMH, SIM_UIDML, SIM_UIDL);  
   -
*/
#ifndef _STM32ID_H
#define _STM32ID_H
#include "mbed.h"
#include <vector>

class STM32ID{
public:
    /** constructor
     */
    STM32ID();
    /** destructor
     */
    ~STM32ID();
    /** getID
     */
    int getID();
    /** getCANaddress
	@param local id
	@return local can address
    */
    int getCANaddress(int id);
private:
    uint32_t hash( uint8_t* b); // computes has code for 96 bit Unique ID
    // tables of known values
    vector <uint32_t> knownUID;
    vector <uint32_t> knownCAN;
};
#endif
