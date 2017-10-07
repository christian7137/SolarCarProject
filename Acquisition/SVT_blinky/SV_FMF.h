/***************************************************
 *
 * File: SV_FMF.h
 *
 * Purpose: Defines constants for FMF message protocol
 *
 * Authors: Fred Engelkemeir, Benton Greene,
 *               J. C. Wiley, Mar 2013, From Fred's original notes.
 *
****************************************************/
#ifndef _SV_FMF_H_
#define _SV_FMF_H_

// device IDs
#define FMFID_BPS 1               //  Same for PC GUI and Controller
#define FMFID_MOTORCONTROLLER 2   // (RS232 interface) for PC GUI
#define FMFID_DASHBOARD 3         // For switches and LEDs
#define FMFID_LCD   4             // (a virtual device on the Dash)
#define FMFID_PIC24  5            // PIC24A Micro(KM) Talk directly to the KM
#define FMFID_TRACKERS 6          // Array data, KM virtual device
#define FMFID_MOTOR 7             // Motor, KM virtual device
#define FMFID_TPMS  8             // PressurePro, KM virtual device
#define FMFID_IMU   9
#define FMFID_BBONE 10            // Beaglebone
#define FMFID_GPS	11
#define FMFID_PIC24B 12           // PIC24B second PIC24HJ for motor controllers
#define FMFID_DISPLAY 13          // PIC24D - connected to dashboard uart
#define FMFID_PIC24C 14             // PIC24C - Sensor board

// message constants
#define FMF_START 0x01
#define FMF_STOP  0xFE

// commands
// TMC output messages
#define FMFCMD_TMC_1 101     // standard message from TMC
#define FMFCMD_TMC_2 102     // special message from TMC

// Bb to BPS messages
#define FMFCMD_BB_BPS_START     1  // Start measuring
#define FMFCMD_BB_BPS_STOP      2  // Stop measuring (Contactor must be off)
#define FMFCMD_BB_BPS_SYSON     3  // Turn on system (Contactor)
#define FMFCMD_BB_BPS_SYSOFF    4  // Turn off system (Contactor)
#define FMFCMD_BB_BPS_RESET     5  // Reset trip
#define FMFCMD_BB_BPS_GETREG    6  // Read register
#define FMFCMD_BB_BPS_SETREG    7  // Set register
#define FMFCMD_BB_BPS_GETFRZF   8  // Read FreezeFrame
#define FMFCMD_BB_BPS_GETHEALTH 9  // Read Health
#define FMFCMD_BB_BPS_IDENT   100  // Ask what system version
#define FMFCMD_BB_BPS_PASS    101  // Passthrough Talk to slaves direct
#define FMFCMD_BB_BPS_GETRTC  102  // Read RTC Read the time/date
#define FMFCMD_BB_BPS_PUTRTC  103  // Write RTC Set the clock struct
#define FMFCMD_BB_BPS_NONE     99  // Do nothing

// BPS to Bb FMF messages
#define FMFCMD_BPS_BB_STATUS      1  // BPS status
#define FMFCMD_BPS_BB_VOLTS       2  // Voltages Measurements
#define FMFCMD_BPS_BB_TEMPS       3  // Temperatures Measurements
#define FMFCMD_BPS_BB_BALANCING   4  // Balancing Status Measurements
#define FMFCMD_BPS_BB_BOOTSTRAP   5  // All bootstrap info
#define FMFCMD_BPS_BB_REGVAL      6  // Register data
#define FMFCMD_BPS_BB_SYSHEALTH   7  // System Health
#define FMFCMD_BPS_BB_FFSTAT      8  // FreezeFrame status
#define FMFCMD_BPS_BB_FFVOLT      9  // FreezeFrame voltages
#define FMFCMD_BPS_BB_FFTEMP     10  // FreezeFrame temperatures
#define FMFCMD_BPS_BB_SOC        11  // State of charge
#define FMFCMD_BPS_BB_ID        100  // System version
#define FMFCMD_BPS_BB_Pass      101  // Pass through response
#define FMFCMD_BPS_BB_RTC       102  // RTC struct


#define FMFCMD_BB_TMC_1 103   // message from Bb to TMC

// Bb to dashboard
#define FMFCMD_BB_DB_LEDS 1   // LEDs command message from Bb to Dashboard
#define FMFCMD_BB_DB_BEEP 2   // Beep command message from Bb to Dashboard
#define FMFCMD_BB_DB_LCD 1    // LCD update command from Bb to (LCD) Dashboard

// Dashboard output message
#define FMFMSG_DB_BB 107      // Standard message from Dashboard

// BB to Display (no return message possible)
#define FMFMSG_BB_DISPLAY  108 // All data display needs - ref interval
#define FMFMSG_BB_DISPLAYQ 109 // Short msg for screen number update - as needed

// CAN addresses
// Bb to TMC
#define BBTMCHI1 0x0E0
#define BBTMCHI2 0x0E1
#define BBTMCLO1 0x0E3
#define BBTMCL02 0x0E4

// TMC to Bb
#define TMCBBSTD 0x0F0
#define TMCBBDG1 0x0F1
#define TMCBBDG2 0x0F2
#define TMCBBDG3 0x0F3
#define TMCBBDG4 0x0F4

// PIC24 to MPPT
#define PIC24MPPT1  0x711
#define PIC24MPPT2  0x712
#define PIC24MPPT3  0x713
#define PIC24MPPT4  0x714

// MPPT to PIC24
#define MPPT1PIC24 0x771
#define MPPT2PIC24 0x772
#define MPPT3PIC24 0x773
#define MPPT4PIC24 0x774

// PIC24A (kludge) to Bb
#define FMFMSG_24_BB      201
#define FMFMSG_24_BB_GPS  202
#define FMFMSG_24_BB_MPPT 203
#define FMFMSG_24_BB_TPMS 204

// Bb to PIC24A (kludge)
#define FMFMSG_BB_24_CMD        225  // general pic24
#define FMFMSG_BB_TPMS_ADDAIR   226  // add air in current position
#define FMFMSG_BB_TPMS_CLEARAIR 227  // clear current position
#define FMFMSG_BB_TPMS_ENDADD   228  // stop waiting for air(tire)
#define FMFMSG_BB_TPMS_CURRENT  230  // set current time (1 argument)
#define FMFMSG_BB_TPMS_NONE     229  // do nothing
#define FMFMSG_BB_TPMS_BLKWRITE 231  // write 5 bytes to indicated TPMS location
#define FMFMSG_BB_TPMS_SMART    232  // wakeup or smart mode


// PIC24B (motor) to Bb
#define FMFMSG_24B_BB_CONTROL  205
#define FMFMSG_24B_BB_INSTR    207
#define FMFMSG_24B_BB_RAW      208
#define FMFMSG_24B_BB_NGM_CTRL 209
#define FMFMSG_24B_BB_INTEG    210

// Bb to PIC24B (motor)
#define FMFMSG_BB_PIC24B_CONTROL 206

// PIC24C (wing) to BB
#define FMFMSG_24C_BB_IMU      151
#define FMFMSG_24C_BB_TEMP     152
#define FMFMSG_24C_BB_CONFIG   154
#define FMFMSG_24C_BB_TIME     156

// BB to PIC24C
#define FMFMSG_BB_24C_MSG      153
#define FMFMSG_BB_24C_SETCLK   155


#endif

