/*
 * gpio.h
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>

/**
 * TODO: define these.  See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
 * and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
 */

#define	LED0_port gpioPortF
#define LED0_pin (4U)
#define LED1_port gpioPortF
#define LED1_pin (5U)

#define BUTTON0_PORT gpioPortF
#define BUTTON0_PIN (6U)
#define BUTTON1_PORT gpioPortF
#define BUTTON1_PIN (7U)


#define LED0_TOGGLE() GPIO_PinOutToggle(LED0_port,LED0_pin)
#define LED1_TOGGLE() GPIO_PinOutToggle(LED1_port,LED1_pin)

#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	1
#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		1


#define SI7021_ENABLE_PORT gpioPortD
#define SI7021_ENABLE_PIN (15U)


void gpioInit();

void gpioEnableDisplay();
void gpioSetDisplayExtcomin(bool high);

#ifndef USE_CUSTOM_BOARD
#define gpioLed0SetOn()		GPIO_PinOutSet(LED0_port,LED0_pin)
#define gpioLed0SetOff()	GPIO_PinOutClear(LED0_port,LED0_pin)
#define gpioLed1SetOn()		GPIO_PinOutSet(LED1_port,LED1_pin)
#define gpioLed1SetOff()	GPIO_PinOutClear(LED1_port,LED1_pin)
#else
#define gpioLed0SetOn()		GPIO_PinOutClear(LED0_port,LED0_pin)
#define gpioLed0SetOff()	GPIO_PinOutSet(LED0_port,LED0_pin)
#define gpioLed1SetOn()		GPIO_PinOutClear(LED1_port,LED1_pin)
#define gpioLed1SetOff()	GPIO_PinOutSet(LED1_port,LED1_pin)
#endif


#ifndef GPIO_DISPLAY_SUPPORT_IMPLEMENTED
#define temp_sensor_lpm_on() \
	GPIO_PinOutSet(SI7021_ENABLE_PORT,SI7021_ENABLE_PIN)

#define temp_sensor_lpm_off() \
	GPIO_PinOutClear(SI7021_ENABLE_PORT,SI7021_ENABLE_PIN)
#else
#define temp_sensor_lpm_on()
#define temp_sensor_lpm_off()
#endif


#endif /* SRC_GPIO_H_ */
