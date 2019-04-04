/*
 * button.h
 *
 *  Created on: Mar 12, 2019
 *      Author: aakash
 */

#ifndef SRC_BUTTON_H_
#define SRC_BUTTON_H_

#include "em_gpio.h"
#include "common_helper.h"
#include "gpio.h"


void buttonInit(void);
void button0_press_register_ISR(void (*_isr_func)());
void button0_release_register_ISR(void (*_isr_func)());

//void GPIO_ODD_IRQHandler();
//void GPIO_EVEN_IRQHandler();

#endif /* SRC_BUTTON_H_ */
