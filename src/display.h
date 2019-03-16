/*
 * display.h
 *
 *  Created on: Jan 1, 2019
 *      Author: Dan Walkes
 *
 * Use these steps to integrate the display module with your source code:
 * 1) Add scheduler and timer events which can provide a 1Hz update for the display EXTCOMIN pin
 *  	through a call to displayUpdate().  Include your scheduler/timer header files in the top of
 *  	display.c.  #define these values in appropriate header files:
 *  	#define SCHEDULER_SUPPORTS_DISPLAY_UPDATE_EVENT 1
 *  	#define TIMER_SUPPORTS_1HZ_TIMER_EVENT	1
 *  	and customize the line timerEnable1HzSchedulerEvent(Scheduler_DisplayUpdate) to make the
 *  	appropriate call to your timer/scheduler routine to start this event.
 *
 * 2) Add functions gpioEnableDisplay() and gpioSetDisplayExtcomin(bool high) to your gpio.c and
 * 		gpio.h files, and include
 * 		#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	1
 * 		and
 *		#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		1
 *		definitions in your gpio.h file
 *		** Note that the Blue Gecko development board uses the same pin for both the sensor and display enable
 *		pins.  This means you cannot disable the sensor for load power management if enabling the display.  Your
 *		GPIO routines need to account for this **
 *
 * 3) Call displayInit() before attempting to write the display and after initializing your timer and
 * scheduler.
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#define ECEN5823_INCLUDE_DISPLAY_SUPPORT 1

#include "glib.h"

/**
 * Display row definitions, used for writing specific content based on assignment requirements.
 * See assignment text for details.
 */
enum display_row {
	DISPLAY_ROW_NAME,
	DISPLAY_ROW_BTADDR,
	DISPLAY_ROW_BTADDR2,
	DISPLAY_ROW_CLIENTADDR,
	DISPLAY_ROW_CONNECTION,
	DISPLAY_ROW_PASSKEY,
	DISPLAY_ROW_ACTION,
	DISPLAY_ROW_TEMPVALUE,
	DISPLAY_ROW_MAX,
};

#if ECEN5823_INCLUDE_DISPLAY_SUPPORT
void displayInit();
bool displayUpdate();
void displayPrintf(enum display_row row, const char *format, ... );
#else
static inline void displayInit() { }
static inline bool displayUpdate() { return true; }
static inline void displayPrintf(enum display_row row, const char *format, ... ) { row=row; format=format;}
#endif



#endif /* SRC_DISPLAY_H_ */
