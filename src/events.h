/*
 * events.h
 *
 *  Created on: Feb 5, 2019
 *      Author: aakash
 */

#ifndef SRC_EVENTS_H_
#define SRC_EVENTS_H_

#include "log.h"
#include "letimer.h"
#include "scheduler.h"
#include "gecko_helper.h"
#include "display.h"
#include "Si7021_I2C.h"
#include "sensor_node_mesh.h"
#include "actuator_node_mesh.h"

#define READ_TEMP_EVENT 0
#define READ_HUMIDITY_EVENT	1

// Event schedule from ISR
void log_temp(void);
void button_press(uint8 flags);

// Events / Tasks

//Sensor
void log_temp_task(void);
void Button_Press_Task(void);

//Actuator
void light_actuator_task(void);
void pump_actuator_task(void);
void light_setpoint_change_task(void);


#endif /* SRC_EVENTS_H_ */
