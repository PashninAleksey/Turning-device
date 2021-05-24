/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

//structure for creating a socket
typedef struct struct_sock_t {

	struct netconn *conn;
} struct_sock;
struct_sock sock01, sock02;


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern const struct snmp_mib pashnin_mib;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
typedef struct {
	float T;
} read_T;

typedef struct {
	uint16_t T_heat1;
	uint16_t T_heat2;
	uint16_t T_cold1;
	uint16_t T_cold2;
} set_T;
/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

#define USER_FLASH_FIRST_PAGE_ADDRESS 0x08020000 /* Only as example see comment */
#define USER_FLASH_LAST_PAGE_ADDRESS  0x080E0000
#define USER_FLASH_END_ADDRESS        0x080FFFFF

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DIR2_Pin GPIO_PIN_2
#define DIR2_GPIO_Port GPIOE
#define STEP2_Pin GPIO_PIN_3
#define STEP2_GPIO_Port GPIOE
#define SLEEP2_Pin GPIO_PIN_4
#define SLEEP2_GPIO_Port GPIOE
#define RESET2_Pin GPIO_PIN_5
#define RESET2_GPIO_Port GPIOE
#define End_Stop2_Pin GPIO_PIN_2
#define End_Stop2_GPIO_Port GPIOC
#define End_Stop1_Pin GPIO_PIN_3
#define End_Stop1_GPIO_Port GPIOC
#define MS32_Pin GPIO_PIN_0
#define MS32_GPIO_Port GPIOA
#define MS22_Pin GPIO_PIN_3
#define MS22_GPIO_Port GPIOA
#define MS12_Pin GPIO_PIN_4
#define MS12_GPIO_Port GPIOA
#define ENABLE2_Pin GPIO_PIN_5
#define ENABLE2_GPIO_Port GPIOA
#define Temperature_Pin GPIO_PIN_6
#define Temperature_GPIO_Port GPIOA
#define ENABLE1_Pin GPIO_PIN_9
#define ENABLE1_GPIO_Port GPIOE
#define MS11_Pin GPIO_PIN_11
#define MS11_GPIO_Port GPIOE
#define MS21_Pin GPIO_PIN_13
#define MS21_GPIO_Port GPIOE
#define MS31_Pin GPIO_PIN_14
#define MS31_GPIO_Port GPIOE
#define RESET1_Pin GPIO_PIN_12
#define RESET1_GPIO_Port GPIOD
#define SLEEP1_Pin GPIO_PIN_13
#define SLEEP1_GPIO_Port GPIOD
#define STEP1_Pin GPIO_PIN_14
#define STEP1_GPIO_Port GPIOD
#define DIR1_Pin GPIO_PIN_15
#define DIR1_GPIO_Port GPIOD
#define Heating_Pin GPIO_PIN_6
#define Heating_GPIO_Port GPIOC
#define RELAY_Pin GPIO_PIN_7
#define RELAY_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
