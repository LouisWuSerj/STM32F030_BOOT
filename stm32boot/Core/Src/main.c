/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus_h.h"
#include "emul_eep_stm32f0.h"
#include "crc32.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
MBVar_t MBRegs;
__attribute__((section(".noinit"))) volatile uint16_t my_non_initialized_integer;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
//static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * Jump to application
 */
void JumptoApp(void)
{
    // disable global interrupt
    __disable_irq();

    // Disable all peripheral interrupts
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(USART2_IRQn);
    NVIC_DisableIRQ(WWDG_IRQn);
    NVIC_DisableIRQ(RTC_IRQn);
    NVIC_DisableIRQ(FLASH_IRQn);
    NVIC_DisableIRQ(RCC_IRQn);
    NVIC_DisableIRQ(EXTI0_1_IRQn);
    NVIC_DisableIRQ(EXTI2_3_IRQn);
    NVIC_DisableIRQ(EXTI4_15_IRQn);
    NVIC_DisableIRQ(DMA1_Channel1_IRQn);
    NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
    NVIC_DisableIRQ(DMA1_Channel4_5_IRQn);
    NVIC_DisableIRQ(ADC1_IRQn);
    NVIC_DisableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    NVIC_DisableIRQ(TIM1_CC_IRQn);
    NVIC_DisableIRQ(TIM3_IRQn);
    NVIC_DisableIRQ(TIM6_IRQn);
    NVIC_DisableIRQ(TIM14_IRQn);
    NVIC_DisableIRQ(TIM15_IRQn);
    NVIC_DisableIRQ(TIM16_IRQn);
    NVIC_DisableIRQ(TIM17_IRQn);
    NVIC_DisableIRQ(I2C1_IRQn);
    NVIC_DisableIRQ(I2C2_IRQn);
    NVIC_DisableIRQ(SPI1_IRQn);
    NVIC_DisableIRQ(SPI2_IRQn);
    NVIC_DisableIRQ(USART1_IRQn);
    NVIC_DisableIRQ(USART2_IRQn);

    // main app start address defined in linker file
    extern uint32_t _main_app_start_address;

    uint32_t MemoryAddr = (uint32_t)&_main_app_start_address;
    uint32_t *pMem = (uint32_t *)MemoryAddr;

    // First address is the stack pointer initial value
    __set_MSP(*pMem); // Set stack pointer

    // Now get main app entry point address
    pMem++;
    void (*pMainApp)(void) = (void (*)(void))(*pMem);

    // Jump to main application (0x0800 4004)
    pMainApp();
}

#define FIRMWARE_START_ADDR    0x08004000             //(uint32_t)(&_main_app_start_address)

void Remap_Table(void)
{
    // Copy interrupt vector table to the RAM.
    volatile uint32_t *VectorTable = (volatile uint32_t *)0x20000000;
    uint32_t ui32_VectorIndex = 0;

    for(ui32_VectorIndex = 0; ui32_VectorIndex < 48; ui32_VectorIndex++)
    {
        VectorTable[ui32_VectorIndex] = *(__IO uint32_t*)((uint32_t)FIRMWARE_START_ADDR + (ui32_VectorIndex << 2));
    }

    __HAL_RCC_AHB_FORCE_RESET();
    //  Enable SYSCFG peripheral clock
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_AHB_RELEASE_RESET();
    // Remap RAM into 0x0000 0000
    __HAL_SYSCFG_REMAPMEMORY_SRAM();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
//  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /**USART1 GPIO Configuration
  PA9     ------> USART1_TX
  PA10     ------> USART1_RX
  PA12     ------> USART1_DE
  */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  MB_Init(115200, 20, 1);

  MBRegs.DEVID = HAL_GetDEVID();
  MBRegs.REVID = HAL_GetREVID();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  while(my_non_initialized_integer == 55555 || !crc32_calc())
	  {
		  MBRegs.cnt = my_non_initialized_integer;

		  switch(MBRegs.CMD)
		  {
			  case FLASH_ERASE:
				  __disable_irq();
				  Internal_Flash_Unlock();
				  uint32_t FlashPageAddrErase = 0x08004000;
				  uint8_t i=0;
				  for(i=0; i<48; i++)
				  {
					  Internal_Flash_Erase(FlashPageAddrErase);
					  FlashPageAddrErase +=1024;
				  }
				  MBRegs.CMD = FLASH_ERASE_OK;
				  Internal_Flash_Lock();
				  __enable_irq();

				  MBRegs.LEN = 0;
				  MBRegs.ADDR=0;
			  break;
			  case FLASH_WRITE:
				  __disable_irq();
				  Internal_Flash_Unlock();
				  Internal_Flash_Write(MBRegs.DATA, 0x08004000 + MBRegs.ADDR, MBRegs.LEN/sizeof(uint16_t));
				  MBRegs.CMD = FLASH_WRITE_OK;
				  Internal_Flash_Lock();
				  __enable_irq();
			  break;
			  case FLASH_READ:
			  break;
			  case SYS_RESET:
				  my_non_initialized_integer = 0;
				  NVIC_SystemReset();
			  break;
			  default: break;
		  }
	  }

	  JumptoApp();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
//static void MX_USART1_UART_Init(void)
//{
//
//  /* USER CODE BEGIN USART1_Init 0 */
////
//  /* USER CODE END USART1_Init 0 */
//
//  /* USER CODE BEGIN USART1_Init 1 */
////
//  /* USER CODE END USART1_Init 1 */
//  huart1.Instance = USART1;
//  huart1.Init.BaudRate = 38400;
//  huart1.Init.WordLength = UART_WORDLENGTH_8B;
//  huart1.Init.StopBits = UART_STOPBITS_1;
//  huart1.Init.Parity = UART_PARITY_NONE;
//  huart1.Init.Mode = UART_MODE_TX_RX;
//  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
//  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//  if (HAL_RS485Ex_Init(&huart1, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN USART1_Init 2 */
////
//  /* USER CODE END USART1_Init 2 */
//
//}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

