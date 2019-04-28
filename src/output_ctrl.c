/*
 * output_ctrl.c
 *
 *  Created on: Apr 15, 2019
 *      Author: aakash
 */

#include "output_ctrl.h"

char * water_level_enum_str[5] = {"Undefined",
									"Full",
									"Error",
									"Half",
									"Empty"
};

char * pump_mode_enum_str[4] = {"Undefined",
									"Auto",
									"Manual ON",
									"Manual OFF"
};

// Structures for sensor value and actuator state
static light_data_t light_val;
static water_data_t water_val;
static uint8_t config_state;



static uint16_t changed_light_setpoint;
static uint8_t changed_light_deadband;
static uint8_t changed_config_state;


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


uint8_t light_output_calc(float value)
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
	LOG_INFO(">>> Bulb %d%% PWM: %d",
			light_val.actuator.light_output,
			PERCENT_TO_PWM(light_val.actuator.light_output));
	displayPrintf(LIGHT_ACTION,"Bulb: %d%% PWM: %d",
			light_val.actuator.light_output,
			PERCENT_TO_PWM(light_val.actuator.light_output));
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

	LOG_INFO("Light %f:%s, Setpoint:%d, Deadband:%d, Bulb:%d%%",
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


		// write value to flash
		light_actuator_state_store();
	}

}

void update_light_deadband(void)
{
	if(changed_light_deadband != light_val.actuator.deadband)
	{
		light_val.actuator.deadband = changed_light_deadband;
		LOG_INFO("Light deadband changed to %d",
				light_val.actuator.deadband);

		// write value to flash
		light_actuator_state_store();
	}


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

	LOG_INFO("Setpoint:%d Deadband:%d Output:%d",
			light_val.actuator.light_setpoint,
			light_val.actuator.deadband,
			light_val.actuator.light_output);


	// write the setpoint and deadband values to GATT chars
	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_write_attribute_value(gattdb_light_setpoint,
			  0, sizeof(uint16_t), (uint8 *)&light_val.actuator.light_setpoint));

	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_write_attribute_value(gattdb_deadband,
			  0, sizeof(uint8_t), (uint8 *)&light_val.actuator.deadband));

	//Set the read value to actual hardware
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
	uint8_t pump_state = water_val.pump_state;

	switch (water_val.pump_mode) {
		case Auto:
			switch(water_val.water_level)
			{
				case full:
					pump_state = 0;
					break;
				case half:
				/*
				 * nothing to be done here,
				 * when filling up pump should continue to be ON
				 * when pump already OFF, then pump should be OFF
				 * so just keep last value.
				 */
				break;
				case empty:
					pump_state = 1;
					break;
				default: // in case of error, turn pump off
					pump_state = 0;
				break;
			}
			break;

		case Manual_Off:
			pump_state = 0;
			break;

		case Manual_On:
			pump_state = 1;
			break;

		default:
			LOG_WARN("Unknown Pump Mode");
			break;
	}

	/*
	 * Write to flash only is state chaned.
	 */
	if(pump_state != water_val.pump_state)
	{
		//Update pump state
		water_val.pump_state = pump_state;
		//Turn pump on or off
		water_val.pump_state ? pump_on():pump_off();

		pump_state_store();
	}

	LOG_INFO("Water level:%s, Pump Mode:%s, Pump State:%s",
			water_level_enum_str[water_val.water_level],
			pump_mode_enum_str[water_val.pump_mode],
			water_val.pump_state ? "ON":"OFF");
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
	water_val.pump_mode = Auto;
	water_val.pump_state ? pump_on():pump_off();

}


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



// Acuator config



int config_state_load(void)
{
  struct gecko_msg_flash_ps_load_rsp_t* pLoad;

  pLoad = gecko_cmd_flash_ps_load(CONFIG_STATE_KEY);

  // Set default values if ps_load fail
  if (pLoad->result ) {
	LOG_WARN("Flash load failed, loading defaults");
    memset(&config_state, 0, sizeof(uint8_t));
    config_state = SENSOR_DEFAULT_CONFIG;
    return -1;
  }

  memcpy(&config_state, pLoad->value.data, pLoad->value.len);

  return 0;
}


int config_state_store(void)
{
  struct gecko_msg_flash_ps_save_rsp_t* pSave;

  pSave = gecko_cmd_flash_ps_save(CONFIG_STATE_KEY,
		  sizeof(uint8_t),
		  (const uint8*)&config_state);

  if (pSave->result) {
    LOG_ERROR("config state store(): PS save failed, code %x", pSave->result);
    return(-1);
  }

  return 0;
}

uint8_t config_init(void)
{
	config_state_load();

	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_write_attribute_value(gattdb_conn_dev,
				  0, sizeof(uint8_t), (uint8 *)&config_state));

	return config_state;
}



void set_changed_config(uint8_t val)
{
	changed_config_state = val;
}

void update_config(void)
{
	if(changed_config_state != config_state)
	{
		config_state = changed_config_state;
		LOG_INFO("Config changed to 0x%x",
				config_state);

		// write value to flash
		config_state_store();
	}

}

/*
 * Setter functions for static variables in file
 */
void set_light_val(float val,bool sensor_state)
{
	light_val.sensor.light_level = val;
	light_val.sensor.reliable = sensor_state;
}

void set_water_level(uint16_t val)
{
	water_val.water_level = val;
}

void set_pump_mode(pump_mode_t val)
{
	water_val.pump_mode = val;
}

void set_changed_light_setpoint(uint16_t val)
{
	changed_light_setpoint = val;
}

void set_changed_light_deadband(uint8_t val)
{
	changed_light_deadband = val;
}


