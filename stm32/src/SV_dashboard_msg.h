/***************************************************
 * 
 * File: SV_dashboard_msg.h
 *
 * Purpose: Sets some definitions for bitmasks in the
 *          dashboard messages
 *
 * Author: B. R. Greene, Mar 2013
 *
 ****************************************************/

//Define button state bitmasks
#define DB_CRUISE_BUTTON 	0b00000001
#define DB_HOME_BUTTON 		0b00000010
#define DB_BACK_BUTTON 		0b00000100
#define DB_SELECT_BUTTON 	0b00001000
#define DB_R_BUTTON 		0b00010000
#define DB_L_BUTTON 		0b00100000
#define DB_D_BUTTON 		0b01000000
#define DB_U_BUTTON 		0b10000000
//Define LED state bitmasks
#define DB_LED_LV		1
#define DB_LED_BPSG		2
#define DB_LED_BPSR		4
#define DB_LED_PRECHG	8
#define DB_LED_MOTORG	16
#define DB_LED_MOTORR	32
#define DB_LED_REVERSE	64
#define DB_LED_BRAKE	128
//Define ignition state bitmasks
#define DB_IGN_BIT_1		1
#define DB_IGN_BIT_2		2
#define DB_IGN_STATE_1		0
#define DB_IGN_STATE_2		1
#define DB_IGN_STATE_3		2
#define DB_IGN_STATE_4		3
//Define bitmasks for various other states
#define SV_REVERSE	4
#define SV_BRAKE	8
#define SV_BLINKER	16
#define DB_LCD_BUSY	32
#define DB_LCD_BUFFER_FULL	64
