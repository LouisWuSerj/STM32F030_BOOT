/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

enum {FLASH_ERASE = 1, FLASH_WRITE, FLASH_READ, FLASH_ERASE_OK, FLASH_ERASE_ERR, FLASH_WRITE_OK, FLASH_WRITE_ERR, SYS_RESET};

typedef struct /*__attribute__((packed))*/
{
	uint32_t REVID;//0-1
	uint32_t DEVID;//2-3
	uint16_t FLAGS;//4
	uint16_t CMD;//5
	uint32_t ADDR;//6-7
	uint16_t res;//8
	uint16_t LEN;//9
	uint16_t DATA[64];//10-
	uint16_t cnt;//74
}MBVar_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define SetBit(var,bit) var |= (1<<bit)
#define ClrBit(var,bit) var &= ~(1<<bit)

#define F_AHB  48000000ull
#define F_APB1 48000000ull
#define F_APB2 48000000ull
//modbus setup
#define MB_UART1
#define MB_UART_NVIC_PRIORITY 5
#define MB_REGS_LEN (sizeof(MBVar_t)/sizeof(uint16_t))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
