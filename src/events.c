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



// ****** Sensor Tasks *******

//Task0
void log_temp_task(void)
{
	static uint8_t state = 0, water = 1;
	int32_t temp;
	uint32_t humidity;
	get_temp_humidity(&temp, &humidity);
	LOG_INFO("Temp %f Water %d State %x",temp/1000.0,water,state);

	SEND_LIGHT_DATA(temp,0);
	SEND_WATER_DATA(water,3);
	water++;
	if(water>4) water =1;
//	send_button_state(state);
	state = state ^ 0x01;
}

//Task1
void Button_Press_Task(void)
{
	LETIMER_start_intr();
}

// ****** Actuator Tasks *******

//Task0
void light_actuator_task(void)
{
	update_light_state();
}

//Task 1
void pump_actuator_task(void)
{
	update_pump_state();

}

//Task 2
void light_setpoint_change_task(void)
{
	LOG_INFO("Light setpoint changed");
	update_light_setpoint();
}

