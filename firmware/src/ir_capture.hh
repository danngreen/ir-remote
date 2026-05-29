#pragma once
#include "board_conf.hh"
#include "debug.hh"
#include "drivers/interrupt.hh"
#include "drivers/pin.hh"
#include "drivers/rcc.hh"
#include "event.hh"
#include "nec.hh"
#include "printf.h"

namespace RemoteVolume
{

struct IRCapture {
	TIM_HandleTypeDef timh;

	nec_state_t nec_state = NEC_STATE_IDLE;
	uint8_t expecting_low = 0;
	uint8_t bit_index = 0;
	uint32_t nec_code = 0;

	uint16_t last_capture = 0;
	Event last_event = Event::None;

	static inline IRCapture *_instance;

	IRCapture() {
		_instance = this;

		// TODO: create mdrivlib TimICChannel driver, and a TimICConfig struct
		mdrivlib::Pin{Board::IRDemod, mdrivlib::PinMode::Alt};

		mdrivlib::RCC_Enable::TIM2_::set();

		TIM_MasterConfigTypeDef sMasterConfig{};
		TIM_IC_InitTypeDef sConfigIC{};

		timh.Instance = TIM2;
		timh.Init.Prescaler = 170; // adjust to hit 1MHz timer res
		timh.Init.CounterMode = TIM_COUNTERMODE_UP;
		timh.Init.Period = 65535;
		timh.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		timh.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_IC_Init(&timh) != HAL_OK) {
			printf_("Error: HAL_TIM_IC_Init failed\n");
		}

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&timh, &sMasterConfig) != HAL_OK) {
			printf_("Error: HAL_TIMEx_MasterConfigSynchronization failed\n");
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
		sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 4; // adjust as needed
		if (HAL_TIM_IC_ConfigChannel(&timh, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) {
			printf_("Error: HAL_TIM_IC_ConfigChannel failed\n");
		}

		mdrivlib::InterruptManager::register_and_start_isr(TIM2_IRQn, 1, 0, [this]() { HAL_TIM_IRQHandler(&timh); });

		HAL_TIM_IC_Start_IT(&timh, TIM_CHANNEL_1);
	}

	void process() {
		uint16_t now = __HAL_TIM_GET_COMPARE(&timh, TIM_CHANNEL_1);
		uint16_t delta = (uint16_t)(now - last_capture);
		last_capture = now;

		// Long gap => reset state machine
		if (delta > NEC_GAP_RESET) {
			nec_state = NEC_STATE_IDLE;
			expecting_low = 0;
			bit_index = 0;
			nec_code = 0;
			return;
		}

		switch (nec_state) {

			case NEC_STATE_IDLE:
				// Look for leader low
				if (delta >= NEC_HDR_LOW_MIN && delta <= NEC_HDR_LOW_MAX) {
					nec_state = NEC_STATE_LEAD_LOW;
				}
				break;

			case NEC_STATE_LEAD_LOW:
				// Expect leader high
				if (delta >= NEC_HDR_HIGH_MIN && delta <= NEC_HDR_HIGH_MAX) {
					nec_state = NEC_STATE_LEAD_HIGH;
					expecting_low = 1;
					bit_index = 0;
					nec_code = 0;
				} else {
					nec_state = NEC_STATE_IDLE;
				}
				break;

			case NEC_STATE_LEAD_HIGH:
				// First low of first bit
				if (delta >= NEC_BIT_LOW_MIN && delta <= NEC_BIT_LOW_MAX) {
					nec_state = NEC_STATE_BITS;
					expecting_low = 0; // next will be high (bit value)
				} else {
					nec_state = NEC_STATE_IDLE;
				}
				break;

			case NEC_STATE_BITS:
				if (expecting_low) {
					// This should be the low (~560 us) for each bit
					if (delta < NEC_BIT_LOW_MIN || delta > NEC_BIT_LOW_MAX) {
						nec_state = NEC_STATE_IDLE;
					} else {
						expecting_low = 0; // next is the high (data)
					}
				} else {
					// High duration → decode bit 0/1
					if (delta >= NEC_ZERO_HIGH_MIN && delta <= NEC_ZERO_HIGH_MAX) {
						// bit 0, do nothing
					} else if (delta >= NEC_ONE_HIGH_MIN && delta <= NEC_ONE_HIGH_MAX) {
						nec_code |= (1UL << bit_index);
					} else {
						// not reached
						nec_state = NEC_STATE_IDLE;
						break;
					}

					// we get 32 bits
					bit_index++;

					if (bit_index >= 32) {
						Debug::Pin0::high();
						// if (nec_validate(nec_code)) {
						// 	Debug::Pin1::high();
						decode(nec_code);
						// Debug::Pin1::low();
						// }
						nec_state = NEC_STATE_IDLE;
						expecting_low = 0;
						bit_index = 0;
						nec_code = 0;
						Debug::Pin0::low();
					} else {
						expecting_low = 1; // next should be low of next bit
					}
				}
				break;
		}
	}

	void decode(uint32_t code) {
		if (code == Code::Up) {
			last_event = Event::VolumeUp;
		}

		else if (code == Code::Down)
		{
			last_event = Event::VolumeDown;
		else if (code == Code::Mute)
		{
			last_command = Event::Mute;
		}

		else
		{
			printf_("Unknown code %x\n", code);
		}
	}

	Event get_event() {
		return std::exchange(last_event, Event::None);
	}
};

} // namespace RemoteVolume
