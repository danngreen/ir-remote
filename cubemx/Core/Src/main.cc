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
CircularBuffer<uint32_t, 32> DEBUG_BUF;

int main(void) {
	TIM_HandleTypeDef htim2;
	_tim_instance = &htim2;

	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	// MX_SAI1_Init();

	MX_TIM2_Init(htim2);

	uint32_t last_led_tm = 0;

	IrCapture ircapture{htim2, TIM_CHANNEL_1, [&last_led_tm](uint32_t code) {
							//
							if (code == 0xF7087F80) {	 // vol down
								GPIOA->BSRR = (1 << 1);	 // turn on PA1 to turn on the LED
								GPIOA->BSRR = (1 << 15); // turn on PA2 to turn off the LED
								last_led_tm = HAL_GetTick();
							}

							if (code == 0xFD027F80) {			// vol up
								GPIOA->BSRR = (1 << (15 + 16)); // turn off PA2 to turn off the LED
								GPIOA->BSRR = (1 << (1 + 16));	// turn off PA1 to turn on the LED
								last_led_tm = HAL_GetTick();
							}

							DEBUG_BUF.put(code);
						}};

	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

	while (true) {
		// Turn both lights off 100ms after either goes on
		if (last_led_tm && (HAL_GetTick() - last_led_tm > 100)) {
			GPIOA->BSRR = (1 << (1 + 16)); // turn off PA1 to turn off the LED
			GPIOA->BSRR = (1 << 15);	   // turn on PA2 to turn off the LED
			last_led_tm = 0;
		}
	}
}

// TODO: use mdrivlib interrupts to avoid global htim2
extern "C" void TIM2_IRQHandler() {
	HAL_TIM_IRQHandler(_tim_instance);
}

extern "C" void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *) {
	IrCapture::_instance->process();
}
