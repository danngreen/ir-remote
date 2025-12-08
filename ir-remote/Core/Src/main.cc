#include "main.h"
#include "capture.hh"
#include "util/circular_buffer.hh"

void SystemClock_Config();
void MX_GPIO_Init();
void MX_SAI1_Init();
void MX_TIM2_Init(TIM_HandleTypeDef &);

// TODO: use mdrivlib interrutps to avoid global
TIM_HandleTypeDef *_tim_instance;

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

// TODO: use mdrivlib interrupts to avoid global htim2
extern "C" void TIM2_IRQHandler() {
	HAL_TIM_IRQHandler(_tim_instance);
}

extern "C" void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *) {
	IrCapture::_instance->process();
}
