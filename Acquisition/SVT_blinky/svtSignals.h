/***************************************************************
   -
   - file:  svtSignals.h
   -
   - purpose: defines svt thread signals, each  thread can have
             upto 32 signals according to Keil web site:
   - https://www.keil.com/pack/doc/CMSIS/RTOS/html/group___c_m_s_i_s___r_t_o_s___signal_mgmt.html#ga87283a6ebc31ce9ed42baf3ea7e4eab6
   - author: J. C. Wiley, Dec. 2014

   -
***************************************************************/

#ifndef _SVT_SIGNALS_H_
#define _SVT_SIGNALS_H_

#define SIG_CAN_NONE              0X0000
#define SIG_CAN_START             0x0001
#define SIG_CAN_MSG_READY         0x0002
#define SIG_CAN_INQUEUE_NOTEMPTY  0x0004
#define SIG_CAN_OUTQUEUE_NOTEMPTY 0x0008
#define SIG_CAN_TX_WARNING        0X0010
#define SIG_CAN_RX_WARNING        0X0020
#define SIG_CAN_BUSSOFF           0X0040
#define SIG_CAN_BIT1ERR           0X0080
#define SIG_CAN_BIT0ERR           0X0100
#define SIG_CAN_ACKERR            0X0200
#define SIG_CAN_CRCERR            0X0400
#define SIG_CAN_FRMERR            0X0800
#define SIG_CAN_STFERR            0X1000
#define SIG_CAN_WAKEUP            0X2000

#define SIG_USB_NONE              0X0000
#define SIG_USB_START             0x0001
#define SIG_USB_MSG_READY         0x0002
#define SIG_USB_INQUEUE_NOTEMPTY  0x0004
#define SIG_USB_OUTQUEUE_NOTEMPTY 0x0008
#define SIG_USB_TX_WARNING        0X0010
#define SIG_USB_RX_WARNING        0X0020
#define SIG_USB_BUSSOFF           0X0040
#define SIG_USB_BIT1ERR           0X0080
#define SIG_USB_BIT0ERR           0X0100
#define SIG_USB_ACKERR            0X0200
#define SIG_USB_CRCERR            0X0400
#define SIG_USB_FRMERR            0X0800
#define SIG_USB_STFERR            0X1000
#define SIG_USB_WAKEUP            0X2000

#endif
