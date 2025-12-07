/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32g4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void) {

	/* USER CODE BEGIN MspInit 0 */

	/* USER CODE END MspInit 0 */

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	/* System interrupt init*/

	/** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
	HAL_PWREx_DisableUCPDDeadBattery();

	/* USER CODE BEGIN MspInit 1 */

	/* USER CODE END MspInit 1 */
}

/**
  * @brief TIM_IC MSP Initialization
  * This function configures the hardware resources used in this example
  * @param htim_ic: TIM_IC handle pointer
  * @retval None
  */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim_ic) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (htim_ic->Instance == TIM2) {
		/* USER CODE BEGIN TIM2_MspInit 0 */

		/* USER CODE END TIM2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_TIM2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**TIM2 GPIO Configuration
    PA0     ------> TIM2_CH1
    */
		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USER CODE BEGIN TIM2_MspInit 1 */

		/* USER CODE END TIM2_MspInit 1 */
	}
}

/**
  * @brief TIM_IC MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param htim_ic: TIM_IC handle pointer
  * @retval None
  */
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim_ic) {
	if (htim_ic->Instance == TIM2) {
		/* USER CODE BEGIN TIM2_MspDeInit 0 */

		/* USER CODE END TIM2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_TIM2_CLK_DISABLE();

		/**TIM2 GPIO Configuration
    PA0     ------> TIM2_CH1
    */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

		/* USER CODE BEGIN TIM2_MspDeInit 1 */

		/* USER CODE END TIM2_MspDeInit 1 */
	}
}

static uint32_t SAI1_client = 0;

void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai) {

	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	/* SAI1 */
	if (hsai->Instance == SAI1_Block_A) {
		/* Peripheral clock enable */

		/** Initializes the peripherals clocks
  */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
		PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
			Error_Handler();
		}

		if (SAI1_client == 0) {
			__HAL_RCC_SAI1_CLK_ENABLE();
		}
		SAI1_client++;

		/**SAI1_A_Block_A GPIO Configuration
    PA10     ------> SAI1_SD_A
    */
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF14_SAI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	if (hsai->Instance == SAI1_Block_B) {
		/* Peripheral clock enable */

		/** Initializes the peripherals clocks
  */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
		PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
			Error_Handler();
		}

		if (SAI1_client == 0) {
			__HAL_RCC_SAI1_CLK_ENABLE();
		}
		SAI1_client++;

		/**SAI1_B_Block_B GPIO Configuration
    PA4     ------> SAI1_FS_B
    PB3     ------> SAI1_SCK_B
    PB4     ------> SAI1_MCLK_B
    PB5     ------> SAI1_SD_B
    */
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF14_SAI1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF12_SAI1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai) {
	/* SAI1 */
	if (hsai->Instance == SAI1_Block_A) {
		SAI1_client--;
		if (SAI1_client == 0) {
			/* Peripheral clock disable */
			__HAL_RCC_SAI1_CLK_DISABLE();
		}

		/**SAI1_A_Block_A GPIO Configuration
    PA10     ------> SAI1_SD_A
    */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
	}
	if (hsai->Instance == SAI1_Block_B) {
		SAI1_client--;
		if (SAI1_client == 0) {
			/* Peripheral clock disable */
			__HAL_RCC_SAI1_CLK_DISABLE();
		}

		/**SAI1_B_Block_B GPIO Configuration
    PA4     ------> SAI1_FS_B
    PB3     ------> SAI1_SCK_B
    PB4     ------> SAI1_MCLK_B
    PB5     ------> SAI1_SD_B
    */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
	}
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
