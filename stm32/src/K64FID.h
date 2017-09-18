/**
   -
   - file:  K64FID.h
   -
   - purpose: Extracts local process unique ID
   -    which will be useful for a network of processors
   -
   - author: J. C. Wiley, Oct. 2014
   -
   - Notes:
   -      Translates internal 128 bit ID to local integer.
   -      Each kinetics device has a unique 128 bit ID (4 32 bit words).
   -      The ID is in fixed registers where a register can be accessed by a macro:
   -      SIM_UIDH, SIM_UIDMH, SIM_UIDML, SIM_UIDL defined in the mbed system
   -      header files. Simply print values:
   -           udebug.printf("0x%08X 0x%08X 0x%08X 0x%08X\n", 
   -                        SIM_UIDH, SIM_UIDMH, SIM_UIDML, SIM_UIDL);  
   -
*/
#ifndef _K64FID_H
#define _K64FID_H
#include "mbed.h"
#include <vector>

class K64FID{
public:
    /** constructor
     */
    K64FID();
    /** destructor
     */
    ~K64FID();
    /** getID
     */
    int getID();
    /** getCANaddress
	@param local id
	@return local can address
    */
    int getCANaddress(int id);
private:
    // tables of known values
    vector <uint32_t> knownUIDH;
    vector <uint32_t> knownUIDMH;
    vector <uint32_t> knownUIDML;
    vector <uint32_t> knownUIDL;
    vector <uint32_t> knownCAN;
};
#endif
