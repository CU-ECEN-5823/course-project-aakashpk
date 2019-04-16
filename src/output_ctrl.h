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

#define PUMP_PORT gpioPortF
#define PUMP_PIN (6U)
#define LIGHT_PORT gpioPortF
#define LIGHT_PIN (7U)

#define LIGHT_IN_MIN 10
#define LIGHT_IN_MAX 50
#define LIGHT_OUT_MIN 0
#define LIGHT_OUT_MAX 100


void pump_control_init();
void light_control_init();

void pump_on();

void pump_off();

void light_level_set();


#endif /* SRC_OUTPUT_CTRL_H_ */
