/*
 * swstk_i2c.h
 *
 *  Created on: Feb 3, 2019
 *      Author: aakash
 */

#ifndef SRC_SWSTK_I2C_H_
#define SRC_SWSTK_I2C_H_


void I2C_SWSTK_Init(void);

int32_t I2C_Command_Read(I2C_TypeDef *i2c, uint8_t addr, uint32_t *data,uint8_t command);

void I2C_Command_Read_NonBlocking(I2C_TypeDef *i2c, uint8_t addr, uint8_t command);

void get_I2C_read_data(int32_t *data);

#endif /* SRC_SWSTK_I2C_H_ */
