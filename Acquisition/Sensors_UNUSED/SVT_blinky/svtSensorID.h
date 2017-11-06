/**********************************************************
 *
 * file - svtSensorID.h
 *
 * purpose - single place to number the sensors for both k64f and Beaglebone
 *
 * authon - J. C. Wiley, June 2015

***********************************************************/

// Note:  sensor ID numbers greater than 7, are not read from the CAN bus by the K64Fs.
//        Have not tested Beaglebone yet.

typedef enum _svtSensorType { GreenLedSensorID =  0x0C,
			                  FXOS8700CQID     =  0x0B,
                              svtGPSsensorID   =  0x08,
                              svtTPMSsensorID  =  0x02,
                              svtMPPTsensorID  =  0x07,
                              svtIMUsensorID   =  0x04,
                              svtDASHsensorID  =  0x05,
                              svtBPSsensorID   =  0x06,
                              svtCNTRLsensorID =  0x0A,
                              svtGAPsensorID   =  0x01,
                              svtNGMsensorID   =  0x09
}svtSensorType_t;
