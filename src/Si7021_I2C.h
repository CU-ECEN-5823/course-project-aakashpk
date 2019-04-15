/*
 * Si7021_I2C.h
 *
 *  Created on: Feb 3, 2019
 *      Author: aakash
 */

#ifndef SRC_SI7021_I2C_H_
#define SRC_SI7021_I2C_H_

#include "em_gpio.h"
#include "swstk_i2c.h"
#include "log.h"
#include "infrastructure.h"
#include "gpio.h"

#define I2CPORT_USED I2C0

#define SI7021_ADDR (0x40)

/** Si7013 Read Temperature Command */
#define SI7013_TEMP_PREV       0xE0  /* Read previous T data from RH measurement
                                      * command*/
#define SI7013_RH         0xE5  /* Perform RH (and T) measurement. */
#define SI7013_TEMP      0xE3 	// Measure and get temperature


#define SENSOR_ENABLE 1
#define SENSOR_DISABLE 0


void temp_sensor_init(void);

void measure_temp_non_blocking(void);

int32_t get_temp_val(void);

void get_transfer_error(void);

int32_t get_temp(int32_t *tdata);

int32_t get_humidity(uint32_t *rhData);

int32_t get_temp_humidity(int32_t *tdata, uint32_t *rhData);

#endif /* SRC_SI7021_I2C_H_ */
