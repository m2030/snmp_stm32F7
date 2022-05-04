/*
 * my_mib.c
 *
 *  Created on: Apr 24, 2022
 *      Author: mohamed.abouhashem
 */

#include "main.h"

#include "my_mib.h"
#include "lwip/apps/snmp_scalar.h"
#include "sht2x_for_stm32_hal.h"
#include "BH1750.h"
extern unsigned char SHTbuffer[100];
extern UART_HandleTypeDef huart3;

/*
 * SNMP general function prototype
 * Add here any function to be usable in specific private MIB node.
 */
static int16_t get_tem_value(struct snmp_node_instance* instance, void* value);
static int16_t get_hum_value(struct snmp_node_instance* instance, void* value);
static int16_t get_lux_value(struct snmp_node_instance* instance, void* value);
static snmp_err_t set_leds_value(struct snmp_node_instance* instance, uint16_t len, void *value);

/* led .1.3.6.1.4.1.26381.1.1 */
//static const struct snmp_scalar_node leds_node = SNMP_SCALAR_CREATE_NODE_READONLY(1, SNMP_ASN1_TYPE_INTEGER, get_leds_value); // ready-only example
static const struct snmp_scalar_node tem_node = SNMP_SCALAR_CREATE_NODE(1, SNMP_NODE_INSTANCE_READ_WRITE, SNMP_ASN1_TYPE_INTEGER, get_tem_value, NULL, set_leds_value); //read-and-write example
/* switch .1.3.6.1.4.1.26381.1.2 */
static const struct snmp_scalar_node hum_node = SNMP_SCALAR_CREATE_NODE_READONLY(2, SNMP_ASN1_TYPE_INTEGER, get_hum_value);
/* xgpio .1.3.6.1.4.1.26381.1.3 */
static const struct snmp_scalar_node lux_node = SNMP_SCALAR_CREATE_NODE_READONLY(3, SNMP_ASN1_TYPE_INTEGER, get_lux_value);

/* all private nodes  .1.3.6.1.4.1.26381.1 .. (.1,.2,.3) .0 */
static const struct snmp_node* const parm_nodes[] = { &hum_node.node.node, &tem_node.node.node, &lux_node.node.node };
static const struct snmp_tree_node parm_tree_node = SNMP_CREATE_TREE_NODE(1, parm_nodes);

/* define private mib */
static const u32_t my_base_oid[] = { 1, 3, 6, 1, 4, 1, 26381, 1 };
const struct snmp_mib parm_mib = SNMP_MIB_CREATE(my_base_oid, &parm_tree_node.node);
extern uint16_t lux;


/* leds value .1.3.6.1.4.1.26381.1.1.0 */
static int16_t get_tem_value(struct snmp_node_instance* instance, void* value) {
	uint32_t *uint_ptr = (uint32_t*) value;
//	convergpio_toRead();
	*uint_ptr = (int16_t) SHT2x_GetInteger(cel1);
	/* get structure values */
//	convergpio_toWrite();
	return sizeof(*uint_ptr);
}

/* switches value .1.3.6.1.4.1.26381.1.2.0 */
static int16_t get_hum_value(struct snmp_node_instance* instance, void* value)
{
	uint32_t *uint_ptr = (uint32_t*) value;
	//*uint_ptr = (uint32_t) HAL_GPIO_ReadPin(GPIOB, *(&switches)); /* get structure values */
	//rh1 = SHT2x_GetRelativeHumidity(1);
	*uint_ptr = (int16_t) SHT2x_GetInteger(rh1);
	sprintf(SHTbuffer, "snmp:%d.%d%% RH\n\r",SHT2x_GetInteger(rh1), SHT2x_GetDecimal(rh1, 1));
	HAL_UART_Transmit(&huart3, SHTbuffer, strlen(SHTbuffer), 1000);

	return sizeof(*uint_ptr);
}

/* xgpio value .1.3.6.1.4.1.26381.1.3.0 */
static int16_t get_lux_value(struct snmp_node_instance* instance, void* value) {
	u32_t *uint_ptr = (uint32_t*) value;
	*uint_ptr = (int16_t) lux;
;
	return sizeof(*uint_ptr);
}

static snmp_err_t set_leds_value(struct snmp_node_instance* instance, uint16_t len, void *value)
{
	uint32_t val = *((u32_t*)value);
	HAL_GPIO_WritePin(GPIOB ,*(&leds), (uint32_t) val); /* write a value in struture */
	return SNMP_ERR_NOERROR;
}
static void convergpio_toRead(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
static void convergpio_toWrite(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
