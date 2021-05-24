#include "flash_write.h"

//addresses for network parameters, temperatures, and ports
uint32_t IP_FLASH_ADDRESS[4] =
		{ 0x08080000, 0x08080004, 0x08080008, 0x0808000C };
uint32_t MASK_FLASH_ADDRESS[4] = { 0x08080030, 0x08080034, 0x08080038,
		0x0808003C };
uint32_t GTW_FLASH_ADDRESS[4] =
		{ 0x08080040, 0x08080044, 0x08080048, 0x0808004C };
uint32_t MAC_FLASH_ADDRESS[6] = { 0x08080010, 0x08080014, 0x08080018,
		0x0808001C, 0x08080020, 0x08080024 };
//index 0 - tcp port,1-t_heat1,2 - t_heat2,3 - t_cold1,4 - t_cold2,5 - relay mode,6 - max x position,7 - max y position
//t_heat1 - heating On,t_heat2 - heating Off,t_cold1 - cooling Off,t_cold2 - cooling On,relay mode = 0 - on/off mode,relay mode = 1 - cooling mode,
uint32_t PORT_and_TEMPERATURE_FLASH_ADDRESS[8] = { 0x08080084, 0x08080088,
		0x0808008C, 0x08080090, 0x08080094, 0x08080098, 0x0808009C, 0x080800A0 };

//arrays for temporary storage of data during recording
uint32_t tmp_port_temperature[8];
uint32_t tmp_ip[4];
uint32_t tmp_gtw[4];
uint32_t tmp_msk[4];
uint32_t tmp_mac[6];

HAL_StatusTypeDef flash_ok = HAL_ERROR;

void read_flash_data() {

	for (int i = 0;
			i < sizeof tmp_port_temperature / sizeof tmp_port_temperature[0];
			i++) {
		tmp_port_temperature[i] =
				*(__IO uint32_t*) PORT_and_TEMPERATURE_FLASH_ADDRESS[i];
	}

	for (int i = 0; i < 4; i++) {
		tmp_ip[i] = *(__IO uint32_t*) IP_FLASH_ADDRESS[i];
	}
	for (int i = 0; i < 4; i++) {
		tmp_gtw[i] = *(__IO uint32_t*) GTW_FLASH_ADDRESS[i];
	}

	for (int i = 0; i < 4; i++) {
		tmp_msk[i] = *(__IO uint32_t*) MASK_FLASH_ADDRESS[i];
	}

	for (int i = 0; i < 6; i++) {
		tmp_mac[i] = *(__IO uint32_t*) MAC_FLASH_ADDRESS[i];
	}
}

//write to memory subnet mask
void Set_MASK_ADDR(uint8_t *mask) {
	read_flash_data();
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	FLASH_Erase_Sector(FLASH_SECTOR_8, VOLTAGE_RANGE_3);

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, IP_FLASH_ADDRESS[i],
					tmp_ip[i]);
		}
	}

	for (int i = 0;
			i < sizeof tmp_port_temperature / sizeof tmp_port_temperature[0];
			i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					PORT_and_TEMPERATURE_FLASH_ADDRESS[i],
					tmp_port_temperature[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, GTW_FLASH_ADDRESS[i],
					tmp_gtw[i]);
		}
	}

	for (int i = 0; i < 6; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, MAC_FLASH_ADDRESS[i],
					tmp_mac[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					MASK_FLASH_ADDRESS[i], mask[i]);
		}
	}

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

}
//write to memory gateway
void Set_GTW_ADDR(uint8_t *gtw) {
	read_flash_data();
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	FLASH_Erase_Sector(FLASH_SECTOR_8, VOLTAGE_RANGE_3);

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, IP_FLASH_ADDRESS[i],
					tmp_ip[i]);
		}
	}

	for (int i = 0;
			i < sizeof tmp_port_temperature / sizeof tmp_port_temperature[0];
			i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					PORT_and_TEMPERATURE_FLASH_ADDRESS[i],
					tmp_port_temperature[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					MASK_FLASH_ADDRESS[i], tmp_msk[i]);
		}
	}

	for (int i = 0; i < 6; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, MAC_FLASH_ADDRESS[i],
					tmp_mac[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, GTW_FLASH_ADDRESS[i],
					gtw[i]);
		}
	}
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

}
//write to memory ip
void Set_IP_ADDR(uint8_t *ip) {
	read_flash_data();
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	FLASH_Erase_Sector(FLASH_SECTOR_8, VOLTAGE_RANGE_3);

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, GTW_FLASH_ADDRESS[i],
					tmp_gtw[i]);
		}
	}

	for (int i = 0;
			i < sizeof tmp_port_temperature / sizeof tmp_port_temperature[0];
			i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					PORT_and_TEMPERATURE_FLASH_ADDRESS[i],
					tmp_port_temperature[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					MASK_FLASH_ADDRESS[i], tmp_msk[i]);
		}
	}

	for (int i = 0; i < 6; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, MAC_FLASH_ADDRESS[i],
					tmp_mac[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			int b = ip[i];
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, IP_FLASH_ADDRESS[i],
					ip[i]);
		}
	}
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

}
//write to memory MAC address
void Set_MAC_ADDR(uint8_t *mac) {

	read_flash_data();
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	FLASH_Erase_Sector(FLASH_SECTOR_8, VOLTAGE_RANGE_3);

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, GTW_FLASH_ADDRESS[i],
					tmp_gtw[i]);
		}
	}

	for (int i = 0;
			i < sizeof tmp_port_temperature / sizeof tmp_port_temperature[0];
			i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					PORT_and_TEMPERATURE_FLASH_ADDRESS[i],
					tmp_port_temperature[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					MASK_FLASH_ADDRESS[i], tmp_msk[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, IP_FLASH_ADDRESS[i],
					tmp_ip[i]);
		}
	}

	for (int i = 0; i < 6; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, MAC_FLASH_ADDRESS[i],
					mac[i]);
		}
	}
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

}
//write port or temperature
void Set_var_for_flash(uint16_t param, uint32_t Flash_addr) {
	read_flash_data();
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}
	FLASH_Erase_Sector(FLASH_SECTOR_8, VOLTAGE_RANGE_3);
	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}

	if (Flash_addr == PORT_and_TEMPERATURE_FLASH_ADDRESS[0]) {
		tmp_port_temperature[0] = param;
	} else if (Flash_addr == PORT_and_TEMPERATURE_FLASH_ADDRESS[1]) {
		tmp_port_temperature[1] = param;
	} else if (Flash_addr == PORT_and_TEMPERATURE_FLASH_ADDRESS[2]) {
		tmp_port_temperature[2] = param;
	} else if (Flash_addr == PORT_and_TEMPERATURE_FLASH_ADDRESS[3]) {
		tmp_port_temperature[3] = param;
	} else if (Flash_addr == PORT_and_TEMPERATURE_FLASH_ADDRESS[4]) {
		tmp_port_temperature[4] = param;
	} else if (Flash_addr == PORT_and_TEMPERATURE_FLASH_ADDRESS[5]) {
		tmp_port_temperature[5] = param;
	} else if (Flash_addr == PORT_and_TEMPERATURE_FLASH_ADDRESS[6]) {
		tmp_port_temperature[6] = param;
	} else {
		tmp_port_temperature[7] = param;
	}

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Unlock();
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, GTW_FLASH_ADDRESS[i],
					tmp_gtw[i]);
		}
	}

	for (int i = 0;
			i < sizeof tmp_port_temperature / sizeof tmp_port_temperature[0];
			i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					PORT_and_TEMPERATURE_FLASH_ADDRESS[i],
					tmp_port_temperature[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD,
					MASK_FLASH_ADDRESS[i], tmp_msk[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, IP_FLASH_ADDRESS[i],
					tmp_ip[i]);
		}
	}

	for (int i = 0; i < 6; i++) {
		flash_ok = HAL_ERROR;
		while (flash_ok != HAL_OK) {
			flash_ok = HAL_FLASH_Program(TYPEPROGRAM_WORD, MAC_FLASH_ADDRESS[i],
					tmp_mac[i]);
		}
	}

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK) {
		flash_ok = HAL_FLASH_Lock();
	}
}
