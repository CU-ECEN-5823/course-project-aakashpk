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

void log_temp(void)
{
	schedule_event(Task0);
	gecko_external_signal(SET_GECKO_EVENT);
}

void button_press(uint8 flags)
{
	if(GPIO_PinInGet(BUTTON0_PORT,BUTTON0_PIN))
	{
		schedule_event(Task1);
		gecko_external_signal(SET_GECKO_EVENT);
	}
}



// ****** Tasks *******

//Task0
void log_temp_task(void)
{
	static uint8_t state = 0;
	int32_t temp;
	uint32_t humidity;
	get_temp_humidity(&temp, &humidity);
	LOG_INFO("Temp %f Humidity %f State %x",temp/1000.0,humidity/1000.0,state);

	SEND_LIGHT_DATA(temp,2);
	SEND_WATER_DATA(humidity,3);

	send_button_state(state);
	state = state ^ 0x01;
}

//Task1
void Button_Press_Task(void)
{
	LETIMER_start_intr();
}



