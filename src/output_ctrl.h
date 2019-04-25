/*
 * output_ctrl.h
 *
 *  Created on: Apr 15, 2019
 *      Author: aakash
 */

#ifndef SRC_OUTPUT_CTRL_H_
#define SRC_OUTPUT_CTRL_H_

#include <stdbool.h>
#include "em_gpio.h"
#include "log.h"
#include "display.h"

#define PUMP_PORT gpioPortF
#define PUMP_PIN (5U)
#define LIGHT_PORT gpioPortF
#define LIGHT_PIN (7U)

#define LIGHT_IN_MIN 0
#define LIGHT_IN_MAX 300
#define LIGHT_OUT_MIN 0
#define LIGHT_OUT_MAX 100

#define DEFAULT_SETPOINT 150
#define DEFAULT_DEADBAND 10

#define LIGHT_STATE_KEY 0x4008
#define PUMP_STATE_KEY 0x4009

typedef enum {
	undefined,
	full,
	error,
	half,
	empty
}water_level_t;


typedef struct{
	uint16_t light_setpoint;
	uint16_t deadband;
	uint8_t light_output;
} light_actuator_t;

typedef struct {
	uint16_t light_level;
	bool sensor_state; //true is sensor OK,false is sensor error
} light_sensor_t;

typedef struct {
	light_sensor_t sensor;
	light_actuator_t actuator;
}light_data_t;

typedef struct{
	water_level_t water_level;
	uint8_t pump_state;
}water_data_t;

void set_light_val(uint16_t val,bool sensor_state);
void set_water_level(uint16_t val);
void set_changed_light_setpoint(uint16_t val);

void pump_control_init();
void light_control_init();

void update_light_state(void);
void update_pump_state(void);
void update_light_setpoint(void);

void pump_on();
void pump_off();

void light_level_set();


#endif /* SRC_OUTPUT_CTRL_H_ */
