/***************************************************************
   -
   - file:  svtUSB.h
   -
   - purpose: handles connection to Beaglebone through USB.
   -      Construction parallel to svtCAN.
   -
   - author: J. C. Wiley, Sept. 2014 CAN
   -             convert to USB  Nov 2015.
   -
***************************************************************/
#ifndef _SVTUSB_H_
#define _SVTUSB_H_
#include "mbed.h"
#include "rtos.h"

#include "k64f_usb_helper.h"
#include "svtSignals.h"
#include "USBK64F.h"

//#include "svtSensorMgr.h"

#define USBVENDORID 0x64f0   // made up not to conflict with known values - for K64F
#define USB_TIMEOUT   0x250        // ms - test that we are still communicating

extern RawSerial udebug;  // crutch for testing

class svtSensorMgr;
/** svtUSB class
 *  
 */
class svtUSB{ 
public:
/** svtUSB - constructor
 */
svtUSB(): _thread(&svtUSB::threadStarter, this, osPriorityNormal, 2024) 
	{
	    OUTqueueCnt = 0;
            sensorMgrP = 0;
         };

/** svtUSB - destructor
 */
    virtual ~svtUSB();

/** init - initialize bus, sets USB vendor and product ID and links to svtSensorMgr
 */
    virtual int init(uint16_t vendor_id, uint16_t product_id, svtSensorMgr* smgr);


/** threadStarter - thread starter function to insure thread is 
 *   started after class fully intialized, must be static
 */
    static void threadStarter(void const *p);
 
/** usbloop - thread execution
 */
    void usbloop();
        
/** handleInMsg - retrieve msg from usbhid  and put in queue
 */
    virtual int handleInMsg();

/** handleOutMsg - retrieve msg from OUTqueue and send USB message.
 */
    virtual int handleOutMsg();

/** processInMsg - messages in input pool, deal with one message.
 */
    int processInMsg();

/** statusTest - hit message timeout, test to see if need to restart
 */
    int statusTest();

/** sendOutMsg - sensors usb put USB message in output queue.
 */
    int sendOutMsg(USB_Message& m);



/** copy for USB_Message
 *
 */
void copymsg(USB_Message& toMsg, USB_Message& fromMsg);

/** getUSBout - number of messages sent
 *
 */
int getUSBout(){ return msgSentCnt;}

/** getUSBin -- number of USB messages received
*
*/
int getUSBin(){return msgRecCnt;}
      
protected:          
    Thread        _thread;
    osThreadId    _threadId;
    Mail<USB_Message, 16>  INqueue;
    Mail<USB_Message, 16>  OUTqueue;  
    Mutex          UsbOutQueue_mutex;
    Mutex          USBInQueue_mutex;  

    int            OUTqueueCnt; // number waiting to be sent
    svtSensorMgr*  sensorMgrP;
    int            msgSentCnt;  // counts messages sent
    int            msgRecCnt;   // counts messages received
    USBK64F*       hid;  // usb hid
    HID_REPORT     send_report;
    HID_REPORT     recv_report;
    USB_Message   _usbmsg; // single out USB message 
};
#endif

