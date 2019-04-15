/*
 * swstk_i2c.c
 *
 *  Created on: Feb 3, 2019
 *      Author: aakash
 */

#include "i2cspm.h"
#include "log.h"

void I2C_SWSTK_Init(void)
{
	// Initialize I2C peripheral
	I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
	I2CSPM_Init(&i2cInit);
}

/**
 * Implementation borrowed from
 * SiLabs Bluetooth thermometer
 * sample code si7013.c
 */
int32_t I2C_Command_Read(I2C_TypeDef *i2c, uint8_t addr, uint32_t *data,
                              uint8_t command)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[1];

  seq.addr  = (addr<<1); // Shift right to make MS 7 bits address bits
  seq.flags = I2C_FLAG_WRITE_READ;

  /* Command to be given to sensor */
  i2c_write_data[0] = command;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 1;

  /* Data received from sensor to
   *  be added to buffer provided
   *  Buffer length also has to be provided
   *  2 bytes of data will be received from
   *  the sensor
   */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 2;

  ret = I2CSPM_Transfer(i2c, &seq);

  if (ret != i2cTransferDone) {
    *data = 0;
    LOG_ERROR("I2C transfer failure %d", ret);
    return((int) ret);
  }

  /*
   * Shift MSB 8 bits, add LSB to it
   * to make 16 bit read data from sensor
   */
  *data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] );

  return((int) 2);
}


void I2C_Transfer_Non_Blocking(I2C_TypeDef *i2c, I2C_TransferSeq_TypeDef *seq)
{
	I2C_TransferReturn_TypeDef ret;
	// Enable Interrupts
	NVIC_EnableIRQ(I2C0_IRQn);
	SLEEP_SleepBlockBegin(sleepEM2);

	//Initialize transfer and enable interrupts
	ret = I2C_TransferInit(i2c, seq);
	if(ret != i2cTransferInProgress)
	{
		LOG_DEBUG("I2C Transfer returned %d", ret);
	}


}


static I2C_TransferSeq_TypeDef    _seq;
static uint8_t                    _i2c_read_data[2];
static uint8_t                    _i2c_write_data[1];


void I2C_Command_Read_NonBlocking(I2C_TypeDef *i2c, uint8_t addr, uint8_t command)
{

	  _seq.addr  = (addr<<1); // Shift right to make MS 7 bits address bits
	  _seq.flags = I2C_FLAG_WRITE_READ;

	  /* Command to be given to sensor */
	  _i2c_write_data[0] = command;
	  _seq.buf[0].data   = _i2c_write_data;
	  _seq.buf[0].len    = 1;

	  /* Data received from sensor to
	   *  be added to buffer provided
	   *  Buffer length also has to be provided
	   *  2 bytes of data will be received from
	   *  the sensor
	   */
	  _seq.buf[1].data = _i2c_read_data;
	  _seq.buf[1].len  = 2;

	  I2C_Transfer_Non_Blocking(i2c, &_seq);
}

void get_I2C_read_data(int32_t *data)
{
	 /*
	   * Shift MSB 8 bits, add LSB to it
	   * to make 16 bit read data from sensor
	   */
	  *data = ((uint32_t) _i2c_read_data[0] << 8) + (_i2c_read_data[1] );
}




