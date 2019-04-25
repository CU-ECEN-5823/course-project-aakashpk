/*
 * output_ctrl.c
 *
 *  Created on: Apr 15, 2019
 *      Author: aakash
 */

#include "output_ctrl.h"

// Structures for sensor value and actuator state
static light_data_t light_val;
static water_data_t water_val;
static changed_light_setpoint;


int light_actuator_state_load(void)
{
  struct gecko_msg_flash_ps_load_rsp_t* pLoad;

  pLoad = gecko_cmd_flash_ps_load(LIGHT_STATE_KEY);

  // Set default values if ps_load fail
  if (pLoad->result ) {
    memset(&light_val.actuator, 0, sizeof(light_actuator_t));
    light_val.actuator.light_setpoint = DEFAULT_SETPOINT;
    light_val.actuator.deadband = DEFAULT_DEADBAND;
	light_val.actuator.light_output = 0;
    return -1;
  }

  memcpy(&light_val.actuator, pLoad->value.data, pLoad->value.len);

  return 0;
}

/***************************************************************************//**
 * This function saves the current light state in Persistent Storage so that
 * the data is preserved over reboots and power cycles.
 * The light state is hold in a global variable lightbulb_state.
 * A PS key with ID 0x4008 is used to store the whole struct.
 *
 * @return 0 if saving succeed, -1 if saving fails.
 ******************************************************************************/
int light_actuator_state_store(void)
{
  struct gecko_msg_flash_ps_save_rsp_t* pSave;

  pSave = gecko_cmd_flash_ps_save(LIGHT_STATE_KEY,
		  sizeof(light_actuator_t),
		  (const uint8*)&light_val.actuator);

  if (pSave->result) {
    LOG_ERROR("lightbulb_state_store(): PS save failed, code %x", pSave->result);
    return(-1);
  }

  return 0;
}


void update_light_state(void)
{
	light_val.sensor.light_level;
	//calc_new_output
	// check state changed
		//actuator change
		//update state in NVM
}

void update_pump_state(void)
{
	uint8_t pump_state;

	switch(water_val.water_level)
	{
		case full:
			pump_state = 0;
			break;
		case half:
			pump_state = 0;
		break;
		case empty:
			pump_state = 1;
			break;
		default: // in case of error, turn pump off
			pump_state = 0;
		break;
	}

	if(pump_state != water_val.pump_state)
	{
		//Update pump state
		water_val.pump_state = pump_state;
		//Turn pump on or off
		water_val.pump_state ? pump_on():pump_off();

		//TODO: Write new pump state to flash
	}
	LOG_INFO("---Water level %d Pump State %d",
			water_val.water_level,
			water_val.pump_state);
}

void update_light_setpoint(void)
{
	LOG_INFO("New Light Setpoint %d",changed_light_setpoint);

	if(changed_light_setpoint != light_val.actuator.light_setpoint)
	{
		light_val.actuator.light_setpoint = changed_light_setpoint;
		// write value to flash here
		light_actuator_state_store();
	}
}


void pump_control_init()
{
	GPIO_DriveStrengthSet(PUMP_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(PUMP_PORT, PUMP_PIN, gpioModePushPull, false);

	/*
	 * Read last value from NVM and
	 * set the pump to that value on restart
	 */
}


/* TODO:
 * Should pump values be written to NVM
 * every time state changes ?
 * Too many writes into flash ??
 */
void pump_on()
{
	/*
	 * Start a timer here,
	 * turn off pump when the timer expires
	 * in case the sensor is broken
	 */
	GPIO_PinOutSet(PUMP_PORT,PUMP_PIN);
	LOG_INFO("Pump turned ON");
	displayPrintf(PUMP_ACTION,"PUMP ON");
}

void pump_off()
{
	GPIO_PinOutClear(PUMP_PORT,PUMP_PIN);
	LOG_INFO("Pump turned OFF");
	displayPrintf(PUMP_ACTION,"PUMP OFF");
}

void set_light_val(uint16_t val,bool sensor_state)
{
	light_val.sensor.light_level = val;
	light_val.sensor.sensor_state = sensor_state;
}

void set_water_level(uint16_t val)
{
	water_val.water_level = val;
}

void set_changed_light_setpoint(uint16_t val)
{
	changed_light_setpoint = val;
}

bool light_output_calc(uint16_t light_val)
{

}

void light_level_set()
{
	// Set MOSFET PWM value here
	LOG_INFO("Light Level is now");
}
