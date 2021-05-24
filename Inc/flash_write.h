/*
 * flash_write.h
 *
 *  Created on: 29 ��� 2020 �.
 *      Author: Daniil
 */

#ifndef FLASH_WRITE_H_
#define FLASH_WRITE_H_
#include "main.h"

void Set_MAC_ADDR(uint8_t *mac);
void Set_IP_ADDR(uint8_t *ip);
void Set_MASK_ADDR(uint8_t *mask);
void Set_GTW_ADDR(uint8_t *gtw);
void Set_Port_or_Temp(uint16_t param, uint32_t Flash_addr);
#endif /* FLASH_WRITE_H_ */
