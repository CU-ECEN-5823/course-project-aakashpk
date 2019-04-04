/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 *
 *      Edited by: Aakash Kumar
 */
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>
#include "display.h"
#include "displayhalconfig.h"


void gpioInit()
{
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);

	gpioLed0SetOff();
	gpioLed1SetOff();

}


void gpioEnableDisplay(void)
{
	GPIO_DriveStrengthSet(LCD_PORT_DISP_PWR, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LCD_PORT_DISP_PWR, LCD_PIN_DISP_PWR, gpioModePushPull, false);
}


void gpioSetDisplayExtcomin(bool high)
{
	if(high) GPIO_PinOutSet(LCD_PORT_EXTCOMIN,LCD_PIN_EXTCOMIN);
	else GPIO_PinOutClear(LCD_PORT_EXTCOMIN,LCD_PIN_EXTCOMIN);
}
