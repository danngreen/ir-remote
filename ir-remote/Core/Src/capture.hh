#include "nec.hh"
#include "stm32g4xx_hal.h"
#include "util/callable.hh"
#include <cstdint>

struct IrCapture {

	IrCapture(TIM_HandleTypeDef &htim, unsigned channel, auto cb)
		: callback{cb}
		, htim{&htim}
		, channel{channel} {
		_instance = this;
	}

	void process() {
		uint16_t now = __HAL_TIM_GET_COMPARE(htim, channel);
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
						nec_state = NEC_STATE_IDLE;
						break;
					}

					bit_index++;
					if (bit_index >= 32) {
						if (nec_validate(nec_code)) {
							callback(nec_code);
						}
						nec_state = NEC_STATE_IDLE;
						expecting_low = 0;
						bit_index = 0;
						nec_code = 0;
					} else {
						expecting_low = 1; // next should be low of next bit
					}
				}
				break;
		}
	}

	static bool nec_validate(uint32_t code) {
		uint8_t addr = code & 0xFF;
		uint8_t addr_inv = (code >> 8) & 0xFF;
		uint8_t cmd = (code >> 16) & 0xFF;
		uint8_t cmd_inv = (code >> 24) & 0xFF;

		if ((addr ^ addr_inv) != 0xFF || (cmd ^ cmd_inv) != 0xFF) {
			// checksum failed
			return false;
		}

		return true;
	}

	Function<void(uint32_t)> callback;
	static inline IrCapture *_instance;

private:
	TIM_HandleTypeDef *htim;
	unsigned channel;

	enum nec_state_t { NEC_STATE_IDLE = 0, NEC_STATE_LEAD_LOW, NEC_STATE_LEAD_HIGH, NEC_STATE_BITS };
	nec_state_t nec_state = NEC_STATE_IDLE;
	uint16_t last_capture = 0;
	uint8_t expecting_low = 0;
	uint8_t bit_index = 0;
	uint32_t nec_code = 0;
};

extern "C" void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *) {
	IrCapture::_instance->process();
}
