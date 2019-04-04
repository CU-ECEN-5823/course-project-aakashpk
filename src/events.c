/*
 * events.c
 *
 *  Created on: Feb 5, 2019
 *      Author: Aakash Kumar
 */

#include "events.h"


/*
 * ****** ISRs ************
 */

void button_press(uint8 flags)
{
	if(!GPIO_PinInGet(BUTTON0_PORT,BUTTON0_PIN))
		schedule_event(Task0);
	else
		schedule_event(Task1);
	gecko_external_signal(SET_GECKO_EVENT);
}



// ****** Tasks *******
//Task0
void Button_Press_Task(void)
{
	send_button_state(0x01);
}

//Task 1
void Button_Release_Task(void)
{
	send_button_state(0x00);
}


