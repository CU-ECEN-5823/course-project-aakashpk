/*
 * events.h
 *
 *  Created on: Feb 5, 2019
 *      Author: aakash
 */

#ifndef SRC_EVENTS_H_
#define SRC_EVENTS_H_

#include "log.h"
#include "Si7021_I2C.h"
#include "letimer.h"
#include "scheduler.h"
#include "gecko_helper.h"
#include "display.h"

#define READ_TEMP_EVENT 0
#define READ_HUMIDITY_EVENT	1

// Event schedule from ISR
void button_press(uint8 flags);

// Events / Tasks
void Button_Press_Task(void);
void Button_Release_Task(void);
#endif /* SRC_EVENTS_H_ */
