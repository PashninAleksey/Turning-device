/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "flash_if.h"
#include "flash_write.h"
#include "net_param_init.h"
#include "my_mibs.h"
#include "tcp_server.h"
#include "modbus.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
u8_t * syscontact_str = (u8_t*) "Si Tech";
u16_t syscontact_len = sizeof("Si Tech");
u8_t * syslocation_str = (u8_t*) "SNMP turning_device";
u16_t syslocation_len = sizeof("SNMP turning_device");
u8_t * sysname_str = (u8_t*) "Turning_device";
u16_t sysname_len = sizeof("Turning_device");
u16_t bufsize = 256;
const u8_t * sysdescr = (u8_t*) "SNMP turning device made in Si Tech ";
const u16_t sysdescr_len = sizeof("SNMP turning device made in Si Tech ");

extern const struct snmp_mib mib2;
static const struct snmp_mib *my_snmp_mibs[] = { &mib2, &pashnin_mib };

//temperature for tcpserver,modbus,snmp
float T_current = 0.0;
//port for tcpserver
uint16_t tcp_port;
//t_heat1 - t heating on,t_heat2 - t heating off,t_cold2 - t cooling on,t_cold1 - t cooling off
uint16_t t_heat1, t_heat2, t_cold1, t_cold2;
//maximum value on the x-axis and y-axis
uint32_t y_max, x_max;
//relay operation mode
uint8_t relay_mode;
osMailQId TemperatureReadHandle, TemperatureSetHandle;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim3;

osThreadId TCPserverHandle;
osThreadId TaskHeatingHandle;
osThreadId modBusHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM3_Init(void);
void TCPInit(void const * argument);
void Thermoregulation(void const * argument);
void modBusInit(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	osMailQDef(TemperatureRead, 1, read_T);
	TemperatureReadHandle = osMailCreate(osMailQ(TemperatureRead), NULL);
	osMailQDef(TemperatureSet, 1, set_T);
	TemperatureSetHandle = osMailCreate(osMailQ(TemperatureSet), NULL);
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	Net_Param_init();
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_LWIP_Init();
	MX_IWDG_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* definition and creation of TCPserver */
	osThreadDef(TCPserver, TCPInit, osPriorityNormal, 0, 512);
	TCPserverHandle = osThreadCreate(osThread(TCPserver), NULL);

	/* definition and creation of TaskHeating */
	osThreadDef(TaskHeating, Thermoregulation, osPriorityNormal, 0, 512);
	TaskHeatingHandle = osThreadCreate(osThread(TaskHeating), NULL);

	/* definition and creation of modBus */
	osThreadDef(modBus, modBusInit, osPriorityNormal, 0, 512);
	modBusHandle = osThreadCreate(osThread(modBus), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE()
	;
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI
			| RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */
	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief IWDG Initialization Function
 * @param None
 * @retval None
 */
static void MX_IWDG_Init(void) {

	/* USER CODE BEGIN IWDG_Init 0 */

	/* USER CODE END IWDG_Init 0 */

	/* USER CODE BEGIN IWDG_Init 1 */

	/* USER CODE END IWDG_Init 1 */
	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
	hiwdg.Init.Reload = 4095;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN IWDG_Init 2 */

	/* USER CODE END IWDG_Init 2 */

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 10;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */
	HAL_TIM_MspPostInit(&htim3);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE()
	;
	__HAL_RCC_GPIOH_CLK_ENABLE()
	;
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	__HAL_RCC_GPIOD_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE,
	DIR2_Pin | STEP2_Pin | ENABLE1_Pin | MS11_Pin | MS21_Pin | MS31_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE, SLEEP2_Pin | RESET2_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, MS32_Pin | MS22_Pin | MS12_Pin | ENABLE2_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, RESET1_Pin | SLEEP1_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, STEP1_Pin | DIR1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : DIR2_Pin STEP2_Pin SLEEP2_Pin RESET2_Pin
	 ENABLE1_Pin MS11_Pin MS21_Pin MS31_Pin */
	GPIO_InitStruct.Pin = DIR2_Pin | STEP2_Pin | SLEEP2_Pin | RESET2_Pin
			| ENABLE1_Pin | MS11_Pin | MS21_Pin | MS31_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : End_Stop2_Pin End_Stop1_Pin */
	GPIO_InitStruct.Pin = End_Stop2_Pin | End_Stop1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : MS32_Pin MS22_Pin MS12_Pin ENABLE2_Pin */
	GPIO_InitStruct.Pin = MS32_Pin | MS22_Pin | MS12_Pin | ENABLE2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : RESET1_Pin SLEEP1_Pin STEP1_Pin DIR1_Pin */
	GPIO_InitStruct.Pin = RESET1_Pin | SLEEP1_Pin | STEP1_Pin | DIR1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : RELAY_Pin */
	GPIO_InitStruct.Pin = RELAY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(RELAY_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//calculate temperature
float computeTemp(float u) {
	float resistance;

	float Untc = u / 4096 * 3.3;
	resistance = Untc * 5100.0 / (3.3 - Untc);
	float result = 1 / (1.0 / 298.15 + 1.0 / 4390.0 * log(resistance / 19500))
			- 273.15;
	return result;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_TCPInit */
/**
 * @brief  Function implementing the TCPserver thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_TCPInit */
void TCPInit(void const * argument) {
	/* USER CODE BEGIN 5 */
	struct netconn *conn;
	err_t err;
	conn = netconn_new(NETCONN_TCP);

	if (conn != NULL) {
		sock01.conn = conn;
		err = netconn_bind(conn, NULL, tcp_port);
		if (err == ERR_OK) {
			netconn_listen(conn);
			sys_thread_new("tcp_thread", tcp_thread, (void*) &sock01,
			DEFAULT_THREAD_STACKSIZE, osPriorityNormal);

		} else {
			netconn_delete(conn);
		}
	}

	//set parameters for SNMP
	snmp_mib2_set_syscontact(syscontact_str, &syscontact_len, bufsize);
	snmp_mib2_set_syslocation(syslocation_str, &syslocation_len, bufsize);
	//set to 0 when disabled, >0 when enabled
	snmp_set_auth_traps_enabled(0);
	snmp_mib2_set_sysdescr(sysdescr, &sysdescr_len);
	snmp_mib2_set_sysname(sysname_str, &sysname_len, bufsize);

	snmp_set_mibs(my_snmp_mibs, LWIP_ARRAYSIZE(my_snmp_mibs));
	snmp_init(); //Just after udp_init

	osEvent eventT;
	volatile read_T *T_r;
	//read temp for SNMP,tcpserver,modbus
	/* Infinite loop */
	for (;;) {
		eventT = osMailGet(TemperatureReadHandle, 100);
		HAL_IWDG_Refresh(&hiwdg);
		if (eventT.status == osEventMail) {
			T_r = eventT.value.p;
			T_current = T_r->T;
		}
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Thermoregulation */
/**
 * @brief Function implementing the TaskHeating thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Thermoregulation */
void Thermoregulation(void const * argument) {
	/* USER CODE BEGIN Thermoregulation */
	osEvent event_setT;
	float T_board = 0.0;
	volatile set_T *T_set;
	volatile read_T T_read;
	volatile read_T *T_read_p;
	T_read_p = &T_read;

	uint8_t t_h1 = t_heat1;
	uint8_t t_h2 = t_heat2;
	uint8_t t_c1 = t_cold1;
	uint8_t t_c2 = t_cold2;

	uint8_t key_h = 0;
	uint8_t key_c = 0;
	uint8_t pwm_val = 0;

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	/* Infinite loop */
	for (;;) {
		T_read_p = osMailAlloc(TemperatureReadHandle, osWaitForever);
		T_board = 0.0;
		event_setT = osMailGet(TemperatureSetHandle, 100);

		if (event_setT.status == osEventMail) {
			T_set = event_setT.value.p;
			t_h1 = T_set->T_heat1;
			t_h2 = T_set->T_heat2;
			t_c1 = T_set->T_cold1;
			t_c2 = T_set->T_cold2;
		}

		for (int i = 0; i < 10; i++) {
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			T_board = T_board + HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
		}
		T_board = T_board / 10.;
		T_board = computeTemp(T_board);
		T_read.T = T_board;

		//if cooling mode is enabled
		if (relay_mode == 1) {
			if (T_board > t_c2) {
				key_c = 1;
				HAL_GPIO_WritePin(GPIOC, RELAY_Pin, GPIO_PIN_SET);
			}
			if (T_board > t_c1 && T_board < t_c2 && key_c == 1) {
				HAL_GPIO_WritePin(GPIOC, RELAY_Pin, GPIO_PIN_SET);
			}
			if (T_board > t_c1 && T_board < t_c2 && key_c == 0) {
				HAL_GPIO_WritePin(GPIOC, RELAY_Pin, GPIO_PIN_RESET);
			}
			if (T_board < t_c1) {
				key_c = 0;
				HAL_GPIO_WritePin(GPIOC, RELAY_Pin, GPIO_PIN_RESET);
			}
		}

		//heating mode
		if (T_board < t_h1) {
			key_h = 1;
			if (pwm_val != htim3.Init.Period) {
				pwm_val++;
			}
		}
		if (T_board > t_h1 && T_board < t_h2 && key_h == 1) {
			if (pwm_val != htim3.Init.Period) {
				pwm_val++;
			}
		}
		if (T_board > t_h1 && T_board < t_h2 && key_h == 0) {
			if (pwm_val != 0) {
				pwm_val--;
			}
		}
		if (T_board > t_h2) {
			key_h = 0;
			if (pwm_val != 0) {
				pwm_val--;
			}
		}
		TIM3->CCR1 = pwm_val;
		osMailPut(TemperatureReadHandle, &T_read);

	}
	/* USER CODE END Thermoregulation */
}

/* USER CODE BEGIN Header_modBusInit */
/**
 * @brief Function implementing the modBus thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_modBusInit */
void modBusInit(void const * argument) {
	/* USER CODE BEGIN modBusInit */
	struct netconn *conn;
	err_t err;

	conn = netconn_new(NETCONN_TCP);
	if (conn != NULL) {
		sock02.conn = conn;
		err = netconn_bind(conn, NULL, 502);
		if (err == ERR_OK) {
			netconn_listen(conn);
			sys_thread_new("modbus_thread", modbus_thread, (void*) &sock02,
			DEFAULT_THREAD_STACKSIZE, osPriorityNormal);

		} else {
			netconn_delete(conn);
		}
	}
	/* Infinite loop */
	for (;;) {
		osDelay(1);
	}
	/* USER CODE END modBusInit */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	NVIC_SystemReset();
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	NVIC_SystemReset();
	/* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
