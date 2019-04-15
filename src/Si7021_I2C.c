/*
 * Si7021_I2C.c
 *
 *  Created on: Feb 3, 2019
 *      Author: aakash
 */

#include "Si7021_I2C.h"

static int32_t temp_data;

void temp_sensor_init()
{
	//Initialize I2C peripheral
	I2C_SWSTK_Init();

	/*
	 * init_App() turns on sensor, turn off
	 * till measurement is to be taken
	 */
	temp_sensor_lpm_off();
}


void measure_temp_non_blocking(void)
{
	I2C_Command_Read_NonBlocking(I2CPORT_USED, SI7021_ADDR, SI7013_TEMP);
}

int32_t get_temp_val()
{
	get_I2C_read_data(&temp_data);
	return (((temp_data) * 21965L) >> 13) - 46850;;
}

void get_transfer_error(void)
{

}

int32_t get_humidity(uint32_t *data)
{
  int ret = I2C_Command_Read(I2CPORT_USED, SI7021_ADDR, data,SI7013_RH);

  if (ret == 2)
  {
    /* convert to milli-percent */
    *data = (((*data) * 15625L) >> 13) - 6000;
  }
  else
    return -1;

  return 0;
}

int32_t get_temp(int32_t *data)
{
  int ret = I2C_Command_Read(I2CPORT_USED, SI7021_ADDR, (uint32_t *)data,SI7013_TEMP);

  if (ret == 2)
  {
    /* convert to milli-percent */
	  *data = (((*data) * 21965L) >> 13) - 46850;
  }
  else
    return -1;

  return 0;
}

int32_t get_temp_humidity(int32_t *tdata, uint32_t *rhData)
{
	/*
	 * Run humidity measurement first as
	 * temp measurement used gets the temp
	 * measured during last humidity measure
	 */
	if(get_humidity(rhData) != 0)
		return -1;

	if(get_temp(tdata) !=0)
		return -1;

	return 0;
}

