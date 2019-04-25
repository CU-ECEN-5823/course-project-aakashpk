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

static uint16_t changed_light_setpoint;
static uint8_t changed_light_deadband;


int light_actuator_state_load(void)
{
  struct gecko_msg_flash_ps_load_rsp_t* pLoad;

  pLoad = gecko_cmd_flash_ps_load(LIGHT_STATE_KEY);

  // Set default values if ps_load fail
  if (pLoad->result ) {
	LOG_WARN("Flash load failed, loading defaults");
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


uint8_t light_output_calc(uint16_t value)
{
	/*
	 * All input values are in lumens
	 * If light value is less than setpoint+deadband/2
	 * then turn on output,
	 * span output between setpoint+deadband/2 and max light value
	 *
	 */
	if(value < (light_val.actuator.light_setpoint
				+light_val.actuator.deadband/2))
	{
		return ((LIGHT_OUT_MAX-LIGHT_OUT_MIN) - ((value)/
				(float)(LIGHT_IN_MAX-
						(light_val.actuator.light_setpoint
								+light_val.actuator.deadband/2))
				*(LIGHT_OUT_MAX-LIGHT_OUT_MIN)));
	}

	else return 0;

}



void light_level_set()
{
	LOG_INFO(">>> Bulb %d%%",light_val.actuator.light_output);
	displayPrintf(LIGHT_ACTION,"Bulb: %d%%",light_val.actuator.light_output);
	// Set MOSFET PWM value here
}


void update_light_state(void)
{
	//update only if sensor data is reliable, else just log
	if(light_val.sensor.reliable)
	{
		uint8_t output_value = light_output_calc(light_val.sensor.light_level);

		/*
		 * Write value to NVM only if ouput value has changed
		 */
		if(output_value != light_val.actuator.light_output)
		{
			light_val.actuator.light_output = output_value;
			light_level_set();
			light_actuator_state_store();
		}
	}

	LOG_INFO("Light %d :%s, Setpoint %d Deadband %d Bulb %d",
					light_val.sensor.light_level,
					light_val.sensor.reliable?"reliable":"unreliable",
					light_val.actuator.light_setpoint,
					light_val.actuator.deadband,
					light_val.actuator.light_output);
}

void update_light_setpoint(void)
{
	if(changed_light_setpoint != light_val.actuator.light_setpoint)
	{
		light_val.actuator.light_setpoint = changed_light_setpoint;
		LOG_INFO("Light setpoint changed to %d",
				light_val.actuator.light_setpoint);
	}

	if(changed_light_deadband != light_val.actuator.deadband)
	{
		light_val.actuator.deadband = changed_light_deadband;
		LOG_INFO("Light deadband changed to %d",
				light_val.actuator.deadband);
	}

	// write value to flash
	light_actuator_state_store();
}

void light_control_init()
{
	/*Init sensor value to 0 and
	 * unreliable reading as default
	 */

	light_val.sensor.light_level=0;
	light_val.sensor.reliable=0;
	//Get last stored values from NVM
	light_actuator_state_load();

	LOG_INFO("Setpoint %d Deadband %d Output %d",
			light_val.actuator.light_setpoint,
			light_val.actuator.deadband,
			light_val.actuator.light_output);

	light_level_set();
}


/*
 *  Pump Control Functions
 */

int pump_state_load(void)
{
  struct gecko_msg_flash_ps_load_rsp_t* pLoad;

  pLoad = gecko_cmd_flash_ps_load(PUMP_STATE_KEY);

  // Set default values if ps_load fail
  if (pLoad->result ) {
	LOG_WARN("Flash load failed, loading defaults");
    memset(&water_val.pump_state, 0, sizeof(uint8_t));
    water_val.pump_state = 0;
    return -1;
  }

  memcpy(&water_val.pump_state, pLoad->value.data, pLoad->value.len);

  return 0;
}


int pump_state_store(void)
{
  struct gecko_msg_flash_ps_save_rsp_t* pSave;

  pSave = gecko_cmd_flash_ps_save(PUMP_STATE_KEY,
		  sizeof(uint8_t),
		  (const uint8*)&water_val.pump_state);

  if (pSave->result) {
    LOG_ERROR("pump_state_store(): PS save failed, code %x", pSave->result);
    return(-1);
  }

  return 0;
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

		pump_state_store();
	}
	LOG_INFO("Water level %d Pump State %d",
			water_val.water_level,
			water_val.pump_state);
}




void pump_control_init(void)
{
	GPIO_DriveStrengthSet(PUMP_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(PUMP_PORT, PUMP_PIN, gpioModePushPull, false);

	water_val.water_level = error; // initialize to unreliable
	/*
	 * Read last value from NVM and
	 * set the pump to that value on restart
	 */
	pump_state_load();
	water_val.pump_state ? pump_on():pump_off();
}


/* TODO:
 * Should pump values be written to NVM
 * every time state changes ?
 * Too many writes into flash ??
 */
void pump_on()
{
	/*
	 * TODO: Start a timer here,
	 * turn off pump when the timer expires
	 * in case the sensor is broken
	 */
	GPIO_PinOutSet(PUMP_PORT,PUMP_PIN);
	LOG_INFO("<<< Pump ON");
	displayPrintf(PUMP_ACTION,"PUMP ON");
}

void pump_off()
{
	GPIO_PinOutClear(PUMP_PORT,PUMP_PIN);
	LOG_INFO("<<< Pump OFF");
	displayPrintf(PUMP_ACTION,"PUMP OFF");
}




/*
 * Setter functions for static variables in file
 */
void set_light_val(uint16_t val,bool sensor_state)
{
	light_val.sensor.light_level = val;
	light_val.sensor.reliable = sensor_state;
}

void set_water_level(uint16_t val)
{
	water_val.water_level = val;
}

void set_changed_light_setpoint(uint16_t val)
{
	changed_light_setpoint = val;
}

void set_changed_light_deadband(uint8_t val)
{
	changed_light_deadband = val;
}

