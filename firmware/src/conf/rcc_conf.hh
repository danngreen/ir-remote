#pragma once
#include "drivers/stm32xx.h"

namespace IRRemote
{

const RCC_OscInitTypeDef osc_conf{
	.OscillatorType = RCC_OSCILLATORTYPE_HSE,
	.HSEState = RCC_HSE_ON,
	.PLL = {.PLLState = RCC_PLL_ON,
			.PLLSource = RCC_PLLSOURCE_HSE,
			.PLLM = RCC_PLLM_DIV3,
			.PLLN = 46,
			.PLLP = RCC_PLLP_DIV2,
			.PLLQ = RCC_PLLQ_DIV4,
			.PLLR = RCC_PLLR_DIV2},
};

const RCC_ClkInitTypeDef clk_conf{
	.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
	.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
	.AHBCLKDivider = RCC_SYSCLK_DIV1,
	.APB1CLKDivider = RCC_HCLK_DIV1,
	.APB2CLKDivider = RCC_HCLK_DIV1,
};

const RCC_PeriphCLKInitTypeDef rcc_periph_conf = {
	.PeriphClockSelection =
		RCC_PERIPHCLK_SAI1 | RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USB | RCC_PERIPHCLK_ADC12 | RCC_PERIPHCLK_I2C1,
	.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1,
	.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1,
	.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL,
	.UsbClockSelection = RCC_USBCLKSOURCE_HSI48,
	.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK,
};

} // namespace IRRemote
