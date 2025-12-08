#include "main.h"
#include "capture.hh"
#include "util/circular_buffer.hh"

static void SystemClock_Config();
static void MX_GPIO_Init();
static void MX_SAI1_Init();
static void MX_TIM2_Init(TIM_HandleTypeDef &);

// TODO: use mdrivlib interrutps to avoid global
static TIM_HandleTypeDef *_tim_instance;

// DEBUGGING AID:
CircularBuffer<uint32_t, 1024> DEBUG_BUF;

int main(void) {
	TIM_HandleTypeDef htim2;
	_tim_instance = &htim2;

	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_SAI1_Init();

	IrCapture ircapture{htim2, TIM_CHANNEL_1, [](uint32_t code) {
							DEBUG_BUF.put(code);
						}};

	MX_TIM2_Init(htim2);

	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

	while (true) {
		__WFI();
	}
}

void SystemClock_Config() {
	RCC_OscInitTypeDef RCC_OscInitStruct{};
	RCC_ClkInitTypeDef RCC_ClkInitStruct{};

	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
	RCC_OscInitStruct.PLL.PLLN = 46;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

// TODO: mdrivlib AudioStream
SAI_HandleTypeDef hsai_BlockA1;
SAI_HandleTypeDef hsai_BlockB1;
static void MX_SAI1_Init() {

	hsai_BlockA1.Instance = SAI1_Block_A;
	hsai_BlockA1.Init.AudioMode = SAI_MODESLAVE_RX;
	hsai_BlockA1.Init.Synchro = SAI_SYNCHRONOUS;
	hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
	hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
	hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK) {
		Error_Handler();
	}
	hsai_BlockB1.Instance = SAI1_Block_B;
	hsai_BlockB1.Init.AudioMode = SAI_MODEMASTER_TX;
	hsai_BlockB1.Init.Synchro = SAI_ASYNCHRONOUS;
	hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	hsai_BlockB1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	hsai_BlockB1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
	hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_HF;
	hsai_BlockB1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
	hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
	hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
	hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK) {
		Error_Handler();
	}
}

static void MX_TIM2_Init(TIM_HandleTypeDef &htim2) {

	TIM_MasterConfigTypeDef sMasterConfig{};
	TIM_IC_InitTypeDef sConfigIC{};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 122; //adjust to hit 1MHz timer res
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 65535;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_IC_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 4; // adjust as needed
	if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}
}

void MX_GPIO_Init() {
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

// TODO: use mdrivlib interrupts to avoid global htim2
void TIM2_IRQHandler() {
	HAL_TIM_IRQHandler(_tim_instance);
}

void Error_Handler() {
	__disable_irq();
	while (1) {
	}
}
