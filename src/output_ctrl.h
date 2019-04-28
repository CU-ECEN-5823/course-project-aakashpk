/*
 * output_ctrl.h
 *
 *  Created on: Apr 15, 2019
 *      Author: aakash
 */

#ifndef SRC_OUTPUT_CTRL_H_
#define SRC_OUTPUT_CTRL_H_

#include <stdbool.h>
#include "gecko_helper.h"
#include "em_gpio.h"


#define PUMP_PORT gpioPortF
#define PUMP_PIN (4U)
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
#define CONFIG_STATE_KEY 0x400A

#define SENSOR_DEFAULT_CONFIG 0x03

#define PWM_BITS 8 // Change this to change to a different bit PWM
#define PWM_MAX ((0x01<<(PWM_BITS))-1)

#define PERCENT_TO_PWM(percent) ((uint32_t)((percent/100.0)*PWM_MAX))

typedef enum {
	undefined0,
	full,
	error,
	half,
	empty
}water_level_t;

typedef struct{
	uint16_t light_setpoint;
	uint8_t deadband;
	uint8_t light_output; //output value in %
} light_actuator_t;

typedef struct {
	float light_level;
	bool reliable; //true is sensor OK,false is sensor error
} light_sensor_t;

typedef struct {
	light_sensor_t sensor;
	light_actuator_t actuator;
}light_data_t;

typedef enum {
	undefined1,
	Auto,
	Manual_On,
	Manual_Off
}pump_mode_t;

typedef struct{
	water_level_t water_level;
	uint8_t pump_state;
	pump_mode_t pump_mode;
}water_data_t;


/*
 * Actuator Init functions
 */
void pump_control_init();
void light_control_init();

/*
 * State update functions to be called
 * from events
 */
void update_light_state(void);
void update_light_setpoint(void);
void update_light_deadband(void);

void update_pump_state(void);

/*
 * Config change functions
 */
void update_config(void);
uint8_t config_init(void);
void set_changed_config(uint8_t val);

/*
 * Actuator functions
 */

void pump_on();
void pump_off();

void light_level_set(void);

/*
 * Setter functions for static variables
 */
void set_light_val(float val,bool sensor_state);
void set_changed_light_setpoint(uint16_t val);
void set_changed_light_deadband(uint8_t val);
void set_water_level(uint16_t val);
void set_pump_mode(pump_mode_t val);



#endif /* SRC_OUTPUT_CTRL_H_ */
