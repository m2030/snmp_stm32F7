/**
 * @author  Alexander Hoffman
 * @email   alxhoff@gmail.com
 * @website http://alexhoffman.info
 * @license GNU GPL v3
 * @brief	STM32 HAL library for BH1750 devices
 *
@verbatim
   ----------------------------------------------------------------------
    Copyright (C) Alexander Hoffman, 2017
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
@endverbatim
 */

#include <stdlib.h>
#include <string.h>

#include "stm32f7xx_hal.h"

#include "bh1750.h"
extern UART_HandleTypeDef huart3;
extern char BHbuffer[50];
uint16_t lux;
void _Error_Handler(char * file, int line)
{
  while(1)
  {
  }
}


HAL_StatusTypeDef BH1750_send_command(BH1750_device_t* dev, uint8_t cmd)
{
	//TODO hal checks
	uint8_t u;
	if(u=HAL_I2C_Master_Transmit(
			dev->i2c_handle,	//I2C Handle
			dev->address_w,		//I2C addr of dev
			&cmd,				//CMD to be executed
			1,					//8bit addr
			10					//Wait time
		) != HAL_OK) return HAL_ERROR;

	return HAL_OK;
}

void BH1750_poll_self(BH1750_device_t* self)
{
	BH1750_get_lumen(self);
}

BH1750_device_t* BH1750_init_dev_struct(I2C_HandleTypeDef* i2c_handle,
		char* name, bool addr_grounded)
{
	BH1750_device_t* init =
			(BH1750_device_t*)calloc(1, sizeof(BH1750_device_t));

	if(init == NULL) return NULL;

	if(addr_grounded){
		init->address_r = BH1750_GROUND_ADDR_READ;
		init->address_w = BH1750_GROUND_ADDR_WRITE;
	}else{
		init->address_r = BH1750_NO_GROUND_ADDR_READ;
		init->address_w = BH1750_NO_GROUND_ADDR_WRITE;
	}

	init->name = (char*)malloc(sizeof(char) * strlen(name));

	if(init->name == NULL) return NULL;

	init->i2c_handle = i2c_handle;

	strcpy(init->name, name);

	init->poll = &BH1750_poll_self;

	return init;
}

HAL_StatusTypeDef BH1750_init_dev(BH1750_device_t* dev)
{
	BH1750_send_command(dev, CMD_POWER_ON);
	BH1750_send_command(dev, CMD_RESET);
	BH1750_send_command(dev, CMD_H_RES_MODE);

	return HAL_OK;
}

HAL_StatusTypeDef BH1750_read_dev(BH1750_device_t* dev)
{
	if(HAL_I2C_Master_Receive(dev->i2c_handle,
			dev->address_r,
			dev->buffer,
			2,
			10
	) != HAL_OK) return HAL_ERROR;

	return HAL_OK;
}

HAL_StatusTypeDef BH1750_convert(BH1750_device_t* dev)
{
	dev->value = dev->buffer[0];
	dev->value = (dev->value << 8) | dev->buffer[1];
	uint8_t size;
	//TODO check float stuff
	dev->value/=1.2;
	lux=dev->value;
    size = sprintf(BHbuffer,"\"Lux\": %d\n\r", dev->value);
    HAL_UART_Transmit(&huart3,BHbuffer , strlen(BHbuffer), 1000);
//	sprintf(BHbuffer, "\"Lux\": %d",455 );
	return HAL_OK;
}

HAL_StatusTypeDef BH1750_get_lumen(BH1750_device_t* dev)
{
	BH1750_read_dev(dev);
	BH1750_convert(dev);
	return HAL_OK;
}
