/*
 * output_ctrl.c
 *
 *  Created on: Apr 15, 2019
 *      Author: aakash
 */

#include "output_ctrl.h"

void pump_control_init()
{
	GPIO_DriveStrengthSet(PUMP_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(PUMP_PORT, PUMP_PIN, gpioModePushPull, false);

	/*
	 * Read last value from NVM and
	 * set the pump to that value on restart
	 */
	//gpioLed0SetOff();
	//gpioLed1SetOff();

}


/* TODO:
 * Should pump values be written to NVM
 * every time state changes ?
 * Too many writes into flash ??
 */
void pump_on()
{
	GPIO_PinOutSet(PUMP_PORT,PUMP_PIN);
	LOG_INFO("Pump turned ON");
}

void pump_off()
{
	GPIO_PinOutClear(PUMP_PORT,PUMP_PIN);
	LOG_INFO("Pump turned OFF");
}

bool light_output_calc()
{

}

void light_level_set()
{
	// Set MOSFET PWM value here
	LOG_INFO("Light Level is now");
}
