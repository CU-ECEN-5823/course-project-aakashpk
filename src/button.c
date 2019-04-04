/*
 * button.c
 *
 *  Created on: Mar 12, 2019
 *      Author: aakash
 */

#include "button.h"


void buttonInit()
{
	/*
	 *Setup PF6 (PB0 on board) as input
	 * and setup interrupts on both rising and falling edge
	 * this raises interrupts on button press and release.
	 */
	GPIO_PinModeSet(BUTTON0_PORT, BUTTON0_PIN, gpioModeInput, true);
	GPIO_ExtIntConfig(BUTTON0_PORT,BUTTON0_PIN,BUTTON0_PIN,true,true,true);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	/* For PB1 - unused for now
	 * Interrupt not enabled, use GPIO_IntEnable(1<<BUTTON1_PIN)
	 * or set last parameter of extintconfig to true to enable interrupts
	 */
	GPIO_PinModeSet(BUTTON1_PORT, BUTTON1_PIN, gpioModeInput, true);
	GPIO_ExtIntConfig(BUTTON0_PORT,BUTTON1_PIN,BUTTON1_PIN,true,false,false);
	//enable interrupts in NVIC using
	//NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

// Removed ISR register functions, using the Si Labs gpiointerrupt.h functions instead.
