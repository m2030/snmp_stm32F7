/*
 * my_mib.h
 *
 *  Created on: Apr 24, 2022
 *      Author: mohamed.abouhashem
 */

#ifndef INC_MY_MIB_H_
#define INC_MY_MIB_H_
#include "main.h"

//-- var available in simple_snmp_agent.
extern uint32_t *leds, *switches, *xgpio;
extern float cel1 ;
extern float rh1;
static void convergpio_toRead(void);
static void convergpio_toWrite(void);
#endif /* INC_MY_MIB_H_ */
